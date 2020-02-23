#!/usr/bin/python3
# -*- coding: utf-8 -*-

import argparse
import imaplib
import pprint
import re

opts = {}
INBOX = 'INBOX'


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
            folder = re.sub(r'\{\d+\}$', '', folder[0]) + folder[1]

        # The regex should match ' "/" ' and ' "." '
        if folder:
            f = re.split(r' \"[\/\.]\" ', folder)
            result.append(f[1])

    return [x for x in result if x not in opts['skip_folders']]


def process_folder(conn, folder):
    print("Processing {}".format(folder))

    rc, data = conn.select(folder)
    n = int(data[0])
    print("Folder {} has {} messages".format(folder, n))

    if n > 0:
        rc, data = conn.search(None, 'ALL')
        for num in data[0].split():
            rc, data = conn.fetch(num, '(RFC822)')
            if opts['verbose']:
                print(rc, num)
            opts['counter'] = opts['counter'] + 1
            with open("{}.eml".format(opts['counter']), "wb") as f:
                f.write(data[0][1])


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-s", "--server", type=str, help="imap server", required=True)
    parser.add_argument("-P", "--port", type=int, help="port number", default=143)
    parser.add_argument("-u", "--user", type=str, help="imap user", required=True)
    parser.add_argument("-p", "--password", type=str, help="imap password",
                        required=True)
    parser.add_argument("-x", "--skip-list", type=str, help="IMAP folders to skip",
                        default="junk,trash,spam,draft")
    parser.add_argument("-f", "--folders", type=str,
                        help="Comma separated list of IMAP folders to download")
    parser.add_argument("-v", "--verbose", help="verbose mode", action='store_true')

    args = parser.parse_args()

    opts['skip_folders'] = args.skip_list.split(',')
    opts['verbose'] = args.verbose
    opts['counter'] = 0

    if opts['verbose']:
        print("Skipped folder list: {}".format(opts['skip_folders']))

    if args.port == 993:
        conn = imaplib.IMAP4_SSL(args.server)
    else:
        conn = imaplib.IMAP4(args.server)

    conn.login(args.user, args.password)
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


if __name__ == "__main__":
    main()
