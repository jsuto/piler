/*
 * config.h, SJ
 */

#ifndef _CONFIG_H
 #define _CONFIG_H

#include <syslog.h>
#include "piler-config.h"
#include "params.h"

#define PROGNAME "piler"

#define VERSION "0.1.7"

#define PROGINFO VERSION ", Janos SUTO <sj@acts.hu>\n\n" CONFIGURE_PARAMS "\n\nSend bugs/issues to https://jira.acts.hu:8443/\n"

#define HOSTID "mailarchiver"

#define CONFIG_FILE CONFDIR "/piler.conf"
#define WORK_DIR DATADIR "/spool/piler/tmp"
#define QUEUE_DIR DATADIR "/piler/store"
#define DEFER_DIR DATADIR "/spool/piler/deferred"

#define CLAMD_SOCKET "/tmp/clamd"

#define PIDFILE "/var/run/piler/piler.pid"
#define QUARANTINELEN 255
#define TIMEOUT 60
#define TIMEOUT_USEC 500000
#define SESSION_TIMEOUT 420
#define MAXBUFSIZE 8192
#define SMALLBUFSIZE 512
#define BIGBUFSIZE 65535
#define TINYBUFSIZE 128
#define MAXVAL 256
#define RANDOM_POOL "/dev/urandom"
#define RND_STR_LEN 36
#define BUFLEN 32
#define IPLEN 16+1
#define KEYLEN 56

#define CRLF "\n"


#define MEMCACHED_CLAPF_PREFIX "_piler"
#define MAX_MEMCACHED_KEY_LEN 250

#define MEMCACHED_SUCCESS 0
#define MEMCACHED_FAILURE 1

#define MEMCACHED_COUNTERS_LAST_UPDATE MEMCACHED_CLAPF_PREFIX ":counters_last_update"
#define MEMCACHED_MSGS_RCVD MEMCACHED_CLAPF_PREFIX ":rcvd"
#define MEMCACHED_MSGS_VIRUS MEMCACHED_CLAPF_PREFIX ":virus"
#define MEMCACHED_MSGS_DUPLICATE MEMCACHED_CLAPF_PREFIX ":duplicate"


#define LOG_PRIORITY LOG_INFO

#define _LOG_INFO 3
#define _LOG_DEBUG 5

#define MAX_RCPT_TO 128

#define MIN_WORD_LEN 3
#define MAX_WORD_LEN 25
#define MAX_TOKEN_LEN 4*MAX_WORD_LEN
#define DELIMITER ' '
#define BOUNDARY_LEN 255
#define MAX_ATTACHMENTS 16

/* SQL stuff */

#define SQL_SPHINX_TABLE "sph_index"
#define SQL_METADATA_TABLE "metadata"
#define SQL_ATTACHMENT_TABLE "attachment"
#define SQL_ARCHIVING_RULE_TABLE "archiving_rule"
#define SQL_COUNTER_TABLE "counter"


/* TRE stuff */

#define NUM_OF_REGEXES 20


/* Error codes */

#define OK 0
#define ERR 1
#define ERR_EXISTS 2

#define AVIR_OK 0
#define AVIR_VIRUS 1



#endif /* _CONFIG_H */

