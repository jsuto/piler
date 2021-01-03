#!/bin/bash

set -o errexit
set -o pipefail
set -o nounset

CONFIG_DIR="/etc/piler"
VOLUME_DIR="/var/piler"
PILER_CONF="${CONFIG_DIR}/piler.conf"
PILER_KEY="${CONFIG_DIR}/piler.key"
PILER_PEM="${CONFIG_DIR}/piler.pem"
PILER_NGINX_CONF="${CONFIG_DIR}/piler-nginx.conf"
SPHINX_CONF="${CONFIG_DIR}/sphinx.conf"
CONFIG_SITE_PHP="${CONFIG_DIR}/config-site.php"
PILER_MY_CNF="${CONFIG_DIR}/.my.cnf"
ROOT_MY_CNF="/root/.my.cnf"


error() {
   echo "ERROR:" "$*" 1>&2
   exit 1
}


log() {
   echo "DEBUG:" "$*"
}


pre_flight_check() {
   [[ -v PILER_HOSTNAME ]] || error "Missing PILER_HOSTNAME env variable"
   [[ -v MYSQL_HOSTNAME ]] || error "Missing MYSQL_HOSTNAME env variable"
   [[ -v MYSQL_PILER_PASSWORD ]] || error "Missing MYSQL_PILER_PASSWORD env variable"
   [[ -v MYSQL_ROOT_PASSWORD ]] || error "Missing MYSQL_ROOT_PASSWORD env variable"
}


give_it_to_piler() {
   local f="$1"

   [[ -f "$f" ]] || error "${f} does not exist, aborting"

   chown "${PILER_USER}:${PILER_USER}" "$f"
   chmod 600 "$f"
}


make_certificate() {
   local f="$1"
   local crt="/tmp/1.cert"
   local SSL_CERT_DATA="/C=US/ST=Denial/L=Springfield/O=Dis/CN=www.example.com"

   log "Making an ssl certificate"

   openssl req -new -newkey rsa:4096 -days 3650 -nodes -x509 -subj "$SSL_CERT_DATA" -keyout "$f" -out "$crt" -sha1 2>/dev/null
   cat "$crt" >> "$f"
   rm -f "$crt"

   give_it_to_piler "$f"
}


make_piler_key() {
   local f="$1"

   log "Generating piler.key"

   dd if=/dev/urandom bs=56 count=1 of="$f" 2>/dev/null
   [[ $(stat -c '%s' "$f") -eq 56 ]] || error "could not read 56 bytes from /dev/urandom to ${f}"

   give_it_to_piler "$f"
}


fix_configs() {
   [[ -f "$PILER_KEY" ]] || make_piler_key "$PILER_KEY"
   [[ -f "$PILER_PEM" ]] || make_certificate "$PILER_PEM"

   if [[ ! -f "$PILER_NGINX_CONF" ]]; then
      log "Writing ${PILER_NGINX_CONF}"

      cp "${PILER_NGINX_CONF}.dist" "$PILER_NGINX_CONF"
      sed -i "s%PILER_HOST%${PILER_HOSTNAME}%" "$PILER_NGINX_CONF"
   fi

   if [[ ! -f "$PILER_CONF" ]]; then
      log "Writing ${PILER_CONF}"

      sed \
         -e "s/verystrongpassword/$MYSQL_PILER_PASSWORD/g" \
         -e "s/hostid=.*/hostid=${PILER_HOSTNAME}/g" \
         -e "s/tls_enable=.*/tls_enable=1/g" \
         -e "s/mysqlsocket=.*/mysqlsocket=/g" "${PILER_CONF}.dist" > "$PILER_CONF"

      {
         echo "mysqlhost=${MYSQL_HOSTNAME}"
      } >> "$PILER_CONF"

      give_it_to_piler "$PILER_CONF"
   fi

   if [[ ! -f "$CONFIG_SITE_PHP" ]]; then
      log "Writing ${CONFIG_SITE_PHP}"

      cp "${CONFIG_DIR}/config-site.dist.php" "$CONFIG_SITE_PHP"

      sed -i "s%HOSTNAME%${PILER_HOSTNAME}%" "$CONFIG_SITE_PHP"

      {
         echo "\$config['DECRYPT_BINARY'] = '/usr/bin/pilerget';"
         echo "\$config['DECRYPT_ATTACHMENT_BINARY'] = '/usr/bin/pileraget';"
         echo "\$config['PILER_BINARY'] = '/usr/sbin/piler';"
         echo "\$config['DB_HOSTNAME'] = '$MYSQL_HOSTNAME';"
         echo "\$config['DB_PASSWORD'] = '$MYSQL_PILER_PASSWORD';"
         echo "\$config['ENABLE_MEMCACHED'] = 1;"
         echo "\$memcached_server = ['memcached', 11211];"
      } >> "$CONFIG_SITE_PHP"
   fi

   sed -e "s%MYSQL_HOSTNAME%${MYSQL_HOSTNAME}%" \
       -e "s%MYSQL_DATABASE%${MYSQL_DATABASE}%" \
       -e "s%MYSQL_USERNAME%${PILER_USER}%" \
       -e "s%MYSQL_PASSWORD%${MYSQL_PILER_PASSWORD}%" \
       -i "$SPHINX_CONF"
}


