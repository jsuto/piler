#!/bin/bash

export PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/libexec/piler:/usr/local/libexec/piler
PRIORITY=mail.error
TMPFILE=/var/run/piler/purge.tmp
PURGE_BEACON=/var/piler/stat/purge

if [ -f $TMPFILE ]; then exit 1; fi

date > $TMPFILE

function finish {
   rm -f $TMPFILE
}

trap finish EXIT

touch $PURGE_BEACON

pilerpurge.py
