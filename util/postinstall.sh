#!/bin/sh


INDEXER=`which indexer 2>/dev/null`
SEARCHD=`which searchd 2>/dev/null`
CRON_ORIG="/tmp/crontab.piler.orig"
CRON_TMP="/tmp/crontab.piler"


. ./util/utilfunc.sh


preinstall_check() {
   check_user root

   if [ "x$INDEXER" = "x" ]; then "ERROR: cannot find sphinx indexer"; echo ""; exit ; fi

   if [ "x$SEARCHD" = "x" ]; then "ERROR: cannot find sphinx searchd"; echo ""; exit 0; fi

   if [ -f $KEYFILE ]; then echo "ERROR: found existing keyfile ($KEYFILE), aborting install"; echo ""; exit 0; fi
}


gather_webserver_data() {
   askNonBlank "Please enter the path of your documentroot" "$DOCROOT"
   DOCROOT=$response

   if [ -d $DOCROOT ]; then echo "ERROR: vhost docroot should NOT exist, abroting install"; echo ""; exit 0; fi

   askNonBlank "Please enter the webserver groupname" "$WWWGROUP"
   WWWGROUP=$response
}


gather_mysql_account() {

   askNonBlank "Please enter mysql hostname" "$MYSQL_HOSTNAME"
   MYSQL_HOSTNAME=$response

   askNonBlank "Please enter mysql database" "$MYSQL_DATABASE"
   MYSQL_DATABASE=$response

   askNonBlank "Please enter mysql user name" "$MYSQL_USERNAME"
   MYSQL_USERNAME=$response

   askNonBlankNoEcho "Please enter mysql password for $MYSQL_USERNAME" ""
   MYSQL_PASSWORD=$response

   askNonBlankNoEcho "Please enter mysql root password" ""
   MYSQL_ROOT_PASSWORD=$response

   s=`echo "use information_schema; select TABLE_NAME from TABLES where TABLE_SCHEMA='$MYSQL_DATABASE'" | mysql -h $MYSQL_HOSTNAME -u root --password=$MYSQL_ROOT_PASSWORD`
   if [ $? -eq 0 ];
   then
      echo "mysql connection successful"; echo;
      if [ `echo $s | grep -c metadata` -eq 1 ]; then echo "ERROR: Detected metadata table in $MYSQL_DATABASE. Aborting"; exit 0; fi
   else
      echo "ERROR: failed to connect to mysql";
      gather_mysql_account
   fi

}


gather_sphinx_data() {

   if [ $INDEXER = "/usr/bin/indexer" ]; then SPHINXCFG="/etc/sphinx.conf"; fi

   askNonBlank "Please enter the path of sphinx.conf" "$SPHINXCFG"
   SPHINXCFG=$response
}


gather_smtp_relay_data() {
   ask "Please enter smtp relay" "$SMARTHOST"
   SMARTHOST=$response

   ask "Please enter smtp relay port" "$SMARTHOST_PORT"
   SMARTHOST_PORT=$response
}


make_cron_entries() {

   crontab -u $PILERUSER -l > $CRON_ORIG

   grep PILERSTART $CRON_ORIG > /dev/null 2>&1
   if [ $? != 0 ]; then
      cat /dev/null > $CRON_ORIG
   fi

   grep PILEREND $CRON_ORIG > /dev/null 2>&1
   if [ $? != 0 ]; then
      cat /dev/null > $CRON_ORIG
   fi


   rm -f $CRON_TMP

   echo ""
   echo "### PILERSTART" >> $CRON_TMP
   echo "*/5 * * * * LC_ALL=C mpstat | tail -1 | awk '{print \$11}' > $LOCALSTATEDIR/piler/stat/cpu.stat" >> $CRON_TMP

   echo "5,35 * * * * $LIBEXECDIR/piler/indexer.delta.sh" >> $CRON_TMP
   echo "30   2 * * * $LIBEXECDIR/piler/indexer.main.sh" >> $CRON_TMP
   echo "*/15 * * * * $INDEXER --quiet tag1 --rotate" >> $CRON_TMP
   echo "*/15 * * * * $INDEXER --quiet note1 --rotate" >> $CRON_TMP
   echo "### PILEREND" >> $CRON_TMP
}


make_new_key() {
   dd if=/dev/urandom bs=56 count=1 of=$KEYTMPFILE 2>/dev/null

   if [ `stat -c '%s' $KEYTMPFILE` -ne 56 ]; then echo "could not read 56 bytes from /dev/urandom to $KEYTMPFILE"; exit 1; fi
}


show_summary() {
   echo
   echo
   echo "INSTALLATION SUMMARY:"
   echo

   echo "piler user: $PILERUSER"
   echo "keyfile: $KEYFILE"
   echo

   echo "mysql host: $MYSQL_HOSTNAME"
   echo "mysql database: $MYSQL_DATABASE"
   echo "mysql username: $MYSQL_USERNAME"
   echo "mysql password: *******"
   echo

   echo "sphinx indexer: $INDEXER"
   echo "sphinx config file: $SPHINXCFG"
   echo

   echo "vhost docroot: $DOCROOT"
   echo "www group: $WWWGROUP"
   echo

   echo "smtp relay host: $SMARTHOST"
   echo "smtp relay port: $SMARTHOST_PORT"
   echo

   echo "piler crontab:"
   cat $CRON_TMP
   echo; echo;

   askYN "Correct? [Y/N]" "N"
   if [ $response != "yes" ]; then
      echo "Aborted."
      exit
   fi

}


