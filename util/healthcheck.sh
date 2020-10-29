#!/bin/bash

set -o errexit
set -o pipefail
set -o nounset

SPHINX_DIR=/var/piler/sphinx
ERROR_FILE=/var/piler/stat/error

if command -v mpstat; then
   mpstat
   echo
else
   echo "host: $(hostname)"
   echo "cpus: $(grep -c ^processor /proc/cpuinfo)"
fi

echo "load: $(cat /proc/loadavg)"
echo "mem: $(grep MemTotal /proc/meminfo | sed 's/MemTotal\s*:\s*//' )"
echo disks:
while read -r p; do df -h "${p##* }" | tail -1; done < <(lsblk | grep part)

echo
piler -V

# shellcheck disable=SC2009
ps uaxw|grep piler

echo -e "\nCron entries:\n"
crontab -l -u piler

errors=0
if [[ -f "$ERROR_FILE" ]]; then
   read -r errors < "$ERROR_FILE"
else
   errors="$(find /var/piler/error -type f|wc -l)"
fi

mysqluser="$(pilerconf -q mysqluser|cut -d = -f2)"
mysqlpwd="$(pilerconf -q mysqlpwd|cut -d = -f2)"
mysqldb="$(pilerconf -q mysqldb|cut -d = -f2)"

echo -e "\nError emails: $errors"
echo -e "Sphinx data: $(du -hs "$SPHINX_DIR")\n"

mysql -t -u "$mysqluser" -p"$mysqlpwd" "$mysqldb" <<< "select * from counter"
mysql -t -u "$mysqluser" -p"$mysqlpwd" information_schema <<< "select table_schema as db, sum(data_length+index_length) as size from TABLES WHERE table_schema='$mysqldb' GROUP BY table_schema"
