NODE_WORKERS=( "worker0" "worker1" )
SMTP_GW="smtpgw"
DOCKER_LIMIT=( --pids-limit 256 --memory 512M )

export CONTAINERS=( "$NODE_GUI" "${NODE_WORKERS[@]}" "$SMTP_GW" "$SINGLE_SERVER" )

setup() {
   destroy_containers
   launch_containers
   setup_piler "piler1"
}

cleanup_package() {
   local pkg="$1"

   log "Removing ${PACKAGE_DIR}/${pkg}"
   rm -f "${PACKAGE_DIR}/${pkg}"
}

launch_containers() {
   pushd "${WORKSPACE}/${PROJECT_ID}/tests"

   CONTAINERS+=( "piler1" "syslog.host" )

   echo -e "PACKAGE=$PACKAGE\nPILER_HOST=cust1.acts.hu\nRT=$RT" > .env

   log "running docker-compose down"

   docker-compose down -v || true

   log "running docker-compose up"

   docker-compose up -d

   wait_until_container_is_healthy "piler1"

   docker-compose ps

   popd
}

setup_piler() {
   local container="$1"

   pushd "${WORKSPACE}/${PROJECT_ID}/tests"

   docker exec "$container" /usr/local/bin/setup-piler.sh

   popd
}
