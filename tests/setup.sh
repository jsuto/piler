#!/bin/bash

set -o nounset
set -o errexit
set -o pipefail

SCRIPT_PATH="$(readlink -f "$0")"
SCRIPT_DIR="${SCRIPT_PATH%/*}"

pushd "$SCRIPT_DIR"
docker cp setup.sql "${CONTAINER}:/tmp"
docker exec "$CONTAINER" bash -c 'mysql --defaults-file=/etc/piler/.my.cnf < /tmp/setup.sql'
docker exec "$CONTAINER" /etc/init.d/rc.piler reload
popd
