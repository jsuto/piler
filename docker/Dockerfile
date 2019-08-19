FROM ubuntu:18.04

ARG PACKAGE

LABEL description="piler container" \
      maintainer="Janos SUTO, sj@acts.hu"
      package="${PACKAGE}"

ENV DEBIAN_FRONTEND="noninteractive" \
    DISTRO="bionic" \
    DOWNLOAD_URL="https://download.mailpiler.com" \
    PILER_USER="piler" \
    MYSQL_HOSTNAME="localhost" \
    MYSQL_DATABASE="piler" \
    MYSQL_PILER_PASSWORD="piler123" \
    MYSQL_ROOT_PASSWORD="abcde123" \
    SPHINX_BIN_TARGZ="sphinx-3.1.1-bin.tar.gz"

ADD "https://bitbucket.org/jsuto/piler/downloads/${PACKAGE}" "/${PACKAGE}"
ADD start.sh /start.sh

RUN apt-get update && \
    apt-get -y --no-install-recommends install \
       wget rsyslog openssl sysstat php7.2-cli php7.2-cgi php7.2-mysql php7.2-fpm php7.2-zip php7.2-ldap \
       php7.2-gd php7.2-curl php7.2-xml catdoc unrtf poppler-utils nginx tnef sudo libodbc1 libpq5 libzip4 \
       libtre5 libwrap0 cron libmariadb3 libmysqlclient-dev python python-mysqldb mariadb-server && \
    service mysql start && mysqladmin -u root password ${MYSQL_ROOT_PASSWORD} && \
    wget --no-check-certificate -q -O ${SPHINX_BIN_TARGZ} ${DOWNLOAD_URL}/generic-local/${SPHINX_BIN_TARGZ} && \
    tar zxvf ${SPHINX_BIN_TARGZ} && \
    rm -f ${SPHINX_BIN_TARGZ} && \
    sed -i 's/mail.[iwe].*//' /etc/rsyslog.conf && \
    sed -i '/session    required     pam_loginuid.so/c\#session    required     pam_loginuid.so' /etc/pam.d/cron && \
    mkdir /etc/piler && \
    printf "[mysql]\nuser = piler\npassword = ${MYSQL_PILER_PASSWORD}\n" > /etc/piler/.my.cnf && \
    printf "[mysql]\nuser = root\npassword = ${MYSQL_ROOT_PASSWORD}\n" > /root/.my.cnf && \
    echo "alias mysql='mysql --defaults-file=/etc/piler/.my.cnf'" > /root/.bashrc && \
    echo "alias t='tail -f /var/log/syslog'" >> /root/.bashrc && \
    dpkg -i $PACKAGE && \
    crontab -u $PILER_USER /usr/share/piler/piler.cron && \
    touch /var/log/mail.log && \
    rm -f $PACKAGE /etc/nginx/sites-enabled/default && \
    sed -i 's/#ngram/ngram/g' /etc/piler/sphinx.conf.dist && \
    sed -i 's/220/311/g' /etc/piler/sphinx.conf.dist

EXPOSE 25 80 443
VOLUME ["/var/piler"]
CMD ["/start.sh"]
