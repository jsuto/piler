#!/usr/bin/python3

# Performance optimizations for large archives (600GB+ DB, 40TB+ storage):
#
# 1. SELECT query: NOT IN -> LEFT JOIN ... IS NULL + LIMIT
#    The old query used nested NOT IN subqueries against rcpt and metadata
#    for legal_hold exclusion, causing full table scans. Now uses LEFT JOIN
#    with IS NULL checks, which MySQL can optimize with index lookups.
#    Added LIMIT (bound to batch_size) so each query returns a bounded result.
#
# 2. Main loop: unbounded cursor -> re-query per chunk
#    Instead of running one huge SELECT and iterating with fetchmany(),
#    each iteration runs a fresh LIMIT-ed query, processes the batch,
#    and loops. Since each batch sets deleted=1, the next query's
#    WHERE deleted=0 naturally excludes already-processed rows.
#
# 3. DELETE statements: eliminated 5 redundant subqueries per batch
#    Each DELETE used to re-resolve piler_ids via
#    SELECT id FROM metadata WHERE piler_id IN (...). Now passes both
#    numeric meta_ids and piler_ids, using numeric ids directly.
#
# 4. Attachment queries: stopped using v_attachment view
#    The v_attachment view computes a correlated COUNT(*) subquery for
#    every row in the attachment table. Now queries attachment directly
#    with the piler_id filter, so the subquery only runs for matching rows.

import configparser
import MySQLdb as dbapi
import argparse
import getpass
import os
import sys
import syslog
import time

# Use LEFT JOIN instead of NOT IN for legal_hold exclusion.
# NOT IN with subqueries causes full table scans on large tables.
# Process in bounded chunks via LIMIT to avoid unbounded result sets.
SQL_PURGE_SELECT_QUERY = (
    "SELECT m.id, m.piler_id, m.size FROM metadata m "
    "LEFT JOIN rcpt r_lh ON m.id = r_lh.id "
    "AND r_lh.`to` IN (SELECT email FROM legal_hold) "
    "LEFT JOIN legal_hold lh_from ON m.`from` = lh_from.email "
    "WHERE m.deleted=0 AND m.retained < UNIX_TIMESTAMP(NOW()) "
    "AND r_lh.id IS NULL AND lh_from.email IS NULL "
    "LIMIT %s"
)

opts = {}


def read_options(filename="", opts={}):
    s = "[piler]\n" + open(filename, 'r').read()
    config = configparser.ConfigParser()
    config.read_string(s)

    if config.has_option('piler', 'mysqlhost'):
        opts['dbhost'] = config.get('piler', 'mysqlhost')
    else:
        opts['dbhost'] = 'localhost'

    opts['username'] = config.get('piler', 'mysqluser')
    opts['password'] = config.get('piler', 'mysqlpwd')
    opts['database'] = config.get('piler', 'mysqldb')
    opts['storedir'] = config.get('piler', 'queuedir')
    opts['rtindex'] = config.getint('piler', 'rtindex', fallback=0)
    opts['sphxdb'] = config.get('piler', 'sphxdb', fallback='piler1')
    opts['sphxhost'] = config.get('piler', 'sphxhost', fallback='127.0.0.1')
    opts['sphxport'] = config.getint('piler', 'sphxport', fallback=9306)
    opts['server_id'] = "%02x" % config.getint('piler', 'server_id')


def is_purge_enabled(opts={}):
    cursor = opts['db'].cursor()

    cursor.execute("SELECT `value` FROM `option` WHERE `key`='enable_purge'")

    row = cursor.fetchone()
    if row and row[0] == '1':
        return True

    return False


