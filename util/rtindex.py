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
SQL_INSERT_QUERY = "INSERT INTO piler1 (id, sender, rcpt, senderdomain, " + \
                   "rcptdomain, subject, arrived, sent, body, size, direction, " + \
                   "folder, attachments, attachment_types) VALUES (%s, %s, %s, " + \
                   "%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)"
SQL_DELETE_QUERY = "DELETE FROM sph_index WHERE id IN (%s)"
SLEEP_DELAY = 5

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


def process_batch(opts):
    try:
        opts['db'] = dbapi.connect(opts['dbhost'], opts['username'],
                                   opts['password'], opts['database'])

        cursor = opts['db'].cursor()

        while True:
            cursor.execute(SQL_SELECT_QUERY)
            rows = cursor.fetchmany(opts['batch_size'])
            if rows == ():
                time.sleep(SLEEP_DELAY)
                break

            ids = [x[0] for x in rows]

            opts['sphx'] = dbapi.connect(host=opts['sphinx_host'],
                                         port=opts['sphinx_port'])
            sphx_cursor = opts['sphx'].cursor()

            sphx_cursor.executemany(SQL_INSERT_QUERY, rows)
            opts['sphx'].commit()
            opts['sphx'].close()

            syslog.syslog("%d records inserted" % (sphx_cursor.rowcount))

            format = ", ".join(['%s'] * len(ids))
            cursor.execute(SQL_DELETE_QUERY % (format), ids)
            opts['db'].commit()

    except dbapi.DatabaseError as e:
        syslog.syslog("Error %s" % e)
        time.sleep(SLEEP_DELAY)

    if opts['db']:
        opts['db'].close()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-c", "--config", type=str, help="piler.conf path",
                        default="/etc/piler/piler.conf")
    parser.add_argument("-b", "--batch-size", type=int, help="batch size " +
                        "to process", default=1000)
    parser.add_argument("-s", "--sphinx", type=str, help="sphinx server",
                        default="127.0.0.1")
    parser.add_argument("-p", "--port", type=int, help="sphinx sql port",
                        default=9306)
    parser.add_argument("-d", "--dry-run", help="dry run", action='store_true')
    parser.add_argument("-v", "--verbose", help="verbose mode",
                        action='store_true')

    args = parser.parse_args()

    if getpass.getuser() not in ['root', 'piler']:
        print("Please run me as user 'piler'")
        sys.exit(1)

    opts['dry_run'] = args.dry_run
    opts['verbose'] = args.verbose
    opts['sphinx_host'] = args.sphinx
    opts['sphinx_port'] = args.port
    opts['batch_size'] = args.batch_size
    opts['db'] = None
    opts['sphx'] = None

    syslog.openlog(logoption=syslog.LOG_PID, facility=syslog.LOG_MAIL)

    read_options(args.config, opts)

    while True:
        process_batch(opts)


if __name__ == "__main__":
    main()
