#!/bin/bash

set -o errexit
set -o pipefail
set -o nounset

[[ $(id -g piler 2>/dev/null) ]] || groupadd piler
[[ $(id -u piler 2>/dev/null) ]] || useradd -g piler -d /var/piler -s /bin/bash piler
