#!/bin/bash

export PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin
PRIORITY=mail.error
TMPFILE=/var/run/piler/import.tmp

if [ -f $TMPFILE ]; then exit 1; fi

date > $TMPFILE

function finish {
   rm -f $TMPFILE
}

trap finish EXIT

pilerimport -G >/dev/null


