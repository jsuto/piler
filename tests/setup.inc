NODE_WORKERS=( "worker0" "worker1" )
SMTP_GW="smtpgw"
DOCKER_LIMIT=( --pids-limit 256 --memory 512M )

export CONTAINERS=( "$NODE_GUI" "${NODE_WORKERS[@]}" "$SMTP_GW" "$SINGLE_SERVER" "piler1" )

setup() {
   launch_containers
   setup_piler "piler1"
}

cleanup_package() {
   local pkg="$1"

   log "Removing ${PACKAGE_DIR}/${pkg}"
   rm -f "${PACKAGE_DIR}/${pkg}"
}

launch_containers() {
   local composefile="docker-compose-piler.yaml"

   log "${FUNCNAME[0]}"

   pushd "${WORKSPACE}/ci/test/docker-compose"

   echo -e "PACKAGE=$PACKAGE\nPILER_HOST=cust1.acts.hu\nRT=$RT" > .env

   destroy_containers

   log "running docker-compose up"

   docker-compose -f "$composefile" up -d

   wait_until_container_is_healthy "piler1"

   docker-compose -f "$composefile" ps

   popd
}

setup_piler() {
   local container="$1"

   log "${FUNCNAME[0]}"

   pushd "${WORKSPACE}/${PROJECT_ID}/tests"

   docker exec "$container" /usr/local/bin/setup-piler.sh

   popd
}
