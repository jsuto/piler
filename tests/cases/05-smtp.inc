
register_case "case1"

case1() {
   log $FUNCNAME "sending emails"

   setup

   "$SMTP_SOURCE_PROG" -s $SMTP_HOST -r archive@cust1.acts.hu -p 25 -t 20 --dir "$EML_DIR/Inbox" --socket --no-counter
   "$SMTP_SOURCE_PROG" -s $SMTP_HOST -r archive@cust1.acts.hu -p 25 -t 20 --dir "$EML_DIR/Inbox2" --socket --no-counter
   "$SMTP_SOURCE_PROG" -s $SMTP_HOST -r archive@cust1.acts.hu -p 25 -t 20 --dir "$EML_DIR/Levelszemet" --socket --no-counter
   "$SMTP_SOURCE_PROG" -s $SMTP_HOST -r archive@cust1.acts.hu -p 25 -t 20 --dir "$EML_DIR/Levelszemet2" --socket --no-counter
   "$SMTP_SOURCE_PROG" -s $SMTP_HOST -r archive@cust1.acts.hu -p 25 -t 20 --dir "$EML_DIR/spam0" --socket --no-counter
   "$SMTP_SOURCE_PROG" -s $SMTP_HOST -r archive@cust1.acts.hu -p 25 -t 20 --dir "$EML_DIR/spam1" --socket --no-counter
   "$SMTP_SOURCE_PROG" -s $SMTP_HOST -r archive@cust1.acts.hu -p 25 -t 20 --dir "$EML_DIR/spam2" --socket --no-counter
   "$SMTP_SOURCE_PROG" -s $SMTP_HOST -r archive@cust1.acts.hu -p 25 -t 20 --dir "$EML_DIR/journal" --socket --no-counter
   "$SMTP_SOURCE_PROG" -s $SMTP_HOST -r archive@cust1.acts.hu -p 25 -t 20 --dir "$EML_DIR/deduptest" --socket --no-counter
   "$SMTP_SOURCE_PROG" -s $SMTP_HOST -r archive@cust1.acts.hu extra@addr.ess another@extra.addr -p 25 -t 20 --dir "$EML_DIR/virus" --socket --no-counter


   wait_until_emails_are_processed "piler1" 3007
   docker exec "piler1" su piler -c /usr/libexec/piler/indexer.delta.sh 2>/dev/null

   count_status_values 3007 2896 111 0

   test_retrieved_messages_are_the_same "piler1" "piler"

   run_05_sphinx_tests

   docker exec "piler1" su piler -c 'php /usr/libexec/piler/generate_stats.php --webui /var/piler/www --start=2015/01/01 --stop=2021/12/31'

   docker exec "piler1" su piler -c 'php /usr/libexec/piler/sign.php --webui /var/piler/www --mode time'
}


count_status_values() {
   local container="piler1"

   log "$FUNCNAME"

   received=$(print_file_in_container "$container" "$LOG_FILE" | grep -c 'received:') || true
   stored=$(print_file_in_container "$container" "$LOG_FILE" | grep -c status=stored) || true
   discarded=$(print_file_in_container "$container" "$LOG_FILE" | grep -c status=discarded) || true
   duplicate=$(print_file_in_container "$container" "$LOG_FILE" | grep -c status=duplicate) || true

   [[ $received -eq $1 ]] || set_verdict $RESULT_CRITICAL
   [[ $stored -eq $2 ]] || set_verdict $RESULT_CRITICAL
   [[ $discarded -eq $3 ]] || set_verdict $RESULT_CRITICAL
   [[ $duplicate -eq $4 ]] || set_verdict $RESULT_CRITICAL
}


run_05_sphinx_tests() {
   local container="piler1"

   log "$FUNCNAME"

   append_queries

   for i in $(seq 0 $((${#SEARCH_QUERIES[@]}-1)) )
   do
      hits=$( run_sphinx_query "$container" "${SEARCH_QUERIES[$i]}" )
      echo $hits ${SEARCH_HITS[$i]}
   done
}


append_queries() {
   append_query "select * from main1,dailydelta1,delta1 WHERE MATCH('@subject budaörsi dc felmérés')" 9
   append_query "select * from main1,dailydelta1,delta1 WHERE MATCH('@sender gruppi.hu')" 4
   append_query "select * from main1,dailydelta1,delta1 WHERE attachments > 0 AND MATCH('@subject spam  ')" 85
   append_query "select * from main1,dailydelta1,delta1 WHERE attachments > 0 AND MATCH('@subject spam @attachment_types image')" 15
   append_query "select * from main1,dailydelta1,delta1 WHERE MATCH('@(subject,body)  New Task TSK002*')" 29
   append_query "select * from main1,dailydelta1,delta1 WHERE MATCH('@subject \"virtualfax daily summary\" | \"adsl hibajegy\"')" 11
}