def purge_m_files(meta_ids=[], piler_ids=[], opts={}):
    if len(piler_ids) == 0:
        return

    # First, delete files from disk (outside the DB transaction)
    remove_m_files(piler_ids, opts)

    if opts['dry_run'] is False:
        cursor = opts['db'].cursor()

        # Use the already-resolved numeric ids directly instead of
        # repeating SELECT id FROM metadata WHERE piler_id IN (...)
        # in every statement.

        id_format = ", ".join(['%s'] * len(meta_ids))
        piler_id_format = ", ".join(['%s'] * len(piler_ids))

        cursor.execute("UPDATE metadata SET deleted=1, subject=NULL, `from`='',"
                       "fromdomain='', message_id='' WHERE piler_id IN "
                       "(%s)" % (piler_id_format), piler_ids)

        cursor.execute("DELETE FROM rcpt WHERE id IN (%s)" % (id_format), meta_ids)
        cursor.execute("DELETE FROM note WHERE id IN (%s)" % (id_format), meta_ids)
        cursor.execute("DELETE FROM tag WHERE id IN (%s)" % (id_format), meta_ids)
        cursor.execute("DELETE FROM private WHERE id IN (%s)" % (id_format), meta_ids)
        cursor.execute("DELETE FROM folder_message WHERE id IN (%s)" % (id_format), meta_ids)

        opts['db'].commit()


def purge_attachments_by_piler_id(piler_ids=[], opts={}):
    format = ", ".join(['%s'] * len(piler_ids))

    cursor = opts['db'].cursor()

    # Avoid the v_attachment view which has a correlated subquery
    # computing refcount for every row. Instead, get the attachment
    # rows directly and compute refcount only for the rows we need.
    cursor.execute(
        "SELECT a.id, a.piler_id, a.attachment_id, "
        "(SELECT COUNT(*) FROM attachment a2 WHERE a2.ptr = a.id) AS refcount "
        "FROM attachment a WHERE a.piler_id IN (%s)" % (format), piler_ids)

    rows = cursor.fetchall()
    if rows:
        remove_attachment_files(rows, opts)


def purge_attachments_by_attachment_id(opts={}):
    if not opts['referenced_attachments']:
        return

    format = ", ".join(['%s'] * len(opts['referenced_attachments']))

    cursor = opts['db'].cursor()

    cursor.execute(
        "SELECT a.id, a.piler_id, a.attachment_id, "
        "(SELECT COUNT(*) FROM attachment a2 WHERE a2.ptr = a.id) AS refcount "
        "FROM attachment a WHERE "
        "(SELECT COUNT(*) FROM attachment a2 WHERE a2.ptr = a.id) = 0 "
        "AND a.id IN (%s)" % (format), opts['referenced_attachments'])

    rows = cursor.fetchall()
    if rows:
        for (id, piler_id, attachment_id, refcount) in rows:
            if opts['dry_run'] is False:
                unlink(get_attachment_file_path(piler_id, attachment_id,
                                                opts), opts)
            else:
                print(get_attachment_file_path(piler_id, attachment_id, opts))


def remove_attachment_files(rows=(), opts={}):
    remove_ids = []
    referenced_ids = []

    if rows == ():
        return

    # If refcount > 0, then save attachment.id, and handle later,
    # otherwise delete the attachment from the filesystem, and
    # attachment table

    for (id, piler_id, attachment_id, refcount) in rows:
        if refcount == 0:
            remove_ids.append(id)

            if opts['dry_run'] is False:
                unlink(get_attachment_file_path(piler_id, attachment_id,
                                                opts), opts)
            else:
                print(get_attachment_file_path(piler_id, attachment_id, opts))
        else:
            referenced_ids.append(id)

    if remove_ids:
        if opts['dry_run'] is False:
            format = ", ".join(['%s'] * len(remove_ids))
            cursor = opts['db'].cursor()
            cursor.execute("DELETE FROM attachment WHERE id IN (%s)" %
                           (format), remove_ids)
            opts['db'].commit()
        else:
            print(remove_ids)

    opts['referenced_attachments'] = referenced_ids


def remove_m_files(ids=[], opts={}):
    for i in range(0, len(ids)):
        if opts['dry_run'] is False:
            unlink(get_m_file_path(ids[i], opts), opts)
            opts['messages'] = opts['messages'] + 1
        else:
            print(get_m_file_path(ids[i], opts))


def unlink(filename="", opts={}):
    if opts['verbose']:
        print("removing", filename)

    try:
        st = os.stat(filename)
        opts['purged_stored_size'] = opts['purged_stored_size'] + st.st_size
        opts['files'] = opts['files'] + 1
        os.unlink(filename)
    except:
        pass


