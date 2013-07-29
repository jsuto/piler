#!/bin/sh


usage() {
   echo "usage: $0 <username> <group> <sysconfdir> <localstatedir> <libexecdir>";
   exit 1;
}


load_default_values() {
   PILERUSER="piler"
   SYSCONFDIR="/usr/local/etc"
   LOCALSTATEDIR="/var"
   LIBEXECDIR="/usr/local/libexec"

   KEYTMPFILE="piler.key"
   KEYFILE="$SYSCONFDIR/piler.key"

   HOSTNAME=`hostname --fqdn`

   MYSQL_HOSTNAME="localhost"
   MYSQL_DATABASE="piler"
   MYSQL_USERNAME="piler"
   MYSQL_PASSWORD=""
   MYSQL_ROOT_PASSWORD=""

   SPHINXCFG="/usr/local/etc/sphinx.conf"

   WWWGROUP="apache"
   DOCROOT="/var/www/$HOSTNAME"

   SMARTHOST=""
   SMARTHOST_PORT=25
}


display_install_intro() {
  echo ""
  echo ""
  echo "This is the postinstall utility for piler"
  echo "It should be run only at the first install. DO NOT run on an existing piler installation!"
  echo ""

  askYN "Continue? [Y/N]" "N"
  if [ $response != "yes" ]; then
     echo "Aborted."
     exit
  fi

  echo ""
}


check_user() {
   user=$1

   if [ x`whoami` != x$user ]; then echo "ERROR: postinstaller must be run as $user user"; exit 1; fi
}


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


