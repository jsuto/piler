#!/usr/bin/python
# -*- coding: utf-8 -*-

import argparse
import imaplib
import pprint

opts = {}
INBOX = 'INBOX'


def read_folder_list(conn):
    result = []

    rc, folders = conn.list()
    for folder in folders:
        f = folder.decode('utf-8').split(' "." ')
        result.append(f[1])

    return [x for x in result if x not in opts['skip_folders']]


def process_folder(conn, folder):
    rc, data = conn.select(folder)
    n = int(data[0])
    print("Folder {} has {} messages".format(folder, n))

    if n > 0:
        rc, data = conn.search(None, 'ALL')
        for num in data[0].split():
            rc, data = conn.fetch(num, '(RFC822)')
            opts['counter'] = opts['counter'] + 1
            with open("{}.eml".format(opts['counter']), "w") as f:
                f.write(data[0][1])


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-s", "--server", type=str, help="imap server", required=True)
    parser.add_argument("-P", "--port", type=int, help="port number", default=143)
    parser.add_argument("-u", "--user", type=str, help="imap user", required=True)
    parser.add_argument("-p", "--password", type=str, help="imap password",
                        required=True)
    parser.add_argument("-x", "--skip-list", type=str, help="IMAP folders to skip",
                        default="junk,trash,spam,draft,")
    parser.add_argument("-f", "--folder", type=str, help="IMAP folder to download")
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

    if args.folder:
        folders = [args.folder]
    else:
        folders = read_folder_list(conn)

    for folder in folders:
        process_folder(conn, folder)

    conn.close()


if __name__ == "__main__":
    main()
