#!/bin/bash

MAINTMPFILE=/var/run/piler/main.indexer.tmp
#MAINTMPFILE=/tmp/main.indexer.tmp
INDEXER=`which indexer`
PRIORITY=mail.error

if [ -f $MAINTMPFILE ]; then echo "INDEXER ERROR: indexer merging to main index is already running. It started at "`cat $MAINTMPFILE` | logger -p $PRIORITY ; exit 1; fi

date > $MAINTMPFILE

function finish {
   rm -f $MAINTMPFILE
}

trap finish EXIT

echo "INDEXER INFO: merging to main started" | logger -p $PRIORITY

$INDEXER --quiet --merge main1 dailydelta1 --merge-dst-range deleted 0 0 --rotate

echo "INDEXER INFO: merging to main finished" | logger -p $PRIORITY

sleep 5

echo "INDEXER INFO: resetting daily delta started" | logger -p $PRIORITY

$INDEXER --quiet dailydelta1 --rotate

echo "INDEXER INFO: resetting daily delta finished" | logger -p $PRIORITY

