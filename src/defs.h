/*
 * defs.h, SJ
 */

#ifndef _DEFS_H
   #define _DEFS_H

#ifdef NEED_MYSQL
  #include <mysql.h>
  #include <mysqld_error.h>
#endif
#ifdef HAVE_TRE
   #include <tre/tre.h>
   #include <tre/regex.h>
#endif

#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/ssl.h>
#include <netinet/in.h>
#include "tai.h"
#include "config.h"

#define MSG_UNDEF -1
#define MSG_BODY 0
#define MSG_RECEIVED 1
#define MSG_FROM 2
#define MSG_SENDER 3
#define MSG_TO 4
#define MSG_CC 5
#define MSG_SUBJECT 6
#define MSG_CONTENT_TYPE 7
#define MSG_CONTENT_TRANSFER_ENCODING 8
#define MSG_CONTENT_DISPOSITION 9
#define MSG_MESSAGE_ID 10
#define MSG_REFERENCES 11
#define MSG_RECIPIENT 12
#define MSG_ENVELOPE_TO 13

#define MAXHASH 277

#define BASE64_RATIO 1.33333333

#define DIGEST_LENGTH EVP_MAX_MD_SIZE
#define DIGEST_HEX_LENGTH 2*DIGEST_LENGTH+1

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
   char filename[SMALLBUFSIZE];
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


struct smtp_acl {
   char network_str[BUFLEN];
   in_addr_t low, high;
   int prefix;
   int rejected;
   struct smtp_acl *r;
};


struct net {
   int socket;
   int use_ssl;
   int starttls;
   int timeout;
   SSL_CTX *ctx;
   SSL *ssl;
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
   int folder_id;

   char emptyfrom, emptyto, emptysubject, emptybody, emptyaname, emptyatype;

   char *rulestr;
   char compiled;

   struct rule *r;
};


struct rule_cond {
   char domain[SMALLBUFSIZE];
   char from[SMALLBUFSIZE];
   char to[SMALLBUFSIZE];
   char subject[SMALLBUFSIZE];
   char body[SMALLBUFSIZE];
   char _size[SMALLBUFSIZE];
   char attachment_name[SMALLBUFSIZE];
   char attachment_type[SMALLBUFSIZE];
   char _attachment_size[SMALLBUFSIZE];
   int size, attachment_size, spam, days, folder_id;
};


struct parser_state {
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
   int meta_content_type;
   int skip_html;
   int has_to_dump;
   int has_to_dump_whole_body;
   int fd;
   int b64fd;
   int mfd;
   int octetstream;
   int realbinary;
   int content_type_is_set;
   int pushed_pointer;
   int saved_size;
   unsigned int writebufpos;
   unsigned int abufpos;
   unsigned int received_header;
   char attachedfile[RND_STR_LEN+SMALLBUFSIZE];
   char message_id[SMALLBUFSIZE];
   char message_id_hash[2*DIGEST_LENGTH+1];
   char miscbuf[MAX_TOKEN_LEN];
   char qpbuf[MAX_TOKEN_LEN];
   char receivedbuf[SMALLBUFSIZE];
   unsigned long n_token;
   unsigned long n_subject_token;
   unsigned long n_body_token;
   unsigned long n_chain_token;

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

   char b_from[SMALLBUFSIZE], b_from_domain[SMALLBUFSIZE], b_sender[SMALLBUFSIZE], b_sender_domain[SMALLBUFSIZE], b_to[MAXBUFSIZE], b_to_domain[SMALLBUFSIZE], b_subject[MAXBUFSIZE], b_body[BIGBUFSIZE];
   char b_journal_to[MAXBUFSIZE];

   unsigned int bodylen;
   unsigned int tolen;
   unsigned int todomainlen;
   unsigned int found_security_header;

   long unsigned int journaltolen;

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
   MYSQL mysql, sphx;
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
   char *move_folder;
   char *failed_folder;
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
   int cap_uidplus;
   int fd;
   long total_size;
   int dryrun;
   int tot_msgs;
   int port;
   int seq;
   int table_id;
   int delay;
   char *server;
   char *username;
   char *password;
   char *database;
   char *skiplist;
   char *folder_imap;
   char *folder_name;
   char *mboxdir;
   char *folder;
   char filename[SMALLBUFSIZE];
   time_t started, updated, finished, after, before;
};


struct licence {
   char customer[TINYBUFSIZE];
   char hostname[TINYBUFSIZE];
   char ip[TINYBUFSIZE];
   time_t expiry;
   int licenced_users;
};


struct data {
   int folder, quiet;
   char recursive_folder_names;
   char starttls[TINYBUFSIZE];
   struct node *mydomains[MAXHASH];
   struct node *imapfolders[MAXHASH];
   struct import *import;
   struct licence licence;
   char *dedup;
   int child_serial;
   int pos;

#ifdef HAVE_TRE
   struct node *archiving_rules[1];
   struct node *retention_rules[1];
   struct node *folder_rules[1];
#endif

#ifdef HAVE_MEMCACHED
   struct memcached_server memc;
#endif

   struct net *net;
};


#if !defined(MARIADB_BASE_VERSION) && !defined(MARIADB_VERSION_ID) && \
  MYSQL_VERSION_ID >= 80001 && MYSQL_VERSION_ID != 80002
typedef bool my_bool;
#endif

struct sql {
#ifdef NEED_MYSQL
   MYSQL_STMT *stmt;
#endif
   char *sql[MAX_SQL_VARS];
   int type[MAX_SQL_VARS];
   int len[MAX_SQL_VARS];
   unsigned long length[MAX_SQL_VARS];
   my_bool is_null[MAX_SQL_VARS];
   my_bool error[MAX_SQL_VARS];
   int pos;
};


struct counters {
   unsigned long long c_rcvd;
   unsigned long long c_virus;
   unsigned long long c_duplicate;
   unsigned long long c_ignore;
   unsigned long long c_size;
   unsigned long long c_stored_size;
};


struct smtp_session {
   char ttmpfile[SMALLBUFSIZE];
   char mailfrom[SMALLBUFSIZE];
   char rcptto[MAX_RCPT_TO][SMALLBUFSIZE];
   char remote_host[INET6_ADDRSTRLEN+1];
   char nullbyte;
   time_t lasttime;
   int protocol_state;
   int slot;
   int fd;
   int bad;
   int tot_len;
   int bdat_bytes_to_read;
   int num_of_rcpt_to;
   struct config *cfg;
   struct net net;
   int max_message_size;
   char *buf;
   int buflen;
   int bufsize;
   int too_big;
   int mail_size;
};

struct tls_protocol {
   char *proto;
   int version;
};

#endif /* _DEFS_H */
