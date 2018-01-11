#!/bin/bash

set -o errexit
set -o pipefail
set -o nounset

export LD_LIBRARY_PATH=../src

./check_parser_utils
./check_parser
./check_rules
./check_digest
./check_mydomains
./check_misc
./check_hash
./check_decoder
