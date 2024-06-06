#!/bin/bash

set -o nounset
set -o errexit
set -o pipefail

EML_DIR="/opt/tests/eml"
SMTP_HOST="127.0.0.1"
SMTP_SOURCE_PROG="/usr/local/bin/smtp-source.py"

SCRIPT_PATH="$(readlink -f "$0")"
SCRIPT_DIR="${SCRIPT_PATH%/*}"

declare -a SEARCH_QUERIES
declare -a SEARCH_HITS
declare -A SHA256_SUM

VERDICT=0
RESULT_CRITICAL=2

error() {
   echo "$@"
   exit 1
}

set_verdict() {
   [[ ${VERDICT} -ge "$1" ]] || { VERDICT="$1"; echo "verdict: ${VERDICT}"; }
}

get_verdict() {
   echo "verdict: ${VERDICT}"
   exit "$VERDICT"
}

count_status_values() {
   local logfile="mail.log"

   echo "${FUNCNAME[0]}"

   # shellcheck disable=SC2153
   docker exec "$CONTAINER" cat "/var/log/${logfile}" > "$logfile"

   received=$(grep -c "received:" "$logfile")
   stored=$(grep -c "status=stored" "$logfile")
   discarded=$(grep -c "status=discarded" "$logfile") || true
   duplicate=$(grep -c "status=duplicate" "$logfile") || true

   [[ $received -eq $1 ]] || set_verdict $RESULT_CRITICAL
   [[ $stored -eq $2 ]] || set_verdict $RESULT_CRITICAL
   [[ $discarded -eq $3 ]] || set_verdict $RESULT_CRITICAL
   [[ $duplicate -eq $4 ]] || set_verdict $RESULT_CRITICAL
}

append_query() {
   SEARCH_QUERIES+=( "$1" )
   SEARCH_HITS+=( "$2" )
}

append_queries() {
   local index="main1,dailydelta1,delta1"

   if [[ $RT -eq 1 ]]; then index="piler1"; fi

   append_query "select * from ${index} WHERE MATCH('@subject budaörsi dc felmérés')" 9
   append_query "select * from ${index} WHERE MATCH('@sender gruppi.hu')" 4
   append_query "select * from ${index} WHERE attachments > 0 AND MATCH('@subject spam  ')" 85
   append_query "select * from ${index} WHERE attachments > 0 AND MATCH('@subject spam @attachment_types image')" 15
   append_query "select * from ${index} WHERE MATCH('@(subject,body)  New Task TSK002*')" 29
   append_query "select * from ${index} WHERE MATCH('@subject \"virtualfax daily summary\" | \"adsl hibajegy\"')" 11
}

