#!/bin/bash

HOSTNAME=`hostname -f`
PILER_HOST_IP="127.0.0.1"
SMARTHOST=""
PILERUSER="piler"
MYSQL_HOSTNAME="localhost"
MYSQL_DATABASE="piler"
MYSQL_USERNAME="piler"
MYSQL_PASSWORD="verystrongpassword"
MYSQL_ROOT_PASSWORD="piler123"
KEYFILE="/usr/local/etc/piler.key"
KEYTMPFILE="key.tmp"
DOCROOT="/var/www/piler"
WWWGROUP="www-data"
SSL_CERT_DATA="/C=US/ST=Denial/L=Springfield/O=Dis/CN=www.example.com"
SPHINX_PILER_CONFIG="/usr/local/etc/sphinx.conf"
CRON_TMP=aaaa.11


isFQDN() {
  # we need min. 2 dots
  if [ x"$1" = "xdogfood" ]; then
    echo 1
    return
  fi

  if [ x"$1" = "x" ]; then
    echo 0
    return
  fi

  NF=`echo $1 | awk -F. '{print NF}'`
  if [ $NF -ge 2 ]; then
    echo 1
  else
    echo 0
  fi
}


ask() {
  PROMPT=$1
  DEFAULT=$2

  echo ""
  echo -n "$PROMPT [$DEFAULT] "
  read response

  if [ -z $response ]; then
    response=$DEFAULT
  fi
}


askNonBlankNoEcho() {
  PROMPT=$1
  DEFAULT=$2

  while [ 1 ]; do
    stty -echo
    ask "$PROMPT" "$DEFAULT"
    stty echo
    echo ""
    if [ ! -z $response ]; then
      break
    fi
    echo "A non-blank answer is required"
  done
}


askNoEcho() {
  PROMPT=$1
  DEFAULT=$2

  stty -echo
  ask "$PROMPT" "$DEFAULT"
  stty echo
  echo ""
}


askNonBlank() {
  PROMPT=$1
  DEFAULT=$2

  while [ 1 ]; do
    ask "$PROMPT" "$DEFAULT"
    if [ ! -z $response ]; then
      break
    fi
    echo "A non-blank answer is required"
  done
}



askYN() {
  PROMPT=$1
  DEFAULT=$2

  if [ "x$DEFAULT" = "xyes" -o "x$DEFAULT" = "xYes" -o "x$DEFAULT" = "xy" -o "x$DEFAULT" = "xY" ]; then
    DEFAULT="Y"
  else
    DEFAULT="N"
  fi

  while [ 1 ]; do
    ask "$PROMPT" "$DEFAULT"
    response=$(perl -e "print lc(\"$response\");")
    if [ -z $response ]; then
      :
    else
      if [ $response = "yes" -o $response = "y" ]; then
        response="yes"
        break
      else
        if [ $response = "no" -o $response = "n" ]; then
          response="no"
          break
        fi
      fi
    fi
    echo "A Yes/No answer is required"
  done
}


check_user() {
   user=$1

   if [ x`whoami` != x$user ]; then echo "ERROR: the installer must be run as $user user"; exit 1; fi
}


get_mysql_root_pwd() {

   askNoEcho "Please enter mysql root password" ""
   MYSQL_ROOT_PASSWORD=$response

   s=`echo "use information_schema; select TABLE_NAME from TABLES where TABLE_SCHEMA='$MYSQL_DATABASE'" | mysql -h $MYSQL_HOSTNAME -u root --password=$MYSQL_ROOT_PASSWORD`
   if [ $? -eq 0 ];
   then
      echo "mysql connection successful"; echo;
      if [ `echo $s | grep -c metadata` -eq 1 ]; then echo "ERROR: Detected metadata table in $MYSQL_DATABASE. Aborting"; exit 0; fi
   else
      echo "ERROR: failed to connect to mysql";
      get_mysql_root_pwd
   fi

}


collect_data() {
   askNonBlank "Please enter FQDN of this host" "$HOSTNAME"
   HOSTNAME=$response

   ask "Please enter the FQDN / IP-address of the smarthost" "$SMARTHOST"
   SMARTHOST=$response

   askNonBlank "Please enter the group of the webserver" "$WWWGROUP"
   WWWGROUP=$response

   askNonBlank "Please enter SSL certificate data for the piler deamon" "$SSL_CERT_DATA"
   SSL_CERT_DATA=$response

   askNonBlankNoEcho "Please enter mysql password for $MYSQL_USERNAME" ""
   MYSQL_PASSWORD=$response

   get_mysql_root_pwd

   show_install_parameters

   askYN "Accept and continue? [Y/n]: " "Y"
   YN=$response

   if test "$YN" = "no"; then collect_data; fi

   hostname $HOSTNAME

}


