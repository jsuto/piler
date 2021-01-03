FROM ubuntu:20.04

ARG PACKAGE

LABEL description="piler ubuntu focal image" \
      maintainer="Janos SUTO, sj@acts.hu" \
      package="${PACKAGE}"

ENV DEBIAN_FRONTEND="noninteractive" \
    DISTRO="focal" \
    DOWNLOAD_URL="https://download.mailpiler.com" \
    PILER_USER="piler" \
    MYSQL_DATABASE="piler" \
    SPHINX_BIN_TARGZ="sphinx-3.3.1-bin.tar.gz"

COPY ${PACKAGE} /

RUN apt-get update && \
    apt-get -y --no-install-recommends install \
       wget rsyslog openssl sysstat php7.4-cli php7.4-cgi php7.4-mysql php7.4-fpm php7.4-zip php7.4-ldap \
       php7.4-gd php7.4-curl php7.4-xml php7.4-memcached catdoc unrtf poppler-utils nginx tnef sudo libzip5 \
       libtre5 cron libmariadb-dev mariadb-client-core-10.3 python3 python3-mysqldb && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* && \
    wget --no-check-certificate -q -O ${SPHINX_BIN_TARGZ} ${DOWNLOAD_URL}/generic-local/${SPHINX_BIN_TARGZ} && \
    tar zxvf ${SPHINX_BIN_TARGZ} && \
    sed -i 's/mail.[iwe].*//' /etc/rsyslog.conf && \
    sed -i '/session    required     pam_loginuid.so/c\#session    required     pam_loginuid.so' /etc/pam.d/cron && \
    dpkg -i ${PACKAGE} && \
    ln -sf /etc/piler/piler-nginx.conf /etc/nginx/sites-enabled && \
    rm -f ${PACKAGE} ${SPHINX_BIN_TARGZ} /etc/nginx/sites-enabled/default /etc/piler/piler.key /etc/piler/piler.pem /etc/piler/config-site.php && \
    crontab -u $PILER_USER /usr/share/piler/piler.cron

VOLUME ["/etc/piler"]
VOLUME ["/var/piler"]

EXPOSE 25 80 443

COPY start.sh /start.sh

CMD ["/start.sh"]
