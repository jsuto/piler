#!/bin/bash

set -o nounset
set -o errexit
set -o pipefail

REPO_ROOT="/repo"
DEBUG="${DEBUG:-false}"
DEBUG_OPTION=""
INSTALL_DIR="/tmp/PACKAGE"
PACKAGE_OUTDIR="/data"


set_mysql_flavour() {
   if [[ "$DISTRO" == "centos7" ]]; then
      MYSQL_FLAVOUR="mysql"
   else
      # shellcheck disable=SC2034
      MYSQL_FLAVOUR="mariadb"
   fi
}

get_pkg_name() {
   RPM_TAG="el9"

   if [[ "$DEBUG" == true ]]; then
      DEBUG_SUFFIX="-debug"
   else
      DEBUG_SUFFIX=""
   fi

   if [[ "$DISTRO" =~ oracle* || "$DISTRO" =~ centos* ]]; then
      PACKAGE="${PROJECT_ID}-${VERSION}-${COMMIT_ID}.${RPM_TAG}.x86_64.rpm"
   else
      PACKAGE="${PROJECT_ID}_${VERSION}-${DISTRO}-${COMMIT_ID}${DEBUG_SUFFIX}_${ARCH}.deb"
   fi

   rm -f "${PACKAGE_OUTDIR}/${PACKAGE}"
}

make_deb_package() {
   local project="$1"
   local description="$2"

   pushd "$PACKAGE_OUTDIR"

   fpm \
      --deb-use-file-permissions \
      -s dir \
      -C "$INSTALL_DIR" \
      -t deb \
      -n "$PROJECT_ID" \
      -v "${VERSION}-${DISTRO}" \
      --iteration "${COMMIT_ID}${DEBUG_SUFFIX}" \
      --description "$description" \
      --before-install "${REPO_ROOT}/build/fpm-${project}-before-install.sh" \
      --after-install "${REPO_ROOT}/build/fpm-${project}-after-install.sh"

   popd
}

make_certificate() {
   SSL_CERT_DATA="/C=US/ST=Denial/L=Springfield/O=Dis/CN=archive.example.com"
   config_dir=/etc/piler

   openssl req -new -newkey rsa:4096 -days 3650 -nodes -x509 -subj "$SSL_CERT_DATA" -keyout "${config_dir}/piler.pem" -out "${config_dir}/1.cert" -sha256 2>/dev/null
   cat "${config_dir}/1.cert" >> "${config_dir}/piler.pem"
   rm -f "${config_dir}/1.cert"
   chown root:piler "${config_dir}/piler.pem"
   chmod 640 "${config_dir}/piler.pem"
}

make_certificate

git config --global --add safe.directory /repo

pushd "$REPO_ROOT"

if [[ "$DEBUG" == "true" ]]; then
   DEBUG_OPTION="--enable-debug"
fi

export DEBUG_OPTION

set_mysql_flavour

get_pkg_name

./configure --prefix=/usr --sysconfdir=/etc --localstatedir=/var --with-database="$MYSQL_FLAVOUR" "$DEBUG_OPTION"
sed "s/#define COMMIT_HASH.*/#define COMMIT_HASH \"-$( git rev-parse --short HEAD )\"/" piler-config.h
make clean
make -j2

./unit_tests/run.sh

echo "Running GUI unit tests"

pushd webui
phpunit
popd

make install DESTDIR="$INSTALL_DIR"
chmod +x "${INSTALL_DIR}/etc/init.d/rc.piler" "${INSTALL_DIR}/etc/init.d/rc.searchd"

if [[ -f contrib/webserver/piler-nginx.conf ]]; then
   cp contrib/webserver/piler-nginx.conf "${INSTALL_DIR}/etc/piler/piler-nginx.conf.dist"
fi

make_deb_package "piler" "piler email archiver"
