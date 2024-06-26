#!/bin/bash

set -o errexit
set -o pipefail
set -o nounset
set -x

MY_IP="1.2.3.4"
PILER_HOSTNAME="${PILER_HOSTNAME:-archive.example.com}"
MYSQL_ROOT_PASSWORD="${MYSQL_ROOT_PASSWORD:-abcde123}"
MYSQL_PILER_PASSWORD="${MYSQL_PILER_PASSWORD:-piler123}"
SERVER_ID="${SERVER_ID:-0}"
USE_SMTP_GATEWAY="${USE_SMTP_GATEWAY:-0}"
SPHINX_WORKER_LISTEN_ADDRESS="${SPHINX_WORKER_LISTEN_ADDRESS:-}"
PHP_FPM_SOCKET="/var/run/php/php8.1-fpm.sock"

MYSQL_HOSTNAME="localhost"
MYSQL_DATABASE="piler"
MYSQL_USERNAME="piler"

DOWNLOAD_URL="https://download.mailpiler.com"
PILER_DEB="piler_1.4.4-jammy-0573c6da_amd64.deb"
PILER_USER="piler"
PILER_CONF="/etc/piler/piler.conf"
CONFIG_SITE_PHP="/etc/piler/config-site.php"
SEARCHCFG="/etc/piler/manticore.conf"

TRAEFIK_VERSION="v2.11.0"
ARCH="amd64"

export DEBIAN_FRONTEND=noninteractive

install_prerequisites() {
   apt-get update
   apt-get -y --no-install-recommends install \
      wget rsyslog openssl sysstat php8.1-cli php8.1-cgi php8.1-mysql \
      php8.1-fpm php8.1-zip php8.1-ldap php8.1-gd php8.1-curl php8.1-xml \
      php8.1-mbstring ca-certificates zip catdoc unrtf poppler-utils \
      nginx tnef libzip4 libtre5 libwrap0 cron libmariadb-dev python3 \
      python3-mysqldb libmariadb-dev mariadb-client-core-10.6 \
      mariadb-server-10.6

   wget "https://github.com/traefik/traefik/releases/download/${TRAEFIK_VERSION}/traefik_${TRAEFIK_VERSION}_linux_${ARCH}.tar.gz"
   tar zxvf "traefik_${TRAEFIK_VERSION}_linux_${ARCH}.tar.gz" traefik
   chown root:root traefik
   chmod 755 traefik
   mv traefik /usr/local/bin
   setcap cap_net_bind_service+ep /usr/local/bin/traefik

   wget https://repo.manticoresearch.com/manticore-repo.noarch.deb && \
   dpkg -i manticore-repo.noarch.deb && \
   apt-get update && \
   apt-get install -y manticore manticore-columnar-lib manticore-extra && \
   rm -f manticore-repo.noarch.deb
   systemctl stop manticore
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
   wget "https://bitbucket.org/jsuto/piler/downloads/${PILER_DEB}"
   dpkg -i "$PILER_DEB"
   rm -f "$PILER_DEB"

   sed -i 's/.*indexer.*/###/g' /usr/share/piler/piler.cron
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
   } >> "$CONFIG_SITE_PHP"

   if [[ "$SPHINX_WORKER_LISTEN_ADDRESS" ]]; then
      echo "\$config['SPHINX_WORKER_LISTEN_ADDRESS'] = '$SPHINX_WORKER_LISTEN_ADDRESS';" >> "$CONFIG_SITE_PHP"
   fi

   echo "\$config['ARCHIVE_HOST'] = '$PILER_HOSTNAME';" >> "$CONFIG_SITE_PHP"
   echo "\$config['SPHINX_MAIN_INDEX'] = 'piler1';" >> "$CONFIG_SITE_PHP"
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
   systemctl enable traefik
}


fix_configs() {
   if [[ ! -f /etc/piler/piler-nginx.conf ]]; then
      sed -e "s%PILER_HOST%$PILER_HOSTNAME%g" -e "s%PHP_FPM_SOCKET%$PHP_FPM_SOCKET%g" /etc/piler/piler-nginx.conf.dist > /etc/piler/piler-nginx.conf
      nginx -t
      nginx -s reload

      sed -i 's/server {/server {\n\tlisten 127.0.0.1:80;/' /etc/piler/piler-nginx.conf
      systemctl stop nginx
      sleep 5
      systemctl start nginx
   fi

   if [[ ! -f "$PILER_CONF" ]]; then
      sed -e "s/verystrongpassword/$MYSQL_PILER_PASSWORD/g" -e "s/piler.example.com/$PILER_HOSTNAME/g" /etc/piler/piler.conf.dist > "$PILER_CONF"
      chmod 600 "$PILER_CONF"
      chown $PILER_USER:$PILER_USER "$PILER_CONF"
   fi

   sed -i 's/tls_enable=0/tls_enable=1/g' "$PILER_CONF"
   sed -i "s%rtindex=.*%rtindex=1%" "$PILER_CONF"

   sed -i "s/define('RT.*/define('RT', 1);/" "$SEARCHCFG"
}

setup_traefik() {
   wget -O /etc/systemd/system/traefik.service "${DOWNLOAD_URL}/generic-local/traefik.service"

   mkdir -p /usr/local/etc/traefik
   touch /usr/local/etc/traefik/acme.json
   chmod 600 /usr/local/etc/traefik/acme.json
   chown www-data:www-data /usr/local/etc/traefik/acme.json

   cat > /usr/local/etc/traefik/traefik.yaml << TRAEFIK
log:
  level: INFO

entryPoints:
  websecure:
    address: "$MY_IP:443"

providers:
  file:
    filename: "/usr/local/etc/traefik/traefik.yaml"

certificatesResolvers:
  le:
    acme:
      storage: "/usr/local/etc/traefik/acme.json"
      email: admin@$PILER_HOSTNAME
      tlsChallenge: {}

tls:
  options:
    default:
      minVersion: VersionTLS13

http:
  middlewares:
    piler_headers:
      headers:
        customResponseHeaders:
          Server: ""
          Strict-Transport-Security: "max-age=31536000"
          X-Content-Type-Optionsi: "nosniff"
          Referrer-Policy: "same-origin"
  routers:
    master:
      rule: "Host(\`$PILER_HOSTNAME\`)"
      service: www
      middlewares:
        - "piler_headers"
      tls:
        certResolver: le
  services:
    www:
      loadBalancer:
        servers:
        - url: "http://127.0.0.1:80/"
TRAEFIK
}


install_prerequisites

start_mysql

install_piler

create_my_cnf "root" "${MYSQL_ROOT_PASSWORD}" /etc/piler/.my.cnf-root
create_my_cnf "piler" "${MYSQL_PILER_PASSWORD}" /etc/piler/.my.cnf

fix_config_site_php

setup_traefik

add_systemd_services

fix_configs
init_db

[[ ! -d /var/run/piler ]] || mkdir -p /var/run/piler

systemctl start pilersearch
systemctl start piler
systemctl start piler-smtp
systemctl start traefik