show_install_parameters() {
   echo
   echo "Configuration details:"
   echo "----------------------"
   echo
   echo "Hostname: $HOSTNAME"
   echo "IP-address: $PILER_HOST_IP"
   echo "Smarthost: $SMARTHOST"
   echo "Documentroot: $DOCROOT"
   echo "Webserver user: $WWWGROUP"
   echo "Piler SSL cert data: $SSL_CERT_DATA"
}


show_licence() {
   echo
   echo "This is the piler first time postinstall for docker script."
   echo "DO NOT run the postinstall if you have done it before!"
   echo
   echo "Enter Y to accept the licence and continue."
   echo

   askYN "Accept licence? [y/N]: " "N"
   YN=$response

   if test "$YN" != "yes"; then echo "Aborted."; exit 1; fi
}


configure_piler() {


cat <<PILERCONF > /usr/local/etc/piler.conf
archive_emails_not_having_message_id=0
archive_only_mydomains=0
backlog=20
cipher_list=HIGH:MEDIUM
clamd_socket=/tmp/clamd
debug=0
default_retention_days=2557
enable_cjk=0
encrypt_messages=1
extra_to_field=X-Envelope-To:
hostid=$HOSTNAME
iv=
listen_addr=0.0.0.0
listen_port=25
locale=
max_requests_per_child=1000
memcached_servers=127.0.0.1
memcached_to_db_interval=900
memcached_ttl=86400
min_word_len=1
mysqlhost=
mysqlport=0
mysqlsocket=/var/run/mysqld/mysqld.sock
mysqluser=$MYSQL_USERNAME
mysqlpwd=$MYSQL_PASSWORD
mysqldb=$MYSQL_DATABASE
mysql_connect_timeout=2
number_of_worker_processes=10
pemfile=/usr/local/etc/piler.pem
pidfile=/var/run/piler/piler.pid
piler_header_field=X-piler: piler already archived this email
queuedir=/var/piler/store
server_id=0
session_timeout=420
syslog_recipients=1
spam_header_line=
tls_enable=1
username=piler
verbosity=1
workdir=/var/piler/tmp
PILERCONF

}


writing_piler_crontab() {

cat <<CRONTAB > $CRON_TMP

### PILERSTART
5,35 * * * * /usr/local/libexec/piler/indexer.delta.sh
30   2 * * * /usr/local/libexec/piler/indexer.main.sh
*/15 * * * * /usr/local/bin/indexer --quiet tag1 --rotate
*/15 * * * * /usr/local/bin/indexer --quiet note1 --rotate
30   6 * * * /usr/bin/php /usr/local/libexec/piler/generate_stats.php --webui /var/www/piler > /dev/null
*/5 * * * * /usr/bin/find /var/www/piler/tmp -type f -name i.\* -exec rm -f {} \;

### PILEREND
CRONTAB

crontab -u $PILERUSER $CRON_TMP

rm -f $CRON_TMP

}


