#!/bin/bash

set -o errexit
set -o pipefail
set -o nounset
set -x

PILER_HOSTNAME="${PILER_HOSTNAME:-archive.yourdomain.com}"
MYSQL_ROOT_PASSWORD="${MYSQL_ROOT_PASSWORD:-abcde123}"
MYSQL_PILER_PASSWORD="${MYSQL_PILER_PASSWORD:-piler123}"
SERVER_ID="${SERVER_ID:-0}"
USE_SMTP_GATEWAY="${USE_SMTP_GATEWAY:-0}"
SPHINX_WORKER_LISTEN_ADDRESS="${SPHINX_WORKER_LISTEN_ADDRESS:-}"
PHP_FPM_SOCKET="/var/run/php/php7.4-fpm.sock"

MYSQL_HOSTNAME="localhost"
MYSQL_DATABASE="piler"
MYSQL_USERNAME="piler"

SPHINX_TARGZ="sphinx-3.3.1-bin.tar.gz"
DOWNLOAD_URL="https://download.mailpiler.com"
PILER_DEB="piler_1.3.12-focal-a1b71bdd_amd64.deb"
PILER_USER="piler"
CONFIG_SITE_PHP="/etc/piler/config-site.php"

export DEBIAN_FRONTEND=noninteractive

install_prerequisites() {
   apt-get update
   apt-get -y --no-install-recommends install \
      wget rsyslog openssl sysstat php7.4-cli php7.4-cgi php7.4-mysql php7.4-fpm php7.4-zip php7.4-ldap \
      php7.4-gd php7.4-curl php7.4-xml ca-certificates zip catdoc unrtf poppler-utils nginx tnef libzip5 \
      libtre5 libwrap0 cron libmariadb-dev python3 python3-mysqldb libmariadb-dev mariadb-client-core-10.3 \
      mariadb-server-10.3

   wget -q -O "/tmp/${SPHINX_TARGZ}" "${DOWNLOAD_URL}/generic-local/${SPHINX_TARGZ}"
   tar -C / -zxvf "/tmp/${SPHINX_TARGZ}"
}


fix_mysql_settings() {
   cat > /etc/mysql/mariadb.conf.d/99-piler.cnf << PILER_CNF
[mysqld]

innodb_buffer_pool_size=512M
innodb_flush_log_at_trx_commit=1
innodb_log_buffer_size=64M
innodb_log_file_size=64M
innodb_read_io_threads=4
innodb_write_io_threads=4
innodb_log_files_in_group=2

innodb_file_per_table
PILER_CNF
}

start_mysql() {
   fix_mysql_settings

   service mysql restart
   mysqladmin -u root password "${MYSQL_ROOT_PASSWORD}"
}


install_piler() {
   wget "${DOWNLOAD_URL}/piler/${PILER_DEB}"
   dpkg -i "$PILER_DEB"
   rm -f "$PILER_DEB"

   crontab -u "$PILER_USER" /usr/share/piler/piler.cron

   rm -f "$PILER_DEB" /etc/nginx/sites-enabled/default
   ln -sf /etc/piler/piler-nginx.conf /etc/nginx/sites-enabled/piler.conf

   touch /var/piler/.bash_history
   chown "${PILER_USER}:${PILER_USER}" /var/piler/.bash_history
}


create_my_cnf() {
   local user=$1
   local password=$2
   local my_cnf=$3

   printf "[client]\\n\\nhost = %s\\nuser = %s\\npassword = %s\\n" "$MYSQL_HOSTNAME" "$user" "$password" > "$my_cnf"
   printf "\\n\\n[mysqldump]\\n\\nhost = %s\\nuser = %s\\npassword = %s\\n" "$MYSQL_HOSTNAME" "$user" "$password" >> "$my_cnf"
   chown $PILER_USER:$PILER_USER "$my_cnf"
   chmod 600 "$my_cnf"
}


