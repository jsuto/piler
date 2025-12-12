![test workflow](https://github.com/jsuto/piler/actions/workflows/test.yaml/badge.svg)

# piler email archiver

piler is an open source email archival application. Please visit https://www.mailpiler.org/ for more.

## Features

- built-in smtp server
- archival rules
- retention rules
- legal hold
- message and attachment deduplication
- message compression
- message encryption
- digital fingerprinting and verification
- full text search
- simple and expert search
- save search criteria
- tagging emails
- view, export, restore emails
- bulk import/export messages
- access control
- AD / LDAP authentication
- IMAP, POP3 authentication
- single sign-on (SSO)
- Google Apps support
- Office 365 support
- STARTTLS support
- Google Authenticator support for 2-factor authentication
- i18n
- customisable theme
- audit logs
- search in audit logs
- online status info
- accounting
- recognised formats: PST, EML, Maildir, mailbox

## Quick deb package install

### Install the prerequisites

```
apt-get update
apt-get -y --no-install-recommends install \
   wget openssl sysstat php-cli php-cgi php-mysql \
   php-fpm php-zip php-ldap php-gd php-curl \
   php-xml php-memcached catdoc unrtf poppler-utils \
   nginx tnef sudo libzip4t64 libtre5 cron \
   mariadb-server mariadb-client-core python3 python3-mysqldb \
   ca-certificates curl rsyslog gnupg

wget https://repo.manticoresearch.com/manticore-repo.noarch.deb
dpkg -i manticore-repo.noarch.deb
apt-get update && apt-get install -y manticore manticore-columnar-lib manticore-extra
rm -f manticore-repo.noarch.deb
```

### Deploy piler

```
dpkg -i piler_1.4.8-noble-e885d9b_arm64.deb
```

Run the postinstall script to bootstrap the database, create crontab entries, etc.

```
/usr/libexec/piler/postinstall.sh
```
