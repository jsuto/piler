# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Piler is an open-source email archival application (https://www.mailpiler.org/) with a C daemon core, PHP web UI, and Manticore search engine for full-text search. Key features include SMTP ingestion, MIME parsing, message deduplication, compression, encryption, and multi-source import (IMAP, POP3, Maildir, Mailbox).

## Build Commands

```bash
# Configure and build (requires MySQL/MariaDB, OpenSSL, libzip, zlib, libcurl, TRE)
./configure --prefix=/usr/local --with-piler-user=piler
make                         # Builds src/, unit_tests/, webui/, and all subdirs
sudo make install
sudo make postinstall        # Runs util/postinstall.sh: sets up dirs, permissions, DB schema
make config-php              # Generates webui/config.php from config.php.in

# Generate 56-byte encryption key
make key

# Clean build artifacts
make clean
make distclean               # Full clean including generated Makefiles

# Build deb package (Ubuntu 24.04)
docker run --rm \
    --security-opt apparmor=unconfined \
    -e PROJECT_ID=piler -e DISTRO=noble -e ARCH=amd64 \
    -e VERSION=1.4.8 -e COMMIT_ID=deadbeef -e BUILD_NUMBER=1234 \
    -v $PWD:/repo -v $PWD/docker:/data \
    sutoj/builder:noble
```

**Configure options:** `--enable-static-build`, `--enable-compat-layout` (older storage layout), `--enable-memcached`, `--enable-tweak-sent-time`, `--with-piler-user=USER`

**Dependencies:** MySQL/MariaDB, OpenSSL, libzip, zlib, libcurl, TRE (regex). Optional: pdftotext, catdoc, catppt, ppthtml, xls2csv, unrtf, tnef (document text extraction).

## Testing

### C Unit Tests
```bash
cd unit_tests
make                     # Build all unit tests (requires libpiler.a from src/)
./check_parser           # Run parser tests
./check_rules            # Run rules engine tests
./check_decoder          # Run decoder tests
./check_digest           # Run digest tests
./check_hash             # Run hash tests
./check_misc             # Run misc tests
./check_parser_utils     # Run parser utility tests
./check_mydomains        # Run domain tests
./check_attachments      # Run attachment tests
./smtp                   # Run SMTP tests
```

Uses a custom lightweight test framework (unit_tests/test.h) with `ASSERT()`, `TEST_HEADER()`, `TEST_FOOTER()` macros — not an external library. Tests link against `../src/libpiler.a` and use .eml fixture files in unit_tests/.

### PHP Unit Tests (Web UI)
```bash
cd webui
phpunit                           # Run all PHP tests
phpunit tests/EmailTest.php       # Run single test file
```

Test files: EmailTest, FormatTest, MiscTest, ParseMessageTest, SplitMessageTest. Config in `webui/phpunit.xml`.

### Integration Tests (Docker-based)
CI runs `tests/run.sh` inside Docker: sends ~3020 test emails, validates status counts, runs Manticore search queries, verifies SHA256 digests of retrieved vs. original messages, tests purge and import.

## Architecture

### Message Flow
1. SMTP server receives messages (piler-smtp or piler daemon)
2. MIME parser tokenizes and decodes content
3. Digest calculated for deduplication
4. Message stored to filesystem with optional compression/encryption
5. Manticore search engine indexes for full-text search
6. Web UI and CLI tools access via MySQL database queries + Manticore for search

### Core Daemon (src/)
All modules compile into `libpiler.a` (static) and `libpiler.so` (shared), linked by CLI tools.

- **piler.c / piler-smtp.c** — Main SMTP receiving daemon / standalone SMTP server
- **parser.c / parser_utils.c** — MIME message parsing and decoding
- **message.c / archive.c** — Message handling, storage, archival (compression/encryption)
- **rules.c** — Archival and retention rules engine (uses TRE regex)
- **smtp.c / session.c** — SMTP protocol and session handling
- **decoder.c** — Content decoding (base64, quoted-printable, etc.)
- **imap.c / pop3.c** — Protocol clients for message import
- **import*.c** — Import handlers (IMAP, Maildir, Mailbox, POP3, PilerExport)

### Web UI (webui/)
Custom MVC PHP application (not a standard framework). Uses Registry pattern for DI. Dual database connections: MySQL (metadata) + Manticore (full-text search via SphinxQL). Configuration via `config.php` (generated) with `config-site.php` for local overrides.

### CLI Tools
pilerget, pileraget, pilerimport, pilerexport, pilerconf, reindex, pilertest, pilerstats.

### Database
Schema: `util/db-mysql.sql`. Migrations: `util/db-upgrade-*.sql`. Core tables: `metadata` (message data), `rcpt` (recipients), `attachment` (deduplicated attachments), `sph_index` (Manticore staging). Uses 20+ prepared statements defined in `src/config.h`.

## Code Conventions

- **C standard:** C99 (`-std=c99 -O2 -Wall -Wextra -g`)
- **Naming:** lowercase_with_underscores for functions and structs, UPPERCASE for constants
- **Return codes:** OK (0), ERR (1), ERR_EXISTS (2), ERR_DISCARDED (3), ERR_FOLDER (-1)
- **Error handling:** syslog-based logging with verbosity levels (1=normal, 3=info, 5=debug)
- **Memory:** prefers stack-allocated fixed buffers (MAXBUFSIZE=8192, SMALLBUFSIZE=512, BIGBUFSIZE=131072)
- **Key limit:** MAX_EMAIL_ADDRESS_SPHINX_LEN=42 (Manticore/Sphinx 3.x compatibility)
- **Encryption key:** 56 bytes (KEYLEN), generated from /dev/urandom

### Key Directories
- `/var/piler/store/` — Message storage (encrypted/compressed)
- `/var/piler/manticore/` — Search index data
- `util/` — Helper scripts (indexer, import, purge, postinstall), DB schema, PHP utilities
- `etc/` — Configuration templates (example.conf, manticore.conf.in, smtp.acl.example, cron.jobs.in)
- `contrib/` — Integration examples (nagios, o365, milter, selinux, nginx configs, etc.)