fix_config_site_php() {
   sed -i -e "s%HOSTNAME%${PILER_HOSTNAME}%g" -e "s%MYSQL_PASSWORD%${MYSQL_PILER_PASSWORD}%g" "$CONFIG_SITE_PHP"

   {
      echo "\$config['SERVER_ID'] = $SERVER_ID;"
      echo "\$config['USE_SMTP_GATEWAY'] = $USE_SMTP_GATEWAY;"
      echo "\$config['SPHINX_VERSION'] = 331;"
   } >> "$CONFIG_SITE_PHP"

   if [[ "$SPHINX_WORKER_LISTEN_ADDRESS" ]]; then
      echo "\$config['SPHINX_WORKER_LISTEN_ADDRESS'] = '$SPHINX_WORKER_LISTEN_ADDRESS';" >> "$CONFIG_SITE_PHP"
   fi

   echo "\$config['ARCHIVE_HOST'] = '$PILER_HOSTNAME';" >> "$CONFIG_SITE_PHP"
}


init_db() {
   sed -e "s%MYSQL_HOSTNAME%$MYSQL_HOSTNAME%g" -e "s%MYSQL_DATABASE%$MYSQL_DATABASE%g" -e "s%MYSQL_USERNAME%$MYSQL_USERNAME%g" -e "s%MYSQL_PASSWORD%$MYSQL_PILER_PASSWORD%g" \
      /usr/share/piler/db-mysql-root.sql.in | mysql --defaults-file=/etc/piler/.my.cnf-root -h $MYSQL_HOSTNAME
   mysql --defaults-file=/etc/piler/.my.cnf -h $MYSQL_HOSTNAME $MYSQL_DATABASE < /usr/share/piler/db-mysql.sql
}


add_systemd_services() {
   pushd /etc/systemd/system

   ln -sf /usr/libexec/piler/piler.service .
   ln -sf /usr/libexec/piler/piler-smtp.service .
   ln -sf /usr/libexec/piler/pilersearch.service .

   popd

   systemctl daemon-reload

   systemctl enable piler
   systemctl enable piler-smtp
   systemctl enable pilersearch
}


fix_configs() {
   if [[ ! -f /etc/piler/piler-nginx.conf ]]; then
      sed -e "s%PILER_HOST%$PILER_HOSTNAME%g" -e "s%PHP_FPM_SOCKET%$PHP_FPM_SOCKET%g" /etc/piler/piler-nginx.conf.dist > /etc/piler/piler-nginx.conf
      nginx -t
      nginx -s reload
   fi

   if [[ ! -f /etc/piler/piler.conf ]]; then
      sed -e "s/verystrongpassword/$MYSQL_PILER_PASSWORD/g" -e "s/piler.yourdomain.com/$PILER_HOSTNAME/g" /etc/piler/piler.conf.dist > /etc/piler/piler.conf
      chmod 600 /etc/piler/piler.conf
      chown $PILER_USER:$PILER_USER /etc/piler/piler.conf
   fi

   sed -i -e "s/MYSQL_HOSTNAME/${MYSQL_HOSTNAME}/g" \
          -e "s/MYSQL_DATABASE/${MYSQL_DATABASE}/g" \
          -e "s/MYSQL_USERNAME/${MYSQL_USERNAME}/g" \
          -e "s/MYSQL_PASSWORD/${MYSQL_PILER_PASSWORD}/g" \
          /etc/piler/sphinx.conf
}


install_prerequisites

start_mysql

install_piler

create_my_cnf "root" "${MYSQL_ROOT_PASSWORD}" /etc/piler/.my.cnf-root
create_my_cnf "piler" "${MYSQL_PILER_PASSWORD}" /etc/piler/.my.cnf

fix_config_site_php

add_systemd_services

fix_configs
init_db

su -c "indexer --all -c /etc/piler/sphinx.conf" $PILER_USER

[[ ! -d /var/run/piler ]] || mkdir -p /var/run/piler

systemctl start pilersearch
systemctl start piler
systemctl start piler-smtp
