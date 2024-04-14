FROM ubuntu:22.04

ARG PACKAGE
ARG TARGETARCH

LABEL description="piler ubuntu jammy image" \
      maintainer="Janos SUTO, sj@acts.hu" \
      package="${PACKAGE}"

ENV DEBIAN_FRONTEND="noninteractive" \
    DISTRO="jammy" \
    PILER_USER="piler" \
    MYSQL_DATABASE="piler"

COPY ${PACKAGE}_${TARGETARCH}.deb /

# hadolint ignore=DL3008,DL3015
RUN apt-get update && \
    apt-get -y --no-install-recommends install \
       wget openssl sysstat php8.1-cli php8.1-cgi php8.1-mysql php8.1-fpm php8.1-zip php8.1-ldap \
       php8.1-gd php8.1-curl php8.1-xml php8.1-memcached catdoc unrtf poppler-utils nginx tnef sudo libzip4 \
       libtre5 cron libmariadb-dev mariadb-client-core-10.6 python3 python3-mysqldb ca-certificates curl rsyslog && \
    wget https://repo.manticoresearch.com/manticore-repo.noarch.deb && \
    dpkg -i manticore-repo.noarch.deb && \
    rm -f manticore-repo.noarch.deb && \
    apt-get update && \
    apt-get install -y manticore manticore-columnar-lib && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* && \
    sed -i '/session    required     pam_loginuid.so/c\#session    required     pam_loginuid.so' /etc/pam.d/cron && \
    dpkg -i ${PACKAGE}_${TARGETARCH}.deb && \
    touch /etc/piler/MANTICORE && \
    ln -sf /etc/piler/piler-nginx.conf /etc/nginx/sites-enabled && \
    rm -f ${PACKAGE}_${TARGETARCH}.deb /etc/nginx/sites-enabled/default /etc/piler/piler.key /etc/piler/piler.pem /etc/piler/config-site.php && \
    crontab -u $PILER_USER /usr/share/piler/piler.cron

VOLUME ["/etc/piler"]
VOLUME ["/var/piler/store"]
VOLUME ["/var/piler/manticore"]

EXPOSE 25 80 443

COPY start.sh /start.sh

CMD ["/start.sh"]
