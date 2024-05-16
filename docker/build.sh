#!/bin/bash

set -o errexit
set -o pipefail
set -o nounset

IMAGE_NAME="sutoj/piler:test"

if [[ $# -ne 1 ]]; then echo "ERROR: missing package name" 1>&2; exit 1; fi

docker buildx build --build-arg PACKAGE="$1" -t "$IMAGE_NAME" .
