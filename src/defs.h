/*
 * defs.h, SJ
 */

#ifndef _DEFS_H
   #define _DEFS_H

#ifdef NEED_MYSQL
  #include <mysql.h>
  #include <mysqld_error.h>
#endif
#ifdef NEED_PSQL
  #include <libpq-fe.h>
#endif
#ifdef HAVE_TRE
   #include <tre/tre.h>
   #include <tre/regex.h>
#endif
#ifdef HAVE_LIBWRAP
   #include <tcpd.h>
#endif

#include <openssl/sha.h>
#include <openssl/ssl.h>
#include "tai.h"
#include "config.h"

#define MSG_UNDEF -1
#define MSG_BODY 0
#define MSG_RECEIVED 1
#define MSG_FROM 2
#define MSG_TO 3
#define MSG_CC 4
#define MSG_SUBJECT 5
#define MSG_CONTENT_TYPE 6
#define MSG_CONTENT_TRANSFER_ENCODING 7
#define MSG_CONTENT_DISPOSITION 8
#define MSG_MESSAGE_ID 9
#define MSG_REFERENCES 10
#define MSG_RECIPIENT 11

#define MAXHASH 277

#define BASE64_RATIO 1.33333333

#define DIGEST_LENGTH SHA256_DIGEST_LENGTH

#define UNDEF 0
#define READY 1
#define BUSY 2

#define MAX_SQL_VARS 20

#define TYPE_UNDEF 0
#define TYPE_SHORT 1
#define TYPE_LONG 2
#define TYPE_LONGLONG 3
#define TYPE_STRING 4

#define MAXCHILDREN 64

#define RULE_UNDEF 0
#define RULE_MATCH 1
#define RULE_NO_MATCH -100


typedef void signal_func (int);


struct child {
   pid_t pid;
   int serial;
   int messages;
   int status;
};


struct attachment {
   int size;
   char type[TINYBUFSIZE];
   char shorttype[TINYBUFSIZE];
   char aname[TINYBUFSIZE];
   char filename[TINYBUFSIZE];
   char internalname[TINYBUFSIZE];
   char digest[2*DIGEST_LENGTH+1];
   char dumped;
};


struct ptr_array {
   uint64 ptr;
   char piler_id[RND_STR_LEN+2];
   int attachment_id;
};


struct node {
   void *str;
   unsigned int key;
   struct node *r;
};


struct rule {
#ifdef HAVE_TRE
   regex_t from;
   regex_t to;
   regex_t subject;
   regex_t body;
   regex_t attachment_name;
   regex_t attachment_type;
#endif
   int spam;
   int size;
   char _size[4];
   int attachment_size;
   char _attachment_size[4];

   char *domain;
   int domainlen;

   int days;

   char emptyfrom, emptyto, emptysubject, emptybody, emptyaname, emptyatype;

   char *rulestr;
   char compiled;

   struct rule *r;
};


struct _state {
   int line_num;
   int message_state;
   int is_header;
   int is_1st_header;
   int textplain;
   int texthtml;
   int message_rfc822;
   int base64;
   int utf8;
   int qp;
   int htmltag;
   int style;
   int skip_html;
   int has_to_dump;
   int fd;
   int b64fd;
   int mfd;
   int octetstream;
   int realbinary;
   int content_type_is_set;
   int pushed_pointer;
   int saved_size;
   int writebufpos;
   int abufpos;
   char attachedfile[RND_STR_LEN+SMALLBUFSIZE];
   char message_id[SMALLBUFSIZE];
   char message_id_hash[2*DIGEST_LENGTH+1];
   char miscbuf[MAX_TOKEN_LEN];
   char qpbuf[MAX_TOKEN_LEN];
   unsigned long n_token;
   unsigned long n_subject_token;
   unsigned long n_body_token;
   unsigned long n_chain_token;

   char filename[TINYBUFSIZE];
   char type[TINYBUFSIZE];
   char charset[TINYBUFSIZE];

   char attachment_name_buf[SMALLBUFSIZE];
   int anamepos;

   struct node *boundaries[MAXHASH];
   struct node *rcpt[MAXHASH];
   struct node *rcpt_domain[MAXHASH];
   struct node *journal_recipient[MAXHASH];

   int n_attachments;
   struct attachment attachments[MAX_ATTACHMENTS];

   char reference[SMALLBUFSIZE];

   char b_from[SMALLBUFSIZE], b_from_domain[SMALLBUFSIZE], b_to[MAXBUFSIZE], b_to_domain[SMALLBUFSIZE], b_subject[MAXBUFSIZE], b_body[BIGBUFSIZE];
   char b_journal_to[MAXBUFSIZE];

