FROM ubuntu:24.04

ARG PACKAGE
ARG TARGETARCH

LABEL description="piler ubuntu noble image" \
      maintainer="Janos SUTO, sj@acts.hu" \
      package="${PACKAGE}"

ENV DEBIAN_FRONTEND="noninteractive" \
    DISTRO="noble" \
    PILER_USER="piler" \
    MYSQL_DATABASE="piler"

COPY ${PACKAGE}_${TARGETARCH}.deb /

# hadolint ignore=DL3008
RUN apt-get update && \
    apt-get -y --no-install-recommends install \
       wget openssl sysstat php8.3-cli php8.3-cgi php8.3-mysql php8.3-fpm php8.3-zip php8.3-ldap \
       php8.3-gd php8.3-curl php8.3-xml php8.3-memcached catdoc unrtf poppler-utils nginx tnef sudo libzip4t64 \
       libtre5 cron libmariadb-dev mariadb-client-core python3 python3-mysqldb ca-certificates curl rsyslog gnupg && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* && \
    sed -i '/session    required     pam_loginuid.so/c\#session    required     pam_loginuid.so' /etc/pam.d/cron && \
    dpkg -i ${PACKAGE}_${TARGETARCH}.deb && \
    touch /etc/piler/MANTICORE && \
    ln -sf /etc/piler/piler-nginx.conf /etc/nginx/sites-enabled && \
    echo "www-data ALL=NOPASSWD: /etc/init.d/rc.piler reload" > /etc/sudoers.d/piler && \
    rm -f ${PACKAGE}_${TARGETARCH}.deb /etc/nginx/sites-enabled/default /etc/piler/piler.key /etc/piler/piler.pem /etc/piler/config-site.php && \
    cp -R /etc/piler /tmp/piler-conf && \
    crontab -u $PILER_USER /usr/share/piler/piler.cron

VOLUME ["/etc/piler"]
VOLUME ["/var/piler/store"]

EXPOSE 25 80 443

COPY start.sh /start.sh

CMD ["/start.sh"]
