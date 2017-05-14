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


def purge_m_files(ids=[], opts={}):
    if len(ids) > 0:
        remove_m_files(ids, opts)

        if opts['dry_run'] is False:
            # Set deleted=1 for aged metadata entries

            cursor = opts['db'].cursor()
            format = ", ".join(['%s'] * len(ids))
            cursor.execute("UPDATE metadata SET deleted=1 WHERE piler_id IN " +
                           "(%s)" % (format), ids)
            opts['db'].commit()


def purge_attachments(ids=[], opts={}):
    format = ", ".join(['%s'] * len(ids))

    cursor = opts['db'].cursor()

    # Select non referenced attachments
    cursor.execute("SELECT i, piler_id, attachment_id FROM v_attachment " +
                   "WHERE refcount=0 AND piler_id IN (%s)" % (format), ids)

    while True:
        rows = cursor.fetchall()
        if rows == ():
            break

        remove_ids = []

        # Delete attachments from filesystem
        for (id, piler_id, attachment_id) in rows:
            remove_ids.append(id)

            if opts['dry_run'] is False:
                unlink(get_attachment_file_path(piler_id, attachment_id, opts))
            else:
                print get_attachment_file_path(piler_id, attachment_id, opts)

        # Delete these IDs from attachment table
        if opts['dry_run'] is False:
            format = ", ".join(['%s'] * len(remove_ids))
            cursor.execute("DELETE FROM attachment WHERE id IN (%s)" %
                           (format), remove_ids)
            opts['db'].commit()
        else:
            print remove_ids


def remove_m_files(ids=[], opts={}):
    for i in range(0, len(ids)):
        if opts['dry_run'] is False:
            unlink(get_m_file_path(ids[i], opts), opts)
        else:
            print get_m_file_path(ids[i], opts)


def unlink(filename="", opts={}):
    try:
        st = os.stat(filename)
        opts['purged_stored_size'] = opts['purged_stored_size'] + st.st_size
        opts['count'] = opts['count'] + 1
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
    parser = argparse.ArgumentParser()
    parser.add_argument("-c", "--config", type=str, help="piler.conf path",
                        default="/usr/local/etc/piler/piler.conf")
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
    opts['db'] = None
    opts['count'] = 0
    opts['size'] = 0
    opts['purged_size'] = 0
    opts['purged_stored_size'] = 0

    syslog.openlog(logoption=syslog.LOG_PID, facility=syslog.LOG_MAIL)

    read_options(args.config, opts)
    try:
        opts['db'] = dbapi.connect("localhost", opts['username'],
                                   opts['password'], opts['database'])

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

            purge_attachments(piler_id, opts)

        # Update the counter table
        if opts['dry_run'] is False:
            cursor.execute("UPDATE counter SET rcvd=rcvd-%s, size=size-%s, " +
                           "stored_size=stored_size-%s",
                           (str(opts['count']), str(opts['purged_size']),
                            str(opts['purged_stored_size'])))
            opts['db'].commit()

    except dbapi.DatabaseError, e:
        print "Error %s" % e

    if opts['db']:
        opts['db'].close()

    syslog.syslog("purged " + str(opts['count']) + " files, " +
                  str(opts['purged_size']) + "/" +
                  str(opts['purged_stored_size']) + " bytes")


if __name__ == "__main__":
    main()
