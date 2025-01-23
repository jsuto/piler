#!/bin/bash

set -o nounset
set -o errexit
set -o pipefail

export DEBIAN_FRONTEND="noninteractive"

install_2004_packages() {
   apt-get update
   apt-get install --no-install-recommends -y cron gnupg nginx sudo jq curl wget unzip openssl openssh-client make gcc sysstat catdoc unrtf poppler-utils tnef \
      ca-certificates libssl-dev libtre-dev libzip-dev libmariadb-dev libcurl4-openssl-dev mariadb-server \
      php7.4-fpm php7.4-cli php7.4-mysql php7.4-zip php7.4-ldap php7.4-curl php7.4-xml php7.4-memcached php7.4-mbstring
}

install_2404_packages() {
   apt-get update
   apt-get install --no-install-recommends -y cron gnupg nginx sudo jq curl wget unzip openssl openssh-client make gcc sysstat catdoc unrtf poppler-utils tnef \
      ca-certificates libssl-dev libtre-dev libzip-dev libmariadb-dev libcurl4-openssl-dev mariadb-server \
      php8.3-fpm php8.3-cli php8.3-mysql php8.3-zip php8.3-ldap php8.3-curl php8.3-xml php8.3-memcached php8.3-mbstring
}

create_piler_user() {
   groupadd piler
   useradd -g piler -s /bin/bash -d /var/piler piler
   usermod -L piler
}

build_piler() {
   wget https://github.com/jsuto/piler/archive/refs/tags/piler-1.4.7.tar.gz
   tar zxf piler-1.4.7.tar.gz
   pushd piler-piler-1.4.7
   ./configure --prefix=/usr --sysconfdir=/etc --localstatedir=/var
   make clean all
   sudo make install
   sudo cp contrib/webserver/piler-nginx.conf /etc/piler/
   ln -sf /etc/piler/piler-nginx.conf /etc/nginx/sites-enabled/
   rm -f /etc/nginx/sites-enabled/default
   sed -e s%LOCALSTATEDIR%/var%g -e s%SYSCONFDIR%/etc%g -e s%LIBEXECDIR%/usr/libexec%g etc/cron.jobs.in | crontab -u piler -
}

install_2004_packages
#install_2404_packages
create_piler_user
build_piler


# TODO: sphinx version to install?
# Crontab: fix tag/note to sphinx
# config-site.php
# piler.conf
# piler.key
