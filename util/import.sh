#!/bin/bash

set -o nounset
set -o errexit
set -o pipefail

export PATH=$PATH:/usr/libexec/piler:/usr/local/libexec/piler

pushd /var/piler/imap

exec 200 > "/tmp/${0}-lock" || exit 1
flock 200 || exit 1
imapfetch.py -i
flock -u 200