def get_m_file_path(id='', opts={}):
    return "/".join([opts['storedir'], id[24:26], id[8:11], id[32:34],
                    id[34:36], id + ".m"])


def get_attachment_file_path(piler_id='', attachment_id=0, opts={}):
    return "/".join([opts['storedir'], piler_id[24:26], piler_id[8:11],
                    piler_id[32:34], piler_id[34:36], piler_id + ".a" +
                    str(attachment_id)])


def purge_index_data(ids=[], opts={}):
    '''
    Delete from index data in case of RT index
    '''

    if opts['rtindex'] == 1 and opts['dry_run'] is False:
        cursor = opts['sphx'].cursor()
        a = ",".join([str(x) for x in ids])
        cursor.execute("DELETE FROM %s WHERE id IN (%s)" % (opts['sphxdb'], a))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-c", "--config", type=str, help="piler.conf path",
                        default="/etc/piler/piler.conf")
    parser.add_argument("-b", "--batch-size", type=int, help="batch size "
                        "to delete", default=1000)
    parser.add_argument("-d", "--dry-run", help="dry run", action='store_true')
    parser.add_argument("-v", "--verbose", help="verbose mode",
                        action='store_true')

    args = parser.parse_args()

    if getpass.getuser() not in ['root', 'piler']:
        print("Please run me as user 'piler'")
        sys.exit(1)

    opts['dry_run'] = args.dry_run
    opts['verbose'] = args.verbose
    opts['db'] = None
    opts['sphx'] = None
    opts['messages'] = 0
    opts['files'] = 0
    opts['size'] = 0
    opts['purged_size'] = 0
    opts['purged_stored_size'] = 0
    opts['referenced_attachments'] = []

    syslog.openlog(logoption=syslog.LOG_PID, facility=syslog.LOG_MAIL)

    read_options(args.config, opts)
    try:
        opts['db'] = dbapi.connect(opts['dbhost'], opts['username'],
                                   opts['password'], opts['database'])

        opts['sphx'] = dbapi.connect(host=opts['sphxhost'], port=opts['sphxport'])

        if is_purge_enabled(opts) is False:
            syslog.syslog("Purging emails is disabled")
            sys.exit(1)

        # Process in chunks. Each iteration runs a bounded LIMIT query,
        # processes one batch_size worth of rows, then loops. This avoids
        # holding a huge unbounded cursor open and keeps lock durations short.

        while True:
            cursor = opts['db'].cursor()
            cursor.execute(SQL_PURGE_SELECT_QUERY, (args.batch_size,))

            rows = cursor.fetchall()
            cursor.close()

            if not rows:
                break

            meta_ids = [x[0] for x in rows]
            piler_ids = [x[1] for x in rows]
            sizes = [x[2] for x in rows]

            opts['purged_size'] = opts['purged_size'] + sum(sizes)

            purge_m_files(meta_ids, piler_ids, opts)
            purge_attachments_by_piler_id(piler_ids, opts)
            purge_index_data(meta_ids, opts)

            # It's possible that there's attachment duplication, thus
            # refcount > 0, even though after deleting the duplicates
            # (references) refcount becomes zero.
            if len(opts['referenced_attachments']) > 0:
                purge_attachments_by_attachment_id(opts)

            syslog.syslog("Purged batch of %d messages" % len(meta_ids))

        # Update the counter table
        if opts['dry_run'] is False:
            cursor = opts['db'].cursor()
            cursor.execute("UPDATE counter SET rcvd=rcvd-%s, size=size-%s, "
                           "stored_size=stored_size-%s",
                           (str(opts['messages']), str(opts['purged_size']),
                            str(opts['purged_stored_size'])))
            opts['db'].commit()

    except dbapi.DatabaseError as e:
        print("Error %s" % e)

    if opts['db']:
        opts['db'].close()

    summary = "Purged " + str(opts['messages']) + " messages, " +\
        str(opts['files']) + " files, " +\
        str(opts['purged_size']) + "/" +\
        str(opts['purged_stored_size']) + " bytes"

    if opts['verbose']:
        print(summary)

    syslog.syslog(summary)


if __name__ == "__main__":
    main()
