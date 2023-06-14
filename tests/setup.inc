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

cleanup_package() {
   local pkg="$1"

   log "Removing ${PACKAGE_DIR}/${pkg}"
   rm -f "${PACKAGE_DIR}/${pkg}"
}

launch_containers() {
   pushd "${WORKSPACE}/${PROJECT_ID}/tests"

   echo -e "PACKAGE=$PACKAGE\nPILER_HOST=cust1.acts.hu\nRT=$RT" > .env

   log "running docker-compose"
   docker-compose up -d
   wait_for_sleep_cycle_in_container "piler1"

   docker-compose ps

   popd
}

launch_containers2() {

   log "starting syslog server"
   docker run -d --net=piler "${DOCKER_LIMIT[@]}" --name syslog.host sutoj/syslog

   log "starting piler"
   docker run -d --net=piler "${DOCKER_LIMIT[@]}" --name piler1 \
      -e PACKAGE="$PACKAGE" \
      -e PILER_HOST="cust1.acts.hu" \
      -e RT="$RT" \
      -p 127.0.0.1:80:80 -p 25:25 \
      -v "${PACKAGE_DIR}:/data:ro" \
      -v "${CONFIG_DIR}/11-aaaa.conf:/etc/rsyslog.d/11-aaaa.conf:ro" \
      "$docker_image"

   wait_for_sleep_cycle_in_container "piler1"

   ###cleanup_package "$PACKAGE"
}

create_rules() {
   local container="$1"

   echo 'echo "insert into domain (domain, mapped) values(\"fictive.com\",\"fictive.com\"),(\"address.com\",\"address.com\"),(\"acts.hu\",\"acts.hu\"),(\"gtsce.com\",\"gtsce.com\"),(\"datanet.hu\",\"datanet.hu\"),(\"gtsdatanet.hu\",\"gtsdatanet.hu\"),(\"gts.hu\",\"gts.hu\"),(\"aaa.fu\",\"aaa.fu\")"| mysql --defaults-file=/etc/piler/.my.cnf piler' | docker exec -i "$container" sh

   echo 'echo "insert into archiving_rule (subject) values (\"Android táblagép\")"| mysql --defaults-file=/etc/piler/.my.cnf piler'|docker exec -i "$container" sh
   echo 'echo "insert into archiving_rule (\`from\`) values (\"@gmail.com\")"| mysql --defaults-file=/etc/piler/.my.cnf piler'|docker exec -i "$container" sh
   echo 'echo "insert into archiving_rule (\`from\`,attachment_type, _attachment_size, attachment_size) values (\"finderis.co.ua\", \"image\", \">\", 100000)"|mysql --defaults-file=/etc/piler/.my.cnf piler'|docker exec -i "$container" sh
   echo 'echo "insert into archiving_rule (\`to\`) values (\"undisclosed-recipients\")"|mysql --defaults-file=/etc/piler/.my.cnf piler'|docker exec -i "$container" sh
   echo 'echo "insert into import (\`type\`, username, password, server) values (\"imap-ssl\", \"sanyi@aaa.fu\", \"abcde123\", \"imap.aaa.fu\")"|mysql --defaults-file=/etc/piler/.my.cnf piler'|docker exec -i "$container" sh

   echo 'echo "update user set password=\"\$6\$GKL00T\$8jqoFOe3PyAbOCLwKB7JwndwC.IinHrZRkdoQDZUc8vybZ88sA2qomlz5JceNif8fFpkGzZ03ilvQa7tqQx0v1\""| mysql --defaults-file=/etc/piler/.my.cnf piler'|docker exec -i "$container" sh

   docker exec "$container" /etc/init.d/rc.piler reload
}

add_data_officer() {
   local container="$1"
   echo 'echo "insert into user (uid, username, realname, password, domain, dn, isadmin) values(2, \"dataofficer\", \"Data officer\", \"\$6\$rX285LfP\$ZxhlacbzKuCcqkaizzBu8SAiYb6.f8K4Us08nUHwSpWMQkNhw4o2rmfKXoTfaM4rnBHUYVK1N4IfBsqN8CAtS/\", \"local\", \"*\", 4)"| mysql --defaults-file=/etc/piler/.my.cnf piler' | docker exec -i "$container" sh
   echo 'echo "insert into email (uid, email) values(2, \"do@local\")"| mysql --defaults-file=/etc/piler/.my.cnf piler' | docker exec -i "$container" sh
}