   int bodylen;
   int tolen;
   int journaltolen;

   int retention;
};


struct session_data {
   char filename[SMALLBUFSIZE];
   char ttmpfile[SMALLBUFSIZE], tmpframe[SMALLBUFSIZE], tre, restored_copy;
   char mailfrom[SMALLBUFSIZE], rcptto[MAX_RCPT_TO][SMALLBUFSIZE], client_addr[SMALLBUFSIZE];
   char fromemail[SMALLBUFSIZE];
   char acceptbuf[SMALLBUFSIZE];
   char attachments[SMALLBUFSIZE];
   char internal_sender, internal_recipient, external_recipient;
   uint64 duplicate_id;
   short int customer_id;
   int direction;
   int tls;
   int spam_message;
   int fd, hdr_len, tot_len, stored_len, num_of_rcpt_to, rav;
   int need_scan;
   float __acquire, __parsed, __av, __store, __compress, __encrypt;
   char bodydigest[2*DIGEST_LENGTH+1];
   char digest[2*DIGEST_LENGTH+1];
   time_t now, sent, delivered, retained;
   char ms_journal;
   char import;
   int journal_envelope_length, journal_bottom_length;
   unsigned int sql_errno;
#ifdef NEED_MYSQL
   MYSQL mysql;
#endif
#ifdef NEED_PSQL
   PGconn *psql;
   char conninfo[SMALLBUFSIZE];
#endif
};


#ifdef HAVE_MEMCACHED

#include <stdbool.h>
#include <netinet/in.h>

struct flags {
   bool no_block:1;
   bool no_reply:1;
   bool tcp_nodelay:1;
   bool tcp_keepalive:1;
};


struct memcached_server {

   struct flags flags;

   int fd;
   unsigned int snd_timeout;
   unsigned int rcv_timeout;

   int send_size;
   int recv_size;
   unsigned int tcp_keepidle;

   int last_read_bytes;

   char *result;
   char buf[MAXBUFSIZE];

   struct sockaddr_in addr;

   char server_ip[IPLEN];
   int server_port;

   char initialised;
};
#endif


struct import {
   char *extra_recipient;
   int status;
   int total_messages;
   int processed_messages;
   int batch_processing_limit;
   int start_position;
   int import_job_id;
   int remove_after_import;
   int download_only;
   int keep_eml;
   int timeout;
   long total_size;
   time_t started, updated, finished;
};


struct __data {
   int folder, quiet;
   char recursive_folder_names;
   char starttls[TINYBUFSIZE];
   struct node *mydomains[MAXHASH];
   struct node *imapfolders[MAXHASH];
   struct import *import;
   char *dedup;
   int child_serial;

#ifdef NEED_MYSQL
   MYSQL_STMT *stmt_generic;
   MYSQL_STMT *stmt_get_meta_id_by_message_id;
   MYSQL_STMT *stmt_insert_into_rcpt_table;
   MYSQL_STMT *stmt_insert_into_sphinx_table;
   MYSQL_STMT *stmt_insert_into_meta_table;
   MYSQL_STMT *stmt_insert_into_attachment_table;
   MYSQL_STMT *stmt_get_attachment_id_by_signature;
   MYSQL_STMT *stmt_get_attachment_pointer;
   MYSQL_STMT *stmt_query_attachment;
   MYSQL_STMT *stmt_get_folder_id;
   MYSQL_STMT *stmt_insert_into_folder_table;
   MYSQL_STMT *stmt_update_metadata_reference;
   MYSQL_STMT *stmt_select_from_meta_table;
   MYSQL_STMT *stmt_select_non_referenced_attachments;
#endif

   char *sql[MAX_SQL_VARS];
   int type[MAX_SQL_VARS];
   int len[MAX_SQL_VARS];
   unsigned long length[MAX_SQL_VARS];
   my_bool is_null[MAX_SQL_VARS];
   my_bool error[MAX_SQL_VARS];
   int pos;

#ifdef HAVE_TRE
   struct node *archiving_rules[1];
   struct node *retention_rules[1];
#endif

#ifdef HAVE_MEMCACHED
   struct memcached_server memc;
#endif

   SSL_CTX *ctx;
   SSL *ssl;
};


struct __counters {
   unsigned long long c_rcvd;
   unsigned long long c_virus;
   unsigned long long c_duplicate;
   unsigned long long c_ignore;
   unsigned long long c_size;
   unsigned long long c_stored_size;
};

#endif /* _DEFS_H */

