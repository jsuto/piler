#!/bin/bash

set -o nounset
set -o errexit
set -o pipefail

export PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/libexec/piler:/usr/local/libexec/piler
TMPFILE=/run/piler/purge.tmp
PURGE_BEACON=/var/piler/stat/purge

if [[ -f $TMPFILE ]]; then exit 1; fi

date > $TMPFILE

function finish {
   rm -f $TMPFILE
}

trap finish EXIT

touch $PURGE_BEACON

pilerpurge.py
