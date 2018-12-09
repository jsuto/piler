CONTAINERS=( "piler1" )

setup() {
   destroy_containers
   launch_containers
   create_rules "piler1"
}

launch_containers() {
   docker run -d --net=piler --name piler1 -e PILER_HOST="cust1.acts.hu" -p 80:80 -p 25:25 "$docker_image" 2>/dev/null
   wait_for_sleep_cycle_in_container "piler1"

   if [[ -v JENKINS_URL ]]; then
      docker cp "${HOME}/telegraf.conf" piler1:/etc/telegraf/telegraf.conf 2>/dev/null
   else
      docker cp "${topdir}/secrets/telegraf.conf" piler1:/etc/telegraf/telegraf.conf 2>/dev/null
   fi

   docker exec "piler1" sed -i 's/HOSTNAME/piler1/g' /etc/telegraf/telegraf.conf
   docker exec "piler1" service telegraf start
}

create_rules() {
   local container="$1"

   echo 'echo "insert into domain (domain, mapped) values(\"fictive.com\",\"fictive.com\")"| mysql --defaults-file=/etc/piler/.my.cnf piler' | docker exec -i "$container" sh 2>/dev/null

   echo 'echo "insert into archiving_rule (subject) values (\"Android táblagép\")"| mysql --defaults-file=/etc/piler/.my.cnf piler'|docker exec -i "$container" sh 2>/dev/null
   echo 'echo "insert into archiving_rule (\`from\`) values (\"@gmail.com\")"| mysql --defaults-file=/etc/piler/.my.cnf piler'|docker exec -i "$container" sh 2>/dev/null
   echo 'echo "insert into archiving_rule (\`from\`,attachment_type, _attachment_size, attachment_size) values (\"finderis.co.ua\", \"image\", \">\", 100000)"|mysql --defaults-file=/etc/piler/.my.cnf piler'|docker exec -i "$container" sh 2>/dev/null
   echo 'echo "insert into archiving_rule (\`to\`) values (\"undisclosed-recipients\")"|mysql --defaults-file=/etc/piler/.my.cnf piler'|docker exec -i "$container" sh 2>/dev/null

   echo 'echo "update user set password=\"\$6\$GKL00T\$8jqoFOe3PyAbOCLwKB7JwndwC.IinHrZRkdoQDZUc8vybZ88sA2qomlz5JceNif8fFpkGzZ03ilvQa7tqQx0v1\""| mysql --defaults-file=/etc/piler/.my.cnf piler'|docker exec -i "$container" sh 2>/dev/null

   docker exec "$container" /etc/init.d/rc.piler reload 2>/dev/null
}
