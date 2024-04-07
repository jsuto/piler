#!/bin/bash

set -o nounset
set -o errexit
set -o pipefail
set -x

SCRIPT_PATH="$(readlink -f "$0")"
SCRIPT_DIR="${SCRIPT_PATH%/*}"
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
   a="$(git log -n 1 --oneline)"
   COMMIT_ID="${a%% *}"
   VERSION="$(head -1 VERSION)"
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

   #write_package_txt
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

make_tarball() {
   local version

   read -r version < VERSION

   TARBALL="${PACKAGE_OUTDIR}/${PROJECT_ID}-${version}.tar.gz"

   git archive --prefix="${PROJECT_ID}-${version}/" HEAD | gzip -c > "$TARBALL"
   sha256sum "$TARBALL"

   printf "%s" "$TARBALL" > "${PACKAGE_OUTDIR}/package-${PROJECT_ID}-tarball.txt"
}


git config --global --add safe.directory /repo

pushd "$REPO_ROOT"

###[[ "$DISTRO" == "centos7" || "$DISTRO" == "xenial" ]] && sed -i -e 's/\-Wimplicit\-fallthrough=2//g' "/tmp/${PROJECT_ID}/configure"

if [[ "$DEBUG" == "true" ]]; then
   DEBUG_OPTION="--enable-debug"
fi

export DEBUG_OPTION

set_mysql_flavour

get_pkg_name

make_tarball

./configure --prefix=/usr --sysconfdir=/etc --localstatedir=/var --with-database="$MYSQL_FLAVOUR" "$DEBUG_OPTION"
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
