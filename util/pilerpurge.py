#!/usr/bin/python

import ConfigParser
import MySQLdb as dbapi
import StringIO
import argparse
import getpass
import os
import sys
import syslog
import time

SQL_PURGE_SELECT_QUERY = "SELECT piler_id, size FROM " +\
    "metadata WHERE deleted=0 AND retained < UNIX_TIMESTAMP(NOW()) " +\
    "AND id NOT IN (SELECT id FROM rcpt WHERE `to` IN " +\
    "(SELECT email FROM legal_hold)) AND id NOT IN (SELECT " +\
    "id FROM metadata WHERE `from` IN (SELECT email FROM " +\
    "legal_hold))"

opts = {}
default_conf = "/usr/local/etc/piler/piler.conf"


def read_options(filename="", opts={}):
    s = "[piler]\n" + open(filename, 'r').read()
    fp = StringIO.StringIO(s)
    config = ConfigParser.RawConfigParser()
    config.readfp(fp)

    opts['username'] = config.get('piler', 'mysqluser')
    opts['password'] = config.get('piler', 'mysqlpwd')
    opts['database'] = config.get('piler', 'mysqldb')
    opts['storedir'] = config.get('piler', 'queuedir')

    opts['server_id'] = "%02x" % config.getint('piler', 'server_id')


def is_purge_enabled(opts={}):
    cursor = opts['db'].cursor()

    cursor.execute("SELECT `value` FROM `option` WHERE `key`='enable_purge'")

    row = cursor.fetchone()
    if row and row[0] == '1':
        return True

    return False


def purge_m_files(ids=[], opts={}):
    if len(ids) > 0:
        remove_m_files(ids, opts)

        # Set deleted=1 for aged metadata entries

        if opts['dry_run'] is False:
            cursor = opts['db'].cursor()
            format = ", ".join(['%s'] * len(ids))
            cursor.execute("UPDATE metadata SET deleted=1 WHERE piler_id IN " +
                           "(%s)" % (format), ids)
            opts['db'].commit()


def purge_attachments_by_piler_id(ids=[], opts={}):
    format = ", ".join(['%s'] * len(ids))

    cursor = opts['db'].cursor()

    cursor.execute("SELECT i, piler_id, attachment_id, refcount FROM " +
                   "v_attachment WHERE piler_id IN (%s)" % (format), ids)

    while True:
        rows = cursor.fetchall()
        if rows == ():
            break
        else:
            remove_attachment_files(rows, opts)


def purge_attachments_by_attachment_id(opts={}):
    format = ", ".join(['%s'] * len(opts['referenced_attachments']))

    cursor = opts['db'].cursor()

    cursor.execute("SELECT i, piler_id, attachment_id, refcount FROM " +
                   "v_attachment WHERE i IN (%s)" %
                   (format), opts['referenced_attachments'])

    while True:
        rows = cursor.fetchall()
        if rows == ():
            break
        else:
            remove_attachment_files(rows, opts)


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
                print get_attachment_file_path(piler_id, attachment_id, opts)
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
            print remove_ids

    opts['referenced_attachments'] = referenced_ids


def remove_m_files(ids=[], opts={}):
    for i in range(0, len(ids)):
        if opts['dry_run'] is False:
            unlink(get_m_file_path(ids[i], opts), opts)
            opts['messages'] = opts['messages'] + 1
        else:
            print get_m_file_path(ids[i], opts)


def unlink(filename="", opts={}):
    if opts['verbose']:
        print "removing", filename

    try:
        st = os.stat(filename)
        opts['purged_stored_size'] = opts['purged_stored_size'] + st.st_size
        opts['files'] = opts['files'] + 1
        os.unlink(filename)
    except:
        pass


def get_m_file_path(id='', opts={}):
    return "/".join([opts['storedir'], opts['server_id'], id[8:11], id[32:34],
                    id[34:36], id + ".m"])


def get_attachment_file_path(piler_id='', attachment_id=0, opts={}):
    return "/".join([opts['storedir'], opts['server_id'], piler_id[8:11],
                    piler_id[32:34], piler_id[34:36], piler_id + ".a" +
                    str(attachment_id)])


def main():
    if "/usr/libexec" in __file__:
        default_conf = "/etc/piler/piler.conf"

    parser = argparse.ArgumentParser()
    parser.add_argument("-c", "--config", type=str, help="piler.conf path",
                        default="/etc/piler/piler.conf")
    parser.add_argument("-b", "--batch-size", type=int, help="batch size " +
                        "to delete", default=1000)
    parser.add_argument("-d", "--dry-run", help="dry run", action='store_true')
    parser.add_argument("-v", "--verbose", help="verbose mode",
                        action='store_true')

    args = parser.parse_args()

    if getpass.getuser() not in ['root', 'piler']:
        print "Please run me as user 'piler'"
        sys.exit(1)

    opts['dry_run'] = args.dry_run
    opts['verbose'] = args.verbose
    opts['db'] = None
    opts['messages'] = 0
    opts['files'] = 0
    opts['size'] = 0
    opts['purged_size'] = 0
    opts['purged_stored_size'] = 0
    opts['referenced_attachments'] = []

    syslog.openlog(logoption=syslog.LOG_PID, facility=syslog.LOG_MAIL)

    read_options(args.config, opts)
    try:
        opts['db'] = dbapi.connect("localhost", opts['username'],
                                   opts['password'], opts['database'])

        if is_purge_enabled(opts) is False:
            syslog.syslog("Purging emails is disabled")
            sys.exit(1)

        cursor = opts['db'].cursor()
        cursor.execute(SQL_PURGE_SELECT_QUERY)

        while True:
            rows = cursor.fetchmany(args.batch_size)
            if rows == ():
                break

            piler_id = [x[0] for x in rows]
            size = [x[1] for x in rows]

            opts['purged_size'] = opts['purged_size'] + sum(size)

            purge_m_files(piler_id, opts)
            purge_attachments_by_piler_id(piler_id, opts)

            # It's possible that there's attachment duplication, thus
            # refcount > 0, even though after deleting the duplicates
            # (references) refcount becomes zero.
            if len(opts['referenced_attachments']) > 0:
                purge_attachments_by_attachment_id(opts)

        # Update the counter table
        if opts['dry_run'] is False:
            cursor.execute("UPDATE counter SET rcvd=rcvd-%s, size=size-%s, " +
                           "stored_size=stored_size-%s",
                           (str(opts['messages']), str(opts['purged_size']),
                            str(opts['purged_stored_size'])))
            opts['db'].commit()

    except dbapi.DatabaseError, e:
        print "Error %s" % e

    if opts['db']:
        opts['db'].close()

    summary = "Purged " + str(opts['messages']) + " messages, " +\
        str(opts['files']) + " files, " +\
        str(opts['purged_size']) + "/" +\
        str(opts['purged_stored_size']) + " bytes"

    if opts['verbose']:
        print summary

    syslog.syslog(summary)


if __name__ == "__main__":
    main()
