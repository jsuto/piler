#!/usr/bin/python3
# -*- coding: utf-8 -*-

import MySQLdb as dbapi
import argparse
import configparser
import fnmatch
import imaplib
import os
import re
import subprocess
import sys

opts = {}
INBOX = 'INBOX'
ST_RUNNING = 1

imaplib._MAXLINE = 10000000

def generate_auth_string(user, token):
    auth_string = f"user={user}\1auth=Bearer {token}\1\1"
    return auth_string


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


def read_folder_list(conn):
    result = []

    rc, folders = conn.list()
    if opts['verbose']:
        print("Folders:", folders)

    for folder in folders:
        if opts['verbose']:
            print("Got folder", folder)

        if isinstance(folder, type(b'')):
            folder = folder.decode('utf-8')
        elif isinstance(folder, type(())):
            folder = re.sub(r'\{\d+\}$', '',
                            folder[0].decode('utf-8')) + folder[1].decode('utf-8')

        # The regex should match ' "/" ' and ' "." '
        if folder:
            f = re.split(r' \"[\/\.\\]+\" ', folder)
            result.append(f[1])

    return [x for x in result if not any(fnmatch.fnmatch(x.lower(), p.lower()) for p in opts['skip_folders'])]


def process_folder(conn, folder):
    # Space in the folder name must be escaped
    folder = re.sub(r' ', '\\ ', folder)

    if opts['verbose']:
        print("Processing {}".format(folder))

    try:
        readonly = True
        if opts['remove']:
            readonly = False
        rc, data = conn.select(folder, readonly=readonly)
    except:
        print("Error processing folder {}".format(folder))
        return

    if rc != "OK":
        print("Error processing folder {}, rc={}, response={}".format(folder,
                                                                      rc, data))
        return

    n = int(data[0])
    if opts['verbose']:
        print("Folder {} has {} messages".format(folder, n))

    if n > 0:
        if opts['id']:
            cursor = opts['db'].cursor()
            data = (ST_RUNNING, n, opts['id'])
            cursor.execute("UPDATE import SET status=%s, total=total+%s WHERE id=%s",
                           data)
            opts['db'].commit()

        rc, data = conn.search(None, opts['search'])
        for num in data[0].split():
            rc, data = conn.fetch(num, '(RFC822)')
            if opts['verbose']:
                print(rc, num)
            if isinstance(data[0], tuple):
                opts['counter'] += 1
                with open("{}.eml".format(opts['counter']), "wb") as f:
                    f.write(data[0][1])
                    if opts['remove']:
                        conn.store(num, '+FLAGS', '\\Deleted')

        if opts['remove']:
            conn.expunge()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-c", "--config", type=str, help="piler.conf path",
                        default="/etc/piler/piler.conf")
    parser.add_argument("--security", type=str, help="imap security. no, TLS or SSL", default="no")
    parser.add_argument("-s", "--server", type=str, help="imap server")
    parser.add_argument("-P", "--port", type=int, help="port number", default=143)
    parser.add_argument("-u", "--user", type=str, help="imap user")
    parser.add_argument("-p", "--password", type=str, help="imap password")
    parser.add_argument("--oauth2-token", type=str, help="oauth2 access token file")
    parser.add_argument("-x", "--skip-list", type=str, help="IMAP folders to skip",
                        default="junk,trash,spam,draft,\"[Gmail]\"")
    parser.add_argument("-f", "--folders", type=str,
                        help="Comma separated list of IMAP folders to download")
    parser.add_argument("--date", type=str, help="Search before/since a given date," +
                        "eg. (BEFORE \"01-Jan-2020\") or (SINCE \"01-Jan-2020\")")
    parser.add_argument("-d", "--dir", help="directory to chdir",
                        default="/var/piler/imap")
    parser.add_argument("-i", "--import-from-table", action='store_true',
                        help="Read imap conn data from import table")
    parser.add_argument("-r", "--remove", help="remove downloaded messages", action='store_true')
    parser.add_argument("-v", "--verbose", help="verbose mode", action='store_true')

    args = parser.parse_args()

    os.chdir(args.dir)

    if not bool(args.import_from_table or args.server):
        print("Please specify either --import-from-table or --server <imap host>")
        sys.exit(1)

    opts['skip_folders'] = args.skip_list.split(',')
    opts['verbose'] = args.verbose
    opts['search'] = 'ALL'
    opts['counter'] = 0
    opts['db'] = None
    opts['id'] = 0
    opts['access_token'] = ''
    opts['remove'] = args.remove

    if args.date:
        opts['search'] = args.date

    if args.oauth2_token:
        with open(args.oauth2_token, 'r') as f:
            opts['access_token'] = f.read()

    security = ''
    server = ''
    user = ''
    password = ''

    if args.import_from_table:
        read_options(args.config, opts)
        try:
            opts['db'] = dbapi.connect(opts['dbhost'], opts['username'],
                                       opts['password'], opts['database'])

            cursor = opts['db'].cursor()
            cursor.execute("SELECT id, type, server, username, password " +
                           "FROM import WHERE started=0")

            row = cursor.fetchone()
            if row:
                (opts['id'], security, server, user, password) = row
                security = row[1]
            else:
                print("Nothing to read from import table")
                sys.exit(0)

        except dbapi.DatabaseError as e:
            print("Error %s" % e)
    else:
        security = args.security
        server = args.server
        user = args.user
        password = args.password

    if opts['verbose']:
        print("Skipped folder list: {}".format(opts['skip_folders']))

    if security == 'no':
        conn = imaplib.IMAP4(server)
    elif security == 'imap-ssl':
        conn = imaplib.IMAP4_SSL(server)
    elif security == 'imap-tls':
        conn = imaplib.IMAP4(server)
        conn.starttls()

    if opts['access_token']:
        conn.authenticate("XOAUTH2", lambda x: generate_auth_string(
            user, opts['access_token']))
    else:
        conn.login(user, password)

    conn.select()

    if args.folders:
        folders = args.folders.split(',')
    else:
        folders = read_folder_list(conn)

    if opts['verbose']:
        print("Folders will be processed: {}".format(folders))

    for folder in folders:
        process_folder(conn, folder)

    conn.close()

    if opts['db']:
        if opts['id']:
            subprocess.call(["pilerimport",
                             "-d", args.dir,
                             "-r",
                             "-T", str(opts['id'])])
        opts['db'].close()

    print("Processed {} messages".format(opts['counter']))


if __name__ == "__main__":
    main()
