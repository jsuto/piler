NODE_WORKERS=( "worker0" "worker1" )
SMTP_GW="smtpgw"
DOCKER_LIMIT=( --pids-limit 256 --memory 512M )

export CONTAINERS=( "$NODE_GUI" "${NODE_WORKERS[@]}" "$SMTP_GW" "$SINGLE_SERVER" "piler1" "syslog.host" )

setup() {
   destroy_containers
   set -x
   launch_containers
   set +x
   create_rules "piler1"
   add_data_officer "piler1"
}

launch_containers() {

   log "starting syslog server"
   docker run -d --net=piler "${DOCKER_LIMIT[@]}" --name syslog.host sutoj/syslog

   log "starting piler"
   docker run -d --net=piler "${DOCKER_LIMIT[@]}" --name piler1 \
      -e PACKAGE="$PACKAGE" \
      -e PILER_HOST="cust1.acts.hu" \
      -p 80:80 -p 25:25 \
      -v "${PACKAGE_DIR}:/data:ro" \
      -v "${CONFIG_DIR}/11-aaaa.conf:/etc/rsyslog.d/11-aaaa.conf:ro" \
      "$docker_image"

   wait_for_sleep_cycle_in_container "piler1"
}

create_rules() {
   local container="$1"

   echo 'echo "insert into domain (domain, mapped) values(\"fictive.com\",\"fictive.com\"),(\"acts.hu\",\"acts.hu\")"| mysql --defaults-file=/etc/piler/.my.cnf piler' | docker exec -i "$container" sh

   echo 'echo "insert into archiving_rule (subject) values (\"Android táblagép\")"| mysql --defaults-file=/etc/piler/.my.cnf piler'|docker exec -i "$container" sh
   echo 'echo "insert into archiving_rule (\`from\`) values (\"@gmail.com\")"| mysql --defaults-file=/etc/piler/.my.cnf piler'|docker exec -i "$container" sh
   echo 'echo "insert into archiving_rule (\`from\`,attachment_type, _attachment_size, attachment_size) values (\"finderis.co.ua\", \"image\", \">\", 100000)"|mysql --defaults-file=/etc/piler/.my.cnf piler'|docker exec -i "$container" sh
   echo 'echo "insert into archiving_rule (\`to\`) values (\"undisclosed-recipients\")"|mysql --defaults-file=/etc/piler/.my.cnf piler'|docker exec -i "$container" sh

   echo 'echo "update user set password=\"\$6\$GKL00T\$8jqoFOe3PyAbOCLwKB7JwndwC.IinHrZRkdoQDZUc8vybZ88sA2qomlz5JceNif8fFpkGzZ03ilvQa7tqQx0v1\""| mysql --defaults-file=/etc/piler/.my.cnf piler'|docker exec -i "$container" sh

   docker exec "$container" /etc/init.d/rc.piler reload
}

add_data_officer() {
   local container="$1"
   echo 'echo "insert into user (uid, username, realname, password, domain, dn, isadmin) values(2, \"dataofficer\", \"Data officer\", \"\$6\$rX285LfP\$ZxhlacbzKuCcqkaizzBu8SAiYb6.f8K4Us08nUHwSpWMQkNhw4o2rmfKXoTfaM4rnBHUYVK1N4IfBsqN8CAtS/\", \"local\", \"*\", 4)"| mysql --defaults-file=/etc/piler/.my.cnf piler' | docker exec -i "$container" sh
   echo 'echo "insert into email (uid, email) values(2, \"do@local\")"| mysql --defaults-file=/etc/piler/.my.cnf piler' | docker exec -i "$container" sh
}
