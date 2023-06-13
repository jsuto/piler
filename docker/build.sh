#!/bin/bash

set -o errexit
set -o pipefail
set -o nounset

IMAGE_NAME="sutoj/piler:1.4.4"

if [[ $# -ne 1 ]]; then echo "ERROR: missing package name" 1>&2; exit 1; fi

docker build --pull --build-arg PACKAGE="$1" -t "$IMAGE_NAME" .