wait_until_mysql_server_is_ready() {
   while true; do if mysql "--defaults-file=${ROOT_MY_CNF}" <<< "show databases"; then break; fi; log "${MYSQL_HOSTNAME} is not ready"; sleep 5; done

   log "${MYSQL_HOSTNAME} is ready"
}


init_database() {
   local db
   local has_piler_db=0

   wait_until_mysql_server_is_ready

   while read -r db; do
      if [[ "$db" == "$MYSQL_DATABASE" ]]; then has_piler_db=1; fi
   done < <(mysql "--defaults-file=${ROOT_MY_CNF}" <<< 'show databases')

   if [[ $has_piler_db -eq 0 ]]; then
      log "no ${MYSQL_DATABASE} database, creating"

      mysql "--defaults-file=${ROOT_MY_CNF}" <<< "create database ${MYSQL_DATABASE} character set utf8mb4"
      mysql "--defaults-file=${ROOT_MY_CNF}" <<< "grant all privileges on ${MYSQL_DATABASE}.* to ${PILER_USER} identified by '${MYSQL_PILER_PASSWORD}'"
      mysql "--defaults-file=${ROOT_MY_CNF}" <<< "flush privileges"

      mysql "--defaults-file=${PILER_MY_CNF}" "$MYSQL_DATABASE" < /usr/share/piler/db-mysql.sql
   else
      log "${MYSQL_DATABASE} db exists"
   fi

   if [[ -v ADMIN_USER_PASSWORD_HASH ]]; then
      mysql "--defaults-file=${PILER_MY_CNF}" "$MYSQL_DATABASE" <<< "update user set password='${ADMIN_USER_PASSWORD_HASH}' where uid=0"
   fi
}


create_my_cnf_files() {
   printf "[client]\nhost = %s\nuser = %s\npassword = %s\n[mysqldump]\nhost = %s\nuser = %s\npassword = %s\n" \
      "$MYSQL_HOSTNAME" "$PILER_USER" "$MYSQL_PILER_PASSWORD" "$MYSQL_HOSTNAME" "$PILER_USER" "$MYSQL_PILER_PASSWORD" \
      > "$PILER_MY_CNF"
   printf "[client]\nhost = %s\nuser = root\npassword = %s\n" "$MYSQL_HOSTNAME" "$MYSQL_ROOT_PASSWORD" > "$ROOT_MY_CNF"

   give_it_to_piler "$PILER_MY_CNF"
}


start_services() {
   service rsyslog start
   service cron start
   service php7.4-fpm start
   service nginx start
}


start_piler() {
   if [[ ! -f "${VOLUME_DIR}/sphinx/main1.spp" ]]; then
      log "main1.spp does not exist, creating index files"
      su -c "indexer --all --config ${SPHINX_CONF}" piler
   fi

   # No pid file should exist for piler
   rm -f /var/run/piler/*pid

   /etc/init.d/rc.searchd start
   /etc/init.d/rc.piler start
}


pre_flight_check
fix_configs
create_my_cnf_files
init_database
start_services
start_piler

while true; do sleep 3600; done