execute_post_install_tasks() {

   askYN "Continue and modify system? [Y/N]" "N"
   if [ $response != "yes" ]; then
      echo "Aborted."
      exit
   fi

   echo;
   echo -n "Creating mysql database... ";
   sed -e "s%MYSQL_HOSTNAME%$MYSQL_HOSTNAMEg%" -e "s%MYSQL_DATABASE%$MYSQL_DATABASE%g" -e "s%MYSQL_USERNAME%$MYSQL_USERNAME%g" -e "s%MYSQL_PASSWORD%$MYSQL_PASSWORD%g" util/db-mysql-root.sql.in | mysql -h $MYSQL_HOSTNAME -u root --password=$MYSQL_ROOT_PASSWORD
   mysql -h $MYSQL_HOSTNAME -u $MYSQL_USERNAME --password=$MYSQL_PASSWORD $MYSQL_DATABASE < util/db-mysql.sql
   echo "Done."

   echo -n "Overwriting sphinx configuration... ";
   sed -e "s%MYSQL_HOSTNAME%$MYSQL_HOSTNAME%" -e "s%MYSQL_DATABASE%$MYSQL_DATABASE%" -e "s%MYSQL_USERNAME%$MYSQL_USERNAME%" -e "s%MYSQL_PASSWORD%$MYSQL_PASSWORD%" etc/sphinx.conf.in > etc/sphinx.conf
   cp etc/sphinx.conf $SPHINXCFG
   echo "Done."

   echo -n "Initializing sphinx indices... ";
   su $PILERUSER -c "indexer --all"
   echo "Done."


   echo -n "installing cron entries for $PILERUSER... "
   crontab -u $PILERUSER $CRON_TMP
   echo "Done."


   echo -n "installing keyfile ($KEYTMPFILE) to $KEYFILE... "
   cp $KEYTMPFILE $KEYFILE
   chgrp $PILERUSER $KEYFILE
   chmod 640 $KEYFILE
   rm -f $KEYTMPFILE
   echo "Done."

   echo -n "Copying www files to $DOCROOT... "
   mkdir -p $DOCROOT || exit 1

   cp -R webui/* $DOCROOT
   cp -R webui/.htaccess $DOCROOT
   chmod 770 $DOCROOT/tmp
   chgrp $WWWGROUP $DOCROOT/tmp

   echo "<?php" > $DOCROOT/config-site.php
   echo >> $DOCROOT/config-site.php

   echo "\$config['SITE_NAME'] = '$HOSTNAME';" >> $DOCROOT/config-site.php
   echo "\$config['SITE_URL'] = 'http://$HOSTNAME/';" >> $DOCROOT/config-site.php
   echo "\$config['DIR_BASE'] = '$DOCROOT/';" >> $DOCROOT/config-site.php

   echo >> $DOCROOT/config-site.php

   echo "\$config['ENABLE_SYSLOG'] = 1;" >> $DOCROOT/config-site.php

   echo >> $DOCROOT/config-site.php

   echo "\$config['SMTP_DOMAIN'] = '$HOSTNAME';" >> $DOCROOT/config-site.php
   echo "\$config['SMTP_FROMADDR'] = 'no-reply@$HOSTNAME';" >> $DOCROOT/config-site.php
   echo "\$config['ADMIN_EMAIL'] = 'admin@$HOSTNAME';" >> $DOCROOT/config-site.php

   echo >> $DOCROOT/config-site.php

   echo "\$config['DB_DRIVER'] = 'mysql';" >> $DOCROOT/config-site.php
   echo "\$config['DB_PREFIX'] = '';" >> $DOCROOT/config-site.php
   echo "\$config['DB_HOSTNAME'] = '$MYSQL_HOSTNAME';" >> $DOCROOT/config-site.php
   echo "\$config['DB_USERNAME'] = '$MYSQL_USERNAME';" >> $DOCROOT/config-site.php
   echo "\$config['DB_PASSWORD'] = '$MYSQL_PASSWORD';" >> $DOCROOT/config-site.php
   echo "\$config['DB_DATABASE'] = '$MYSQL_DATABASE';" >> $DOCROOT/config-site.php

   echo >> $DOCROOT/config-site.php

   echo "\$config['SMARTHOST'] = '$SMARTHOST';" >> $DOCROOT/config-site.php
   echo "\$config['SMARTHOST_PORT'] = $SMARTHOST_PORT;" >> $DOCROOT/config-site.php

   echo >> $DOCROOT/config-site.php

   echo "?>" >> $DOCROOT/config-site.php

   echo "Done."

}


clean_up_temp_stuff() {
   rm -f $CRON_TMP

   echo; echo "Done post installation tasks."; echo
}


load_default_values


if [ $# -ne 5 ]; then usage; fi

PILERUSER=$1
PILERGROUP=$2
SYSCONFDIR=$3
LOCALSTATEDIR=$4
LIBEXECDIR=$5

#LOGFILE="/tmp/piler-install.log.$$"
#touch $LOGFILE
#chmod 600 $LOGFILE

preinstall_check

display_install_intro

gather_webserver_data
gather_mysql_account
gather_sphinx_data
gather_smtp_relay_data


make_cron_entries
make_new_key

show_summary

execute_post_install_tasks

clean_up_temp_stuff


