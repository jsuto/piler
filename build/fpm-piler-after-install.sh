#!/bin/bash

set -o errexit
set -o pipefail
set -o nounset

config_dir=/etc/piler
SSL_CERT_DATA="/C=HU/ST=Pest/L=Budapest/O=Example/CN=archive.example.com"
WWW_GROUP="www-data"


error() {
   echo "ERROR:" "$*" 1>&2
   exit 1
}


log() {
   echo "DEBUG:" "$*"
}


make_certificate() {
   log "Making an ssl certificate"
   openssl req -new -newkey rsa:4096 -days 3650 -nodes -x509 -subj "$SSL_CERT_DATA" -keyout "${config_dir}/piler.pem" -out "${config_dir}/1.cert" -sha256 2>/dev/null
   cat "${config_dir}/1.cert" >> "${config_dir}/piler.pem"
   rm -f "${config_dir}/1.cert"
   chown root:piler "${config_dir}/piler.pem"
   chmod 640 "${config_dir}/piler.pem"
}


make_piler_key() {
   log "Generating piler.key"
   dd if=/dev/urandom bs=56 count=1 of="${config_dir}/piler.key" 2>/dev/null
   [[ $(stat -c '%s' "${config_dir}/piler.key") -eq 56 ]] || error "could not read 56 bytes from /dev/urandom to ${config_dir}/piler.key"
   chown root:piler "${config_dir}/piler.key"
   chmod 640 "${config_dir}/piler.key"
}


if [[ -f /etc/redhat-release ]]; then
   WWW_GROUP="php-fpm"
   if grep -E 'Linux release (8|9)' /etc/redhat-release; then
      WWW_GROUP="apache"
   fi
fi
if [[ -d /etc/YaST2 ]]; then WWW_GROUP="nobody"; fi

log "adding setuid permissions to piler binaries"

for i in pileraget pilerexport pilerget pilerimport pilertest reindex pilerstats; do
   chown piler:piler /usr/bin/$i
   chmod +s /usr/bin/$i
done

chmod 755 /var/piler
chgrp "$WWW_GROUP" /var/piler/export

[[ -f "${config_dir}/piler.key" ]] || make_piler_key
[[ -f "${config_dir}/piler.pem" ]] || make_certificate

log "fixing /var/piler/run ownership"

chown piler:piler /var/piler/error /var/piler/imap /var/piler/stat /var/piler/store /var/piler/tmp /var/piler/run

if [[ -d /var/piler/sphinx ]]; then
   chown piler:piler /var/piler/sphinx
fi

if [[ -d /var/piler/manticore ]]; then
   chown piler:piler /var/piler/manticore
fi

if [[ -d /var/piler/astore ]]; then
   chown piler:piler /var/piler/astore /var/piler/customer /var/piler/license
fi

chown "piler:${WWW_GROUP}" /var/piler/www/tmp /var/piler/www/images
chmod 775 /var/piler/www/tmp /var/piler/www/images

log "Checking for /var/piler/customer"

if [[ -d /var/piler/customer ]]; then
   chown "piler:${WWW_GROUP}" /var/piler/customer
   chmod 770 /var/piler/customer
fi

if [[ -f /etc/piler/manticore.conf.dist && ! -f /etc/piler/manticore.conf ]]; then
   cp /etc/piler/manticore.conf.dist /etc/piler/manticore.conf
   chmod +x /etc/piler/manticore.conf
fi

if [[ -f /etc/piler/manticore.sql.dist && ! -f /etc/piler/manticore.sql ]]; then
   cp /etc/piler/manticore.sql.dist /etc/piler/manticore.sql
fi

log "Checking for /etc/piler/config-site.php"

if [[ ! -f /etc/piler/config-site.php ]] && [[ -f /etc/piler/config-site.dist.php ]]; then
   cp /etc/piler/config-site.dist.php /etc/piler/config-site.php
fi

log "Checking for /etc/piler/sites"

if [[ ! -d /etc/piler/sites ]]; then
   mkdir /etc/piler/sites
fi

chown piler:piler /etc/piler/sites

log "Checking for /etc/piler/sites/customer-sites.php"

if [[ ! -f /etc/piler/sites/customer-sites.php ]]; then
   printf '<?php\n' > /etc/piler/sites/customer-sites.php
   chown piler:piler /etc/piler/sites/customer-sites.php
fi

if [[ -f /etc/piler/manticore.tmpl.slave.dist ]]; then
   log "Checking for /etc/piler/manticore.tmpl.slave"

   [[ -f /etc/piler/manticore.tmpl.slave ]] || cp /etc/piler/manticore.tmpl.slave.dist /etc/piler/manticore.tmpl.slave
fi

if [[ -f /etc/piler/manticore.tmpl.rt.slave.dist ]]; then
   log "Checking for /etc/piler/manticore.tmpl.rt.slave"

   [[ -f /etc/piler/manticore.tmpl.rt.slave ]] || cp /etc/piler/manticore.tmpl.rt.slave.dist /etc/piler/manticore.tmpl.rt.slave
fi

if [[ ! -f /etc/piler/search.help && -f /etc/piler/search.help.dist ]]; then
   log "Copying search.help"
   cp /etc/piler/search.help.dist /etc/piler/search.help
fi

if [[ -f /etc/apt/sources.list ]]; then
   [[ $(grep -c stretch /etc/apt/sources.list) -eq 0 ]] || sed -i 's/Engine=InnoDB;/Engine=InnoDB ROW_FORMAT=DYNAMIC;/g' /usr/share/piler/db-mysql.sql
fi

if [[ -f /usr/libexec/piler/piler.service ]]; then
   ln -sf /usr/libexec/piler/piler.service /lib/systemd/system
   /usr/bin/systemctl enable --now piler.service
fi

if [[ -f /usr/libexec/piler/piler-smtp.service ]]; then
   ln -sf /usr/libexec/piler/piler-smtp.service /lib/systemd/system
   /usr/bin/systemctl enable --now piler-smtp.service
fi

if [[ -f /usr/libexec/piler/pilersearch.service ]]; then
   ln -sf /usr/libexec/piler/pilersearch.service /lib/systemd/system
   /usr/bin/systemctl enable --now pilersearch.service
fi

if [[ -f /usr/libexec/piler/tika.service ]]; then
   ln -sf /usr/libexec/piler/tika.service /lib/systemd/system
   /usr/bin/systemctl enable --now tika.service
fi

if [[ $(pgrep -c systemd) -gt 0 && -x /bin/systemctl ]]; then
   systemctl daemon-reload
fi

if [[ -f /var/piler/www/config.php ]]; then
   PILER_VERSION="$( piler -v )"
   for f in /var/piler/www/config.php /var/piler/www/vue/Footer.vue; do
      if [[ -f "$f" ]]; then sed -i "s/PILER_VERSION/${PILER_VERSION}/" $f; fi
   done
fi


log "postinstall has finished"
