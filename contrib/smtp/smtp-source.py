#!/usr/bin/python

import argparse
import smtplib
import random
import string
import quopri
import time
import sys
import email.utils

from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart

"""
check out http://blog.magiksys.net/generate-and-send-mail-with-python-tutorial
for a real professional solution
"""

eol = "\r\n"
dictionary = '/'.join(__file__.split('/')[:-1]) + "/dictionary.txt"
checkpoint_for_newline = 300


def create_random_string(len=60):
    return ''.join(random.choice(string.uppercase + string.digits) for i in range(len))


def create_msg(words, size=10000):
    msg_text = ""
    msg_size = 0
    new_line_checkpoint = checkpoint_for_newline

    while msg_size < size:
        choice = random.SystemRandom().choice
        s = str(choice(words)).replace('\n', ' ')
        msg_text += s
        msg_size += len(s)
        if msg_size > new_line_checkpoint:
            new_line_checkpoint += checkpoint_for_newline
            msg_text += eol + eol

    return msg_text


def create_text_mime_part(text, charset, encoding, boundary):
    part = ""

    part += "--" + boundary + eol
    part += "Content-Type: text/plain; charset=\"" + charset + "\"" + eol
    part += "Content-Transfer-Encoding: " + encoding + eol
    part += eol
    part += text + eol + eol

    return part
 

def create_headers(args, boundary):
    hdr = ""

    hdr += "Content-Type: multipart/alternative;" + eol
    hdr += "   boundary=\"" + boundary + "\"" + eol
    hdr += "MIME-Version: 1.0" + eol
    hdr += "Subject: " + args.subject + eol
    hdr += "From: " + args.sender + eol
    hdr += "To: " + ', ' . join(args.rcpt) + eol
    hdr += "Date: " + email.utils.formatdate(time.time(), localtime=True) + eol
    hdr += "Message-ID: " + "<" + create_random_string(40) + "@" + args.helo + ">" + eol
    hdr += eol + eol

    return hdr


def create_message(words, args):
    msg = ""

    boundary = '_' + create_random_string() + '_'

    msg += create_headers(args, boundary)

    the_text = create_msg(words, args.msglen)
    the_text = quopri.encodestring(the_text)

    msg += create_text_mime_part(the_text, "iso-8859-2", "quoted-printable", boundary)
    msg += "--" + boundary + eol

    return msg


parser = argparse.ArgumentParser()

parser.add_argument("--helo", type=str, help="ehlo hostname", default="myhost.aaa.fu")
parser.add_argument("-f", "--sender", type=str, help="sender email", default="sender@aaa.fu")
parser.add_argument("-r", "--rcpt", type=str, nargs='+', help="recipient email(s)", default=["archive@aaa.fu"])
parser.add_argument("-c", "--count", type=int, help="number of emails to send", default=1)
parser.add_argument("-d", "--debug", type=int, help="debug level", default=0)
parser.add_argument("--session", type=int, help="number of emails to send in one smtp transaction", default=1)
parser.add_argument("-s", "--server", type=str, help="smtp server", required=True)
parser.add_argument("-p", "--port", type=int, help="smtp port", default=25)
parser.add_argument("--subject", type=str, help="subject", default="This is test subject")
parser.add_argument("-l", "--msglen", type=int, help="message length (approx.)", default=20000)
parser.add_argument("--starttls", help="use STARTTLS", action="store_true")
parser.add_argument("--pem", type=str, help="pem file for starttls", default="")

args = parser.parse_args()


if args.starttls and args.pem == "":
    sys.exit("make a pem file for starttls")


with open(dictionary) as f:
    words = f.readlines()

i = 0
total_count = 0

while i < args.count:
    server = smtplib.SMTP(args.server, args.port, args.helo, 10)

    if args.starttls:
        server.starttls(args.pem, args.pem)

    k = 0

    while i < args.count and k < args.session:
        message = create_message(words, args)
        server.set_debuglevel(args.debug)
        server.sendmail(args.sender, args.rcpt, message)

        k += 1
        total_count += 1
 
        if args.debug == 0:
            sys.stdout.write('%s\r' % str(total_count))
            sys.stdout.flush()

    server.quit()
    i += 1


if args.debug == 0:
    print
