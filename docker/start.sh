#!/bin/bash

set -o errexit
set -o pipefail
set -o nounset

CONFIG_DIR="/etc/piler"
PILER_CONF="${CONFIG_DIR}/piler.conf"
PILER_KEY="${CONFIG_DIR}/piler.key"
PILER_PEM="${CONFIG_DIR}/piler.pem"
PILER_NGINX_CONF="${CONFIG_DIR}/piler-nginx.conf"
SPHINX_CONF="${CONFIG_DIR}/manticore.conf"
CONFIG_SITE_PHP="${CONFIG_DIR}/config-site.php"
PILER_MY_CNF="${CONFIG_DIR}/.my.cnf"
RT="${RT:-0}"
MEMCACHED_HOSTNAME="${MEMCACHED_HOSTNAME:-memcached}"
MANTICORE_HOSTNAME="${MANTICORE_HOSTNAME:-manticore}"
TMP_CONF_DIR="/tmp/piler-conf"

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
   [[ -v MYSQL_DATABASE ]] || error "Missing MYSQL_DATABASE env variable"
   [[ -v MYSQL_USER ]]     || error "Missing MYSQL_USER env variable"
   [[ -v MYSQL_PASSWORD ]] || error "Missing MYSQL_PASSWORD env variable"
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

   [[ -f /etc/piler/MANTICORE ]] || touch /etc/piler/MANTICORE
   for f in config-site.dist.php manticore.conf manticore.conf.dist piler-nginx.conf.dist piler.conf.dist; do
      if [[ ! -f "/etc/piler/${f}" ]]; then cp "${TMP_CONF_DIR}/${f}" /etc/piler; fi
   done

   if [[ ! -f "$PILER_NGINX_CONF" ]]; then
      log "Writing ${PILER_NGINX_CONF}"

      cp "${PILER_NGINX_CONF}.dist" "$PILER_NGINX_CONF"
      sed -i "s%PILER_HOST%${PILER_HOSTNAME}%" "$PILER_NGINX_CONF"
   fi

   if [[ ! -f "$PILER_CONF" ]]; then
      log "Writing ${PILER_CONF}"

      sed \
         -e "s/mysqluser=.*/mysqluser=${MYSQL_USER}/g" \
         -e "s/mysqldb=.*/mysqldb=${MYSQL_DATABASE}/g" \
         -e "s/verystrongpassword/${MYSQL_PASSWORD}/g" \
         -e "s/hostid=.*/hostid=${PILER_HOSTNAME}/g" \
         -e "s/tls_enable=.*/tls_enable=1/g" \
         -e "s/sphxhost=.*/sphxhost=${MANTICORE_HOSTNAME}/g" \
         -e "s/rtindex=.*/rtindex=${RT}/g" \
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
         echo "\$config['DB_DATABASE'] = '$MYSQL_DATABASE';"
         echo "\$config['DB_USERNAME'] = '$MYSQL_USER';"
         echo "\$config['DB_PASSWORD'] = '$MYSQL_PASSWORD';"
         echo "\$config['MEMCACHED_ENABLED'] = 1;"
         echo "\$memcached_server = ['$MEMCACHED_HOSTNAME', 11211];"
      } >> "$CONFIG_SITE_PHP"
   fi

   sed -e "s%MYSQL_HOSTNAME%${MYSQL_HOSTNAME}%" \
       -e "s%MYSQL_DATABASE%${MYSQL_DATABASE}%" \
       -e "s%MYSQL_USERNAME%${MYSQL_USER}%" \
       -e "s%MYSQL_PASSWORD%${MYSQL_PASSWORD}%" \
       -i "$SPHINX_CONF"

   # Fixes for RT index

   if [[ $RT -eq 1 ]]; then
      sed -i "s/define('RT', 0)/define('RT', 1)/" "$SPHINX_CONF"
      if ! grep "'RT'" "$CONFIG_SITE_PHP"; then
         echo "\$config['RT'] = 1;" >> "$CONFIG_SITE_PHP"
      fi

      if ! grep "'SPHINX_MAIN_INDEX'" "$CONFIG_SITE_PHP"; then
         echo "\$config['SPHINX_MAIN_INDEX'] = 'piler1';" >> "$CONFIG_SITE_PHP"
      fi
   fi

   if ! grep "'SPHINX_HOSTNAME'" "$CONFIG_SITE_PHP"; then
      echo "\$config['SPHINX_HOSTNAME'] = '${MANTICORE_HOSTNAME}:9306';" >> "$CONFIG_SITE_PHP"
   fi

   if ! grep "'SPHINX_HOSTNAME_READONLY'" "$CONFIG_SITE_PHP"; then
      echo "\$config['SPHINX_HOSTNAME_READONLY'] = '${MANTICORE_HOSTNAME}:9307';" >> "$CONFIG_SITE_PHP"
   fi

   # Fix for PATH_PREFIX
   if [[ -v PATH_PREFIX ]];
     then
       log "PATH_PREFIX set $PATH_PREFIX"
       sed -i -e "s#location.origin\ +\ .*#location.origin\ +\ $PATH_PREFIX,#" /var/piler/www/assets/js/piler.js
       sed -i -e "s#^\$config\['PATH_PREFIX'\].*#\$config\['PATH_PREFIX'\] = '$PATH_PREFIX';#" "$CONFIG_SITE_PHP"
   fi
}


wait_until_mysql_server_is_ready() {
   while true; do if mysql "--defaults-file=${PILER_MY_CNF}" <<< "show databases"; then break; fi; log "${MYSQL_HOSTNAME} is not ready"; sleep 5; done

   log "${MYSQL_HOSTNAME} is ready"
}


init_database() {
   local table
   local has_metadata_table=0

   wait_until_mysql_server_is_ready

   while read -r table; do
      if [[ "$table" == metadata ]]; then has_metadata_table=1; fi
   done < <(mysql "--defaults-file=${PILER_MY_CNF}" "$MYSQL_DATABASE" <<< 'show tables')

   if [[ $has_metadata_table -eq 0 ]]; then
      log "no metadata table, creating tables"

      mysql "--defaults-file=${PILER_MY_CNF}" "$MYSQL_DATABASE" < /usr/share/piler/db-mysql.sql
   else
      log "metadata table exists"
   fi

   if [[ -v ADMIN_USER_PASSWORD_HASH ]]; then
      mysql "--defaults-file=${PILER_MY_CNF}" "$MYSQL_DATABASE" <<< "update user set password='${ADMIN_USER_PASSWORD_HASH}' where uid=0"
   fi
}


create_my_cnf_files() {
   printf "[client]\nhost = %s\nuser = %s\npassword = %s\n[mysqldump]\nhost = %s\nuser = %s\npassword = %s\n" \
      "$MYSQL_HOSTNAME" "$MYSQL_USER" "$MYSQL_PASSWORD" "$MYSQL_HOSTNAME" "$MYSQL_USER" "$MYSQL_PASSWORD" \
      > "$PILER_MY_CNF"

   give_it_to_piler "$PILER_MY_CNF"
}


start_services() {
   service cron start
   service php8.3-fpm start
   service nginx start
   rsyslogd
}


start_piler() {
   # No pid file should exist for piler
   rm -f /var/run/piler/*pid

   /etc/init.d/rc.piler start
}


pre_flight_check
fix_configs
create_my_cnf_files
init_database
start_services
start_piler

sleep infinity
