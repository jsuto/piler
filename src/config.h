/*
 * config.h, SJ
 */

#ifndef _CONFIG_H
 #define _CONFIG_H

#include <syslog.h>
#include "piler-config.h"
#include "params.h"

typedef unsigned long long uint64;

#define HOSTID "archive.example.com"

#define CONFIG_FILE CONFDIR "/piler/piler.conf"
#define SMTP_ACL_FILE CONFDIR "/piler/smtp.acl"
#define WORK_DIR DATADIR "/piler/tmp"
#define QUEUE_DIR DATADIR "/piler/store"
#define ERROR_DIR DATADIR "/piler/error"

#define PIDFILE "/var/run/piler/piler.pid"
#define QUARANTINELEN 255
#define TIMEOUT 60
#define TIMEOUT_USEC 500000
#define SESSION_TIMEOUT 420
#define MAXBUFSIZE 8192
#define SMALLBUFSIZE 512
#define BIGBUFSIZE 131072
#define REALLYBIGBUFSIZE 524288
#define SMTPBUFSIZE 2048000
#define TINYBUFSIZE 128
#define MAXVAL 256
#define RANDOM_POOL "/dev/urandom"
#define RND_STR_LEN 36
#define BUFLEN 32
#define QUEUE_ID_LEN 16
#define IPLEN 16+1
#define KEYLEN 56
#define MIN_EMAIL_ADDRESS_LEN 9
// Sphinx 3.x has an issue with tokens longer than 41 characters.
//
// When a regular user executes a query, then his default email address filter
// causes the query to fail with the below error message, even when the query
// itself is correct:
//
// SELECT id FROM main1,dailydelta1,delta1 WHERE MATCH(' ( (@sender thisisanextremelylongemailaddressyesareallylongoneyeahitolyouXaddressXcom ) | (@rcpt thisisanextremelylongemailaddressyesareallylongoneyeahitolyouXaddressXcom) ) ') ORDER BY `sent` DESC LIMIT 0,20 OPTION max_matches=1000'
//
// ERROR 1064 (42000): index dailydelta1,delta1,main1: syntax error, unexpected $end near ' '
//
// Note that we use 42, because the parser adds a trailing space to the tokens
// See https://www.mailpiler.org/wiki/current:sphinx3 and
// https://bitbucket.org/jsuto/piler/issues/1082/no-sphinx-results-with-long-email for more
#define MAX_EMAIL_ADDRESS_SPHINX_LEN 42

#define CRLF "\n"


#define MEMCACHED_CLAPF_PREFIX "_piler:"
#define MAX_MEMCACHED_KEY_LEN 250

#define MEMCACHED_SUCCESS 0
#define MEMCACHED_FAILURE 1

#define MEMCACHED_COUNTERS_LAST_UPDATE MEMCACHED_CLAPF_PREFIX "counters_last_update"
#define MEMCACHED_MSGS_RCVD MEMCACHED_CLAPF_PREFIX "rcvd"
#define MEMCACHED_MSGS_VIRUS MEMCACHED_CLAPF_PREFIX "virus"
#define MEMCACHED_MSGS_DUPLICATE MEMCACHED_CLAPF_PREFIX "duplicate"
#define MEMCACHED_MSGS_IGNORE MEMCACHED_CLAPF_PREFIX "ignore"
#define MEMCACHED_MSGS_SIZE MEMCACHED_CLAPF_PREFIX "size"
#define MEMCACHED_MSGS_STORED_SIZE MEMCACHED_CLAPF_PREFIX "stored_size"


#define PILEREXPORT_BEGIN_MARK "x-exported-by-pilerexport: start\n"

#define LOG_PRIORITY LOG_INFO

#define _LOG_INFO 3
#define _LOG_DEBUG 5
#define _LOG_EXTREME 100

#define MAX_RCPT_TO 128

#define MIN_WORD_LEN 3
#define MAX_WORD_LEN 25
#define MAX_TOKEN_LEN 4*MAX_WORD_LEN
#define DELIMITER ' '
#define BOUNDARY_LEN 255
#define MAX_ATTACHMENTS 16
#define MAX_ZIP_RECURSION_LEVEL 2

/* SQL stuff */

#define SQL_SPHINX_TABLE "sph_index"
#define SQL_METADATA_TABLE "metadata"
#define SQL_ATTACHMENT_TABLE "attachment"
#define SQL_FOLDER_TABLE "folder"
#define SQL_RECIPIENT_TABLE "rcpt"
#define SQL_ARCHIVING_RULE_TABLE "archiving_rule"
#define SQL_RETENTION_RULE_TABLE "retention_rule"
#define SQL_FOLDER_RULE_TABLE "folder_rule"
#define SQL_COUNTER_TABLE "counter"
#define SQL_OPTION_TABLE "option"
#define SQL_DOMAIN_TABLE "domain"
#define SQL_CUSTOMER_TABLE "customer"
#define SQL_IMPORT_TABLE "import"
#define SQL_LEGAL_HOLD_TABLE "legal_hold"
#define SQL_FOLDER_MESSAGE_TABLE "folder_message"
#define SQL_MESSAGES_VIEW "v_messages"
#define SQL_ATTACHMENTS_VIEW "v_attachment"

