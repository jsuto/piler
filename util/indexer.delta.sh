#!/bin/bash

export PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin
MAINTMPFILE=/var/run/piler/main.indexer.tmp
DELTATMPFILE=/var/run/piler/delta.indexer.tmp
INDEXER=indexer
PRIORITY=mail.error
TOUCHFILE=/var/piler/stat/indexer

if [ -f $MAINTMPFILE ]; then echo "INDEXER ERROR: indexer merging to main index is already running. It started at "`cat $MAINTMPFILE` | logger -p $PRIORITY ; exit 1; fi

if [ -f $DELTATMPFILE ]; then echo "INDEXER ERROR: delta indexing and merging is already running. It started at "`cat $DELTATMPFILE` | logger -p $PRIORITY ; exit 1; fi

date > $DELTATMPFILE

touch $TOUCHFILE

function finish {
   rm -f $DELTATMPFILE
}

trap finish EXIT

echo "INDEXER INFO: indexing delta1 started" | logger -p $PRIORITY

$INDEXER --quiet delta1 --rotate

echo "INDEXER INFO: indexing delta1 finished" | logger -p $PRIORITY

sleep 5

echo "INDEXER INFO: merging delta to dailydelta started" | logger -p $PRIORITY

$INDEXER --quiet --merge dailydelta1 delta1 --merge-dst-range deleted 0 0 --rotate

echo "INDEXER INFO: merging delta to dailydelta finished" | logger -p $PRIORITY