run_sphinx_tests() {
   echo "${FUNCNAME[0]}"

   append_queries

   for i in $(seq 0 $((${#SEARCH_QUERIES[@]}-1)) )
   do
      hits=$( echo "${SEARCH_QUERIES[$i]}; show meta" | docker exec -i "$CONTAINER" mysql -h 127.0.0.1 -P9306 | grep total_found|awk '{ print $2 }' )
      echo "${hits} ${SEARCH_HITS[$i]}"
   done
}

run_import_job() {
   echo "${FUNCNAME[0]}"
   docker exec "$CONTAINER" su piler -c /usr/libexec/piler/import.sh
}

wait_until_emails_are_processed() {
   local container="$1"
   local num=$2
   local loops
   local i=0
   local processed=0

   echo "${FUNCNAME[0]}"

   loops=$(( num / 100 ))

   while true; do
      processed="$(docker exec "$container" grep -c status= /var/log/mail.log)"
      i=$(( i + 1 ))
      echo "processed ${processed} messages"

      [[ $processed -lt "$num" ]] || break

      sleep 10

      [[ $i -gt $loops ]] && error "${container} did not process ${num} emails"
   done

   echo "${processed} emails are processed in ${container}"
}

read_eml_files_data() {
   local fname
   local message_id
   local sum
   local ts_start
   local ts_stop
   local sumsfile="/opt/tests/shasums.txt"

   ts_start="$(date +%s)"

   if [[ -f "$sumsfile" ]]; then
      echo "reading ${sumsfile}"
      # shellcheck disable=SC1090
      source "$sumsfile"
   else
      echo "reading eml files from ${EML_DIR}"

      while read -r -d $'\0' fname; do
         if [[ $(grep -c -i Message-ID: "$fname") -gt 0 ]]; then
            message_id="$(grep -i ^Message-ID: "$fname" | head -1 | awk '{ print $2 }')"
            if [[ "$message_id" ]]; then
               sum="$(sha256sum "$fname" | cut -f1 -d ' ')"
               SHA256_SUM["$message_id"]="$sum"
            fi
         fi
      done < <(find "$EML_DIR" -type f -print0)
   fi

   ts_stop="$(date +%s)"
   echo "${FUNCNAME[0]}" "took" $(( ts_stop - ts_start )) "secs"
}

test_retrieved_messages_are_the_same() {
   local container="$1"
   local database="$2"
   local message_id
   local i=0
   local bad=0
   declare -A PILER_MESSAGE_ID
   declare -a RETRIEVED_SHA256_SUM

   shift
   shift

   echo "${FUNCNAME[0]}" "$container" "$database"

   while read -r message_id retrieved_cksum; do
      #echo "digest=$retrieved_cksum" "mid=$message_id"

      PILER_MESSAGE_ID["$retrieved_cksum"]="$message_id"
      RETRIEVED_SHA256_SUM+=( "$retrieved_cksum" )
   done < <(docker exec -i "$CONTAINER" mysql -u piler -ppiler123 -h mysql --skip-column-names "$database" <<< "select message_id, digest from metadata where digest != ''" 2>/dev/null)

   for retrieved_cksum in "${RETRIEVED_SHA256_SUM[@]}"; do
      message_id="${PILER_MESSAGE_ID["$retrieved_cksum"]}"
      i=$(( i + 1 ))
      # some of the messages are NOT present in the metadata table
      # either because of a rule discarded it, or it has a nested
      # mime message inside which overrides the first Message-ID
      # found by a naive grep in read_eml_files_data()
      if [[ ${SHA256_SUM["$message_id"]+abc} ]]; then
         [[ "$retrieved_cksum" == "${SHA256_SUM["$message_id"]}" ]] || { bad=$(( bad + 1 )); echo "$message_id" is BAD; }
      fi
   done

   if [[ $bad -gt 0 ]]; then
      set_verdict "$RESULT_CRITICAL"
      exit 1
   fi

   echo "tested ${i} messages"
}

for i in Inbox Inbox2 Levelszemet Levelszemet2 spam0 spam1 spam2 journal deduptest special; do
   "$SMTP_SOURCE_PROG" -s "$SMTP_HOST" -r "archive@${ARCHIVE_HOST}" -p 25 -t 20 --dir "$EML_DIR/$i" --no-counter
done

"$SMTP_SOURCE_PROG" -s "$SMTP_HOST" -r "archive@${ARCHIVE_HOST}" extra@addr.ess another@extra.addr -p 25 -t 20 --dir "$EML_DIR/virus" --no-counter

wait_until_emails_are_processed "$CONTAINER" 3020

[[ $RT -eq 1 ]] || docker exec "$CONTAINER" su piler -c /usr/libexec/piler/indexer.delta.sh 2>/dev/null

count_status_values 3020 2909 111 0

read_eml_files_data
test_retrieved_messages_are_the_same "$CONTAINER" "piler"

run_sphinx_tests

docker exec "$CONTAINER" su piler -c 'php /usr/libexec/piler/generate_stats.php --webui /var/piler/www --start=2015/01/01 --stop=2021/12/31'

docker exec "$CONTAINER" su piler -c 'php /usr/libexec/piler/sign.php --webui /var/piler/www --mode time'

run_import_job

docker exec "$CONTAINER" tail -30 /var/log/nginx/error.log

docker exec -i "$CONTAINER" bash -c 'cat >>/root/.bashrc' <<< "alias n='tail -f /var/log/nginx/error.log'"
docker exec -i "$CONTAINER" bash -c 'cat >>/root/.bashrc' <<< "alias t='tail -f /var/log/mail.log'"

pushd "$SCRIPT_DIR"
docker cp addons.sh "${CONTAINER}:/tmp"
popd

get_verdict
