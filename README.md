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
- recognised formats: EML, Maildir, mailbox

## Build a deb package for Ubuntu 24.04 LTS

Clone the repo, and the following command in the repo root:

```
      docker run --rm \
           -e PROJECT_ID=piler \
           -e DISTRO=noble \
           -e ARCH=amd64 \
           -e VERSION=1.4.8 \
           -e COMMIT_ID=deadbeef \
           -e BUILD_NUMBER=1234 \
           -v $PWD:/repo \
           -v $PWD/docker:/data \
           sutoj/builder:noble
```

This works for any branch, not just master.
