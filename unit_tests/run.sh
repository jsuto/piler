#!/bin/bash

set -o errexit
set -o pipefail
set -o nounset
set -x

SCRIPT_PATH="$(readlink -f "$0")"
SCRIPT_DIR="${SCRIPT_PATH%/*}"

echo "Running unit tests"

export LD_LIBRARY_PATH=../src

pushd "$SCRIPT_DIR"

setup_mysql() {
   if [[ "$DISTRO" == "jammy" ]]; then
      service mariadb start
   else
      service mysql start
   fi

   mysql -u piler -ppiler123 piler1 < ../util/db-mysql.sql
}

if [[ -v BUILD_NUMBER ]]; then
   setup_mysql
fi

./check_parser_utils
./check_parser
./check_rules
./check_digest
./check_mydomains
./check_misc
./check_hash
./check_decoder
./check_attachments
