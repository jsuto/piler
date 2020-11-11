#!/bin/bash

set -o errexit
set -o pipefail

export PATH=$PATH:/usr/libexec/piler:/usr/local/libexec/piler

pushd /var/piler/imap

[[ "${FLOCKER}" != "$0" ]] && exec env FLOCKER="$0" flock -en "$0" "$0" "$@"

imapfetch.py -i
