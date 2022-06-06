#!/usr/bin/python3

import configparser
import MySQLdb as dbapi
import argparse
import getpass
import os
import sys
import syslog
import time

SQL_SELECT_QUERY = "SELECT id, `from`, `to`, fromdomain, todomain, subject, " + \
                   "arrived, sent, body, size, direction, folder, attachments, " + \
                   "attachment_types FROM sph_index"

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

    # FIXME
    opts['sphx_host'] = "127.0.0.1"
    opts['sphx_port'] = 9306


def process_messages(opts):
    opts['db'] = dbapi.connect(opts['dbhost'], opts['username'],
                               opts['password'], opts['database'])

    opts['sphx'] = dbapi.connect(host=opts['sphx_host'],
                                 port=opts['sphx_port'])

    cursor = opts['db'].cursor()
    s_cursor = opts['sphx'].cursor()

    cursor.execute(SQL_SELECT_QUERY)
    rows = cursor.fetchmany(opts['batch_size'])

    if rows:
        ids = [x[0] for x in rows]
        syslog.syslog("Processed %d items" % (len(ids)))

        s_cursor.executemany("""
            INSERT INTO piler1 (id, sender, rcpt, senderdomain, rcptdomain,
            subject, arrived, sent, body, size, direction, folder, attachments,
            attachment_types) VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s,
            %s, %s, %s, %s)""", rows)

        format = ", ".join(['%s'] * len(ids))
        cursor.execute("DELETE FROM sph_index WHERE id IN (%s)" % (format), ids)
        opts['db'].commit()

    else:
        time.sleep(opts['sleep'])

    if opts['db']:
        opts['db'].close()

    if opts['sphx']:
        opts['sphx'].close()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-c", "--config", type=str, help="piler.conf path",
                        default="/etc/piler/piler.conf")
    parser.add_argument("-b", "--batch-size", type=int, help="batch size " +
                        "to process", default=1000)
    parser.add_argument("-s", "--sleep", type=int, help="sleep after no data " +
                        "to index", default=5)
    parser.add_argument("-d", "--dry-run", help="dry run", action='store_true')
    parser.add_argument("-v", "--verbose", help="verbose mode",
                        action='store_true')

    args = parser.parse_args()

    if getpass.getuser() not in ['root', 'piler']:
        print("Please run me as user 'piler'")
        sys.exit(1)

    opts['dry_run'] = args.dry_run
    opts['verbose'] = args.verbose
    opts['batch_size'] = args.batch_size
    opts['sleep'] = args.sleep
    opts['db'] = None
    opts['sphx'] = None

    syslog.openlog(logoption=syslog.LOG_PID, facility=syslog.LOG_MAIL)

    read_options(args.config, opts)

    while True:
        process_messages(opts)


if __name__ == "__main__":
    main()
