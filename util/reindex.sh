#!/bin/bash

set -o nounset
set -o errexit
set -o pipefail

INSTALL_PREFIX=/usr/local
THRESHOLD=100000000

start_id=1
stop_id=5000000

while [[ $start_id -lt $stop_id ]]; do
   y=$(( start_id + 9999 ))
   echo $start_id $y
   "${INSTALL_PREFIX}/bin/reindex" -f $start_id -t $y -p
   "${INSTALL_PREFIX}/libexec/piler/indexer.delta.sh"
   start_id=$(( start_id + 10000 ))

   if [[ "$(stat -c %s /var/piler/sphinx/dailydelta1.spp)" -gt "$THRESHOLD" ]]; then
      "${INSTALL_PREFIX}/libexec/piler/indexer.main.sh"
   fi
done
