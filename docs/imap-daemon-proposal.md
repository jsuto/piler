# IMAP Daemon for Piler - Proposal

## Motivation

Users who keep only recent mail (3-6 months) on their mail server rely on piler for older messages. Currently, accessing archived mail requires using the web UI. An IMAP daemon would allow direct access from mail clients (Thunderbird, Outlook, Apple Mail, etc.), providing tighter integration with the user's workflow.

## Proposed Approach

### Single INBOX Design

Rather than synthesizing artificial folder structures, all archived emails would be served from a single INBOX folder. This approach:

- Matches piler's actual data model (flat archive, no folders)
- Simplifies implementation significantly
- Lets users organize with their mail client's search, filters, and virtual folders
- Avoids confusion about which folder a message "belongs" to

### Architecture

```
┌─────────────┐      ┌──────────────┐      ┌─────────┐
│ Mail Client │─────▶│ piler-imapd  │─────▶│  MySQL  │
└─────────────┘      └──────┬───────┘      └─────────┘
                            │
                    FETCH   │
                    BODY[]  ▼
                     ┌─────────────┐
                     │  pilerget   │
                     └─────────────┘
```

The daemon would:
1. Accept IMAP connections from mail clients
2. Authenticate users via piler's existing auth system
3. Query MySQL for message metadata (listings, envelopes)
4. Retrieve message bodies by calling `pilerget` binary
5. Enforce piler's access control (users only see emails they have permission to view)

### Data Mapping

| IMAP Concept | Piler Source |
|--------------|--------------|
| UID | `metadata.id` |
| FLAGS | `\Seen` (always set - archived mail) |
| INTERNALDATE | `metadata.arrived` |
| RFC822.SIZE | `metadata.size` |
| ENVELOPE | `metadata.from`, `rcpt.to`, `metadata.subject`, `metadata.sent`, `metadata.message_id` |
| BODY[] | Retrieved via `pilerget {piler_id}` |

### Supported IMAP Commands

Minimal command set covering read-only mail access:

| Command | Behavior |
|---------|----------|
| CAPABILITY | Returns `IMAP4rev1` |
| LOGIN | Authenticates via piler's auth (LDAP, IMAP proxy, or local DB) |
| SELECT INBOX | Returns message count, selects the archive |
| EXAMINE INBOX | Same as SELECT (read-only anyway) |
| FETCH | Returns envelope from DB, body via pilerget |
| UID FETCH | Same as FETCH using UIDs |
| SEARCH | Date/from/to/subject queries against MySQL |
| UID SEARCH | Same as SEARCH using UIDs |
| NOOP | No operation (keepalive) |
| LOGOUT | Closes connection |

Unsupported commands (STORE, COPY, APPEND, DELETE, etc.) would return appropriate errors. The archive is read-only.

### Language Choice: Go

Recommended implementation language is Go because:

- Mature IMAP server library: `github.com/emersion/go-imap`
- Handles IMAP protocol complexity (parsing, state machine, concurrency)
- Single binary deployment, no runtime dependencies
- Good MySQL drivers available
- Can shell out to `pilerget` for message retrieval

Alternative: Could be written in C to match the codebase, but IMAP server implementation from scratch would be significantly more work.

## Limitations

### Read-Only Archive

- Clients cannot modify flags, move, or delete messages
- Flag changes are not persisted (reset on reconnect)
- Some clients may show warnings about read-only mailbox

### Large Archives

- Archives with 100k+ messages may have slow initial sync
- Modern clients handle this via lazy loading (envelopes first, bodies on-demand)
- SEARCH by date range helps clients limit what they fetch

### No Full-Text Search

- IMAP SEARCH would query MySQL columns (from, to, subject, date)
- Piler's Manticore full-text search is not exposed via IMAP
- Users would use web UI for advanced searches

### No Push Notifications

- IMAP IDLE not implemented in initial version
- Clients poll for new messages (acceptable for archive use case)

## Implementation Estimate

| Component | Effort |
|-----------|--------|
| IMAP protocol handling (using go-imap) | 2-3 days |
| MySQL backend integration | 2-3 days |
| Authentication integration | 1-2 days |
| Access control (email permissions) | 1-2 days |
| Testing with various clients | 2-3 days |
| Documentation and packaging | 1-2 days |
| **Total** | **~2-3 weeks** |

## Deployment Model

- Separate daemon: `piler-imapd`
- Listens on port 143 (IMAP) or 993 (IMAPS)
- Configuration in `/usr/local/etc/piler/piler-imapd.conf`
- Systemd service: `piler-imapd.service`
- Optional component (not required for core piler functionality)

## Configuration Options

```ini
[server]
listen = 0.0.0.0:993
tls_cert = /etc/piler/server.crt
tls_key = /etc/piler/server.key

[database]
host = localhost
port = 3306
user = piler
password = ***
database = piler

[piler]
pilerget_path = /usr/local/bin/pilerget
config_path = /usr/local/etc/piler/piler.conf
```

## Open Questions

1. **TLS requirement**: Should plaintext IMAP (port 143) be supported, or require TLS only?

2. **Authentication**: Should it support the same auth backends as the web UI (LDAP, IMAP proxy, POP3 proxy, local DB)?

3. **Concurrent connections**: What's a reasonable limit per user?

4. **Message flags**: Should we add a table to persist flags (\Seen, \Flagged) per user, or keep it purely stateless?

5. **Search scope**: Should SEARCH return all messages the user can access, or should there be a configurable limit (e.g., last N years)?

## Alternatives Considered

### Dovecot Frontend

Use Dovecot as the IMAP server with a custom backend script that queries piler. Pros: battle-tested IMAP implementation. Cons: complex setup, another moving part.

### JMAP Instead of IMAP

Modern protocol, simpler than IMAP. Cons: limited client support (no Thunderbird/Outlook).

### Enhanced Web UI

Add features like "open in mail client" or browser extension. Cons: doesn't solve the tight integration use case.

## Next Steps

1. Gather user feedback on this proposal
2. Decide on open questions above
3. Create proof-of-concept with basic LOGIN/SELECT/FETCH
4. Test with target mail clients (Thunderbird, Outlook, Apple Mail)
5. Iterate based on findings
