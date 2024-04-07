FROM ubuntu:22.04

LABEL description="jammy builder container"
LABEL maintainer="Janos SUTO, sj@acts.hu"

ENV DEBIAN_FRONTEND="noninteractive"

SHELL ["/bin/bash", "-c", "-o", "pipefail"]

# hadolint ignore=DL3001,DL3008,DL3028
RUN apt-get update && \
    apt-get install --no-install-recommends -y gnupg git jq curl unzip openssl openssh-client make gcc sysstat catdoc unrtf poppler-utils tnef libwrap0-dev libssl-dev libtre-dev libzip-dev libmariadb-dev libcurl4-openssl-dev ruby ruby-dev mariadb-server openjdk-8-jre phpunit \
    php8.1-cli php8.1-cgi php8.1-mysql php8.1-fpm php8.1-zip php8.1-ldap php8.1-gd php8.1-curl php8.1-xml php8.1-memcached php8.1-mbstring php8.1-opcache && \
    apt-get clean && rm -rf /var/lib/apt/lists/* && \
    groupadd piler && useradd -g piler -s /bin/bash -d /var/piler piler && usermod -L piler && \
    mkdir -p /var/piler && \
    service mariadb start && \
    echo "create database piler1 character set utf8mb4" | mysql && \
    echo "grant all privileges on piler1.* to piler@localhost identified by 'piler123'" | mysql && \
    gem install fpm && \
    curl -L -o /usr/local/bin/mc https://dl.min.io/client/mc/release/linux-amd64/mc && \
    chmod +x /usr/local/bin/mc && \
    curl -L -o /tmp/composer.phar https://getcomposer.org/composer-stable.phar

COPY run.sh /run.sh

CMD ["/run.sh"]
