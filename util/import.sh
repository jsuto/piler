#!/bin/bash

set -o nounset
set -o errexit
set -o pipefail

export PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin
TMPFILE=/var/run/piler/import.tmp

if [[ -f $TMPFILE ]]; then exit 1; fi

date > $TMPFILE

function finish {
   rm -f $TMPFILE
}

trap finish EXIT

cd /var/piler/imap

pilerimport -G >/dev/null
