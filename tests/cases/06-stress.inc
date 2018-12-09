
register_case "stress"

stress() {
   log $FUNCNAME "sending 10k emails"

   docker exec -i "piler1" cp /dev/null /var/log/syslog 2>/dev/null

   "${SMTP_SOURCE_DIR}/smtp-source.py" -s $SMTP_HOST -r archive@cust1.acts.hu -p 25 -t 20 --count 10000 --no-counter

   wait_until_emails_are_processed "piler1" 10000

   log "running delta indexer"
   docker exec -i "piler1" su piler -c /usr/libexec/piler/indexer.delta.sh 2>/dev/null

   log "running main indexer"
   docker exec -i "piler1" su piler -c /usr/libexec/piler/indexer.main.sh 2>/dev/null

   log "running main index watcher"
   docker exec -i "piler1" su piler -c /usr/libexec/piler/watch_sphinx_main_index.sh 2>/dev/null
}