piler_postinstall() {

cd /tmp

mkdir -p /var/piler/tmp /var/piler/stat /var/piler/imap /var/piler/store/00 /var/piler/import /var/piler/export /var/piler/sphinx
chown -R piler:piler /var/piler/tmp /var/piler/stat /var/piler/imap /var/piler/store /var/piler/import /var/piler/export /var/piler/sphinx

echo -n "Creating mysql database... ";
sed -e "s%MYSQL_HOSTNAME%$MYSQL_HOSTNAMEg%" -e "s%MYSQL_DATABASE%$MYSQL_DATABASE%g" -e "s%MYSQL_USERNAME%$MYSQL_USERNAME%g" -e "s%MYSQL_PASSWORD%$MYSQL_PASSWORD%g" /usr/local/share/piler/db-mysql-root.sql.in | mysql -h $MYSQL_HOSTNAME -u root --password=$MYSQL_ROOT_PASSWORD
mysql -h $MYSQL_HOSTNAME -u $MYSQL_USERNAME --password=$MYSQL_PASSWORD $MYSQL_DATABASE < /usr/local/share/piler/db-mysql.sql
echo "Done."

echo -n "Overwriting sphinx configuration... ";
sed -e "s%MYSQL_HOSTNAME%$MYSQL_HOSTNAME%" -e "s%MYSQL_DATABASE%$MYSQL_DATABASE%" -e "s%MYSQL_USERNAME%$MYSQL_USERNAME%" -e "s%MYSQL_PASSWORD%$MYSQL_PASSWORD%" jsuto-piler-*/etc/sphinx.conf.in > /usr/local/etc/sphinx.conf
echo "Done."

echo -n "Initializing sphinx indices... ";
su $PILERUSER -c "indexer --all"
echo "Done."

echo -n "Generating openssh keys ... ";
/usr/bin/ssh-keygen -t dsa -f /etc/ssh/ssh_host_dsa_key -N ''
/usr/bin/ssh-keygen -t rsa -f /etc/ssh/ssh_host_rsa_key -N ''
/usr/bin/ssh-keygen -t ecdsa -f /etc/ssh/ssh_host_ecdsa_key -N ''
echo "Done."

echo -n "Making an ssl certificate ... "
openssl req -new -newkey rsa:4096 -days 3650 -nodes -x509 -subj "$SSL_CERT_DATA" -keyout /usr/local/etc/piler.pem -out 1.cert
cat 1.cert >> /usr/local/etc/piler.pem
chmod 600 /usr/local/etc/piler.pem
rm 1.cert


echo -n "installing keyfile ($KEYTMPFILE) to $KEYFILE... "
dd if=/dev/urandom bs=56 count=1 of=$KEYTMPFILE
cp $KEYTMPFILE $KEYFILE
chgrp $PILERUSER $KEYFILE
chmod 640 $KEYFILE
rm -f $KEYTMPFILE
echo "Done."

cat <<NGINXCONFIG > /usr/local/etc/nginx-piler.conf

server {
        server_name  $HOSTNAME;

        root /var/www/piler;

        access_log  /var/log/nginx/$HOSTNAME-access.log;
        error_log   /var/log/nginx/$HOSTNAME-error.log;

        gzip  on;
        gzip_types text/plain application/xml text/css;
        gzip_vary on;

        location / {
            index  index.php index.html;
        }

        #error_page  404              /404.html;

        error_page   500 502 503 504  /50x.html;
        location = /50x.html {
            root   html;
        }

        location ~ [^/]\.php(/|$) {
            fastcgi_split_path_info ^(.+?\.php)(/.*)$;
            if (!-f \$document_root\$fastcgi_script_name) {
               return 404;
            }

            fastcgi_pass unix:/var/run/php5-fpm.sock;
            fastcgi_index index.php;
            include fastcgi_params;
        }

        location ~* \.(ico|css|js|gif|jpe?g|png)$ {
            expires 2w;
        }

        rewrite /search.php /index.php?route=search/search&type=simple;
        rewrite /advanced.php /index.php?route=search/search&type=advanced;
        rewrite /expert.php /index.php?route=search/search&type=expert;
        rewrite /search-helper.php /index.php?route=search/helper;
        rewrite /audit-helper.php /index.php?route=audit/helper;
        rewrite /message.php /index.php?route=message/view;
        rewrite /bulkrestore.php /index.php?route=message/bulkrestore;
        rewrite /bulkpdf.php /index.php?route=message/bulkpdf;
        rewrite /folders.php /index.php?route=folder/list&;
        rewrite /settings.php /index.php?route=user/settings;
        rewrite /login.php /index.php?route=login/login;
        rewrite /logout.php /index.php?route=login/logout;
        rewrite /google.php /index.php?route=login/google;
        rewrite /domain.php /index.php?route=domain/domain;
        rewrite /ldap.php /index.php?route=ldap/list;
        rewrite /customer.php /index.php?route=customer/list;
        rewrite /retention.php /index.php?route=policy/retention;
        rewrite /archiving.php /index.php?route=policy/archiving;
        rewrite /view/javascript/piler.js /js.php;
}
NGINXCONFIG


cat <<CONFIGSITE > /usr/local/etc/config-site.php
<?php

\$config['SITE_NAME'] = '$HOSTNAME';
\$config['SITE_URL'] = 'http://' . \$config['SITE_NAME'] . '/';
\$config['DIR_BASE'] = '$DOCROOT/';

\$config['TIMEZONE'] = 'Europe/Budapest';

\$config['ENABLE_SYSLOG'] = 1;

\$config['SMTP_DOMAIN'] = '$HOSTNAME';
\$config['SMTP_FROMADDR'] = 'no-reply@$HOSTNAME';
\$config['ADMIN_EMAIL'] = 'admin@$HOSTNAME';

\$config['DB_DRIVER'] = 'mysql';
\$config['DB_PREFIX'] = '';
\$config['DB_HOSTNAME'] = '$MYSQL_HOSTNAME';
\$config['DB_USERNAME'] = '$MYSQL_USERNAME';
\$config['DB_PASSWORD'] = '$MYSQL_PASSWORD';
\$config['DB_DATABASE'] = '$MYSQL_DATABASE';

\$config['PILER_HOST'] = '$PILER_HOST_IP';

\$config['SMARTHOST'] = '$SMARTHOST';
\$config['SMARTHOST_PORT'] = 25;

?>
CONFIGSITE

}




check_user root
show_licence


collect_data


###write_nginx_vhost



configure_piler

piler_postinstall

writing_piler_crontab

echo "changing root password:"
passwd root


