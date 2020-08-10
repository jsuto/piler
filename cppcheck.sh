#!/bin/bash

set -o nounset
set -o errexit
set -o pipefail

cppcheck -DHAVE_PDFTOTEXT -DHAVE_PPTHTML -DHAVE_TNEF -DHAVE_UNRTF -DHAVE_XLS2CSV -DHAVE_CATPPT -DHAVE_CATDOC -DHAVE_ZIP -D_GNU_SOURCE -DHAVE_DAEMON -DHAVE_TRE -DHAVE_CLAMD -DHAVE_LIBCLAMAV -DNEED_MYSQL --error-exitcode=1 --enable=all --suppressions-list=suppressions.txt --force src/ unit_tests/
