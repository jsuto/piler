# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Piler is an open-source email archival application with a C daemon core, PHP web UI, and Manticore search engine for full-text search. Key features include SMTP ingestion, MIME parsing, message deduplication, compression, encryption, and multi-source import (IMAP, POP3, Maildir, Mailbox).

## Build Commands

```bash
# Configure and build (requires MySQL/MariaDB, OpenSSL, libzip, zlib, libcurl, TRE)
./configure --prefix=/usr/local --with-piler-user=piler
make
sudo make install
sudo make postinstall      # Sets up directories, permissions, DB schema

# Generate encryption key
make key

# Clean build artifacts
make clean
make distclean            # Full clean including generated Makefiles

# Build deb package (Ubuntu 24.04)
docker run --rm \
    -e PROJECT_ID=piler -e DISTRO=noble -e ARCH=amd64 \
    -e VERSION=1.4.8 -e COMMIT_ID=deadbeef -e BUILD_NUMBER=1234 \
    -v $PWD:/repo -v $PWD/docker:/data \
    sutoj/builder:noble
```

## Testing

### C Unit Tests
```bash
cd unit_tests
make                     # Build all unit tests
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

### PHP Unit Tests (Web UI)
```bash
cd webui
phpunit                  # Run all PHP tests
phpunit tests/EmailTest.php  # Run single test file
```

### Integration Tests (Docker-based)
```bash
# Runs in CI via .github/workflows/test.yaml
# Uses tests/run.sh inside Docker container
```

## Architecture

### Core Daemon (src/)
- **piler.c** - Main SMTP receiving daemon entry point
- **piler-smtp.c** - Standalone SMTP server component
- **parser.c / parser_utils.c** - MIME message parsing and decoding
- **message.c** - Message handling and storage operations
- **archive.c** - Message archival logic with compression/encryption
- **rules.c** - Archival and retention rules engine
- **smtp.c / session.c** - SMTP protocol and session handling
- **decoder.c** - Content decoding (base64, quoted-printable, etc.)
- **imap.c / pop3.c** - Protocol clients for message import
- **import*.c** - Import handlers (IMAP, Maildir, Mailbox, POP3, PilerExport)

### Library Structure
All core modules compile into `libpiler.a` (static) and `libpiler.so` (shared), linked by CLI tools.

### CLI Tools
- **pilerget** - Retrieve stored messages
- **pileraget** - Retrieve attachments
- **pilerimport** - Import messages from various sources
- **pilerexport** - Export archived messages
- **pilerconf** - Configuration utility
- **reindex** - Rebuild search indices
- **pilertest** - Testing utility
- **pilerstats** - Generate statistics

### Web UI (webui/)
PHP MVC application with Zend Framework components. Entry point is `index.php`, configuration via `config.php`.

### Message Flow
1. SMTP server receives messages (piler-smtp or piler daemon)
2. MIME parser tokenizes and decodes content
3. Digest calculated for deduplication
4. Message stored to filesystem with optional compression/encryption
5. Manticore search engine indexes for full-text search
6. Web UI and CLI tools access via database queries

### Key Directories
- `/var/piler/store/` - Message storage (encrypted/compressed)
- `/var/piler/manticore/` - Search index data
- `/usr/libexec/piler/` - Helper scripts (indexer.*.sh, import.sh, purge.sh)
- `/usr/local/etc/piler/` - Configuration files

## Configure Options

```bash
--enable-static-build    # Build statically linked executables
--enable-compat-layout   # Support older storage layout
--enable-memcached       # Build with memcached support
--with-piler-user=USER   # Daemon user (default: piler)
```

## Dependencies

Required: MySQL/MariaDB, OpenSSL, libzip, zlib, libcurl, TRE (regex)

Optional (for document text extraction): pdftotext, catdoc, catppt, ppthtml, xls2csv, unrtf, tnef