#define SQL_PREPARED_STMT_GET_DOMAINS                "SELECT `domain` FROM `" SQL_DOMAIN_TABLE "`"
#define SQL_PREPARED_STMT_GET_META_ID_BY_MESSAGE_ID  "SELECT id, piler_id FROM " SQL_METADATA_TABLE " WHERE message_id=?"
#define SQL_PREPARED_STMT_INSERT_INTO_RCPT_TABLE     "INSERT INTO " SQL_RECIPIENT_TABLE " (`id`,`to`,`todomain`) VALUES(?,?,?)"
#define SQL_PREPARED_STMT_INSERT_INTO_SPHINX_TABLE   "INSERT INTO " SQL_SPHINX_TABLE " (`id`, `from`, `to`, `fromdomain`, `todomain`, `subject`, `body`, `arrived`, `sent`, `size`, `direction`, `folder`, `attachments`, `attachment_types`) values(?,?,?,?,?,?,?,?,?,?,?,?,?,?)"
#define SQL_PREPARED_STMT_INSERT_INTO_META_TABLE     "INSERT INTO " SQL_METADATA_TABLE " (`from`,`fromdomain`,`subject`,`spam`,`arrived`,`sent`,`retained`,`size`,`hlen`,`direction`,`attachments`,`piler_id`,`message_id`,`reference`,`digest`,`bodydigest`,`vcode`) VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"
#define SQL_PREPARED_STMT_INSERT_INTO_ATTACHMENT_TABLE     "INSERT INTO " SQL_ATTACHMENT_TABLE " (`piler_id`,`attachment_id`,`sig`,`name`,`type`,`size`,`ptr`) VALUES(?,?,?,?,?,?,?)"
#define SQL_PREPARED_STMT_GET_ATTACHMENT_ID_BY_SIGNATURE   "SELECT `id` FROM `" SQL_ATTACHMENT_TABLE "` WHERE `sig`=? AND `ptr`=0 AND `size`=?"
#define SQL_PREPARED_STMT_GET_ATTACHMENT_POINTER     "SELECT `piler_id`, `attachment_id` FROM " SQL_ATTACHMENT_TABLE " WHERE id=?"
#define SQL_PREPARED_STMT_QUERY_ATTACHMENT           "SELECT `attachment_id`, `ptr` FROM " SQL_ATTACHMENT_TABLE " WHERE piler_id=? ORDER BY attachment_id ASC"
#define SQL_PREPARED_STMT_GET_FOLDER_ID              "SELECT `id` FROM " SQL_FOLDER_TABLE " WHERE `name`=? AND `parent_id`=?"
#define SQL_PREPARED_STMT_INSERT_INTO_FOLDER_TABLE   "INSERT INTO `" SQL_FOLDER_TABLE "` (`name`, `parent_id`) VALUES(?,?)"
#define SQL_PREPARED_STMT_UPDATE_METADATA_REFERENCE  "UPDATE " SQL_METADATA_TABLE " SET reference=? WHERE message_id=? AND reference=''"
#define SQL_PREPARED_STMT_GET_METADATA_REFERENCE     "SELECT COUNT(*) AS count FROM " SQL_METADATA_TABLE " WHERE reference=?"
#define SQL_PREPARED_STMT_GET_GUI_IMPORT_JOBS        "SELECT id, type, username, password, server FROM " SQL_IMPORT_TABLE " WHERE started=0 ORDER BY id LIMIT 0,1"
#define SQL_PREPARED_STMT_INSERT_FOLDER_MESSAGE      "INSERT INTO " SQL_FOLDER_MESSAGE_TABLE " (`folder_id`, `id`) VALUES(?,?)"
#define SQL_PREPARED_STMT_UPDATE_IMPORT_TABLE        "UPDATE " SQL_IMPORT_TABLE " SET started=?, status=?, imported=? WHERE id=?"

/* Error codes */

#define OK 0
#define ERR 1
#define ERR_EXISTS 2
#define ERR_DISCARDED 3
#define ERR_MYDOMAINS 4
#define ERR_FOLDER -1

#define AVIR_OK 0
#define AVIR_VIRUS 1


#define DIRECTION_INCOMING 0
#define DIRECTION_INTERNAL 1
#define DIRECTION_OUTGOING 2
#define DIRECTION_INTERNAL_AND_OUTGOING 3

#define WRITE_TO_STDOUT 0
#define WRITE_TO_BUFFER 1

#define S_STATUS_UNDEF "undef"
#define S_STATUS_STORED "stored"
#define S_STATUS_DUPLICATE "duplicate"
#define S_STATUS_DISCARDED "discarded"
#define S_STATUS_ERROR "error"

#endif /* _CONFIG_H */
