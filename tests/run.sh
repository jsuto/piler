#!/bin/bash

set -o nounset
set -o errexit
set -o pipefail

SCRIPT_PATH="$(readlink -f "$0")"
SCRIPT_DIR="${SCRIPT_PATH%/*}"

EML_DIR=/home/sj/temp/eml
SMTP_SOURCE_PROG=/home/sj/devel/ci/test/smtp/smtp-source.py
SMTP_HOST="127.0.0.1"

for i in Inbox Inbox2 Levelszemet Levelszemet2 spam0 spam1 spam2 journal deduptest special virus; do
   "$SMTP_SOURCE_PROG" -s "$SMTP_HOST" -r archive@cust1.acts.hu -p 25 -t 20 --dir "$EML_DIR/$i" --no-counter
done
