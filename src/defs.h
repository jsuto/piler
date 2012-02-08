/*
 * defs.h, SJ
 */

#ifndef _DEFS_H
   #define _DEFS_H

#ifdef NEED_MYSQL
  #include <mysql.h>
#endif
#ifdef NEED_SQLITE3
  #include <sqlite3.h>

   /* for older versions of sqlite3 do not have the sqlite3_prepare_v2() function, 2009.12.30, SJ */

  #if SQLITE_VERSION_NUMBER < 3006000
    #define sqlite3_prepare_v2 sqlite3_prepare
  #endif

#endif
#ifdef HAVE_TRE
   #include <tre/tre.h>
   #include <tre/regex.h>
#endif

#include <openssl/sha.h>
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

#define MAXHASH 8171

#define BASE64_RATIO 1.33333333

#define DIGEST_LENGTH SHA256_DIGEST_LENGTH

#define UNDEF 0
#define READY 1
#define BUSY 2


#define MAXCHILDREN 64


typedef void signal_func (int);


struct child {
   pid_t pid;
   int messages;
   int status;
};


struct attachment {
   int size;
   char type[TINYBUFSIZE];
   char filename[TINYBUFSIZE];
   char internalname[TINYBUFSIZE];
   char digest[2*DIGEST_LENGTH+1];
};


struct ptr_array {
   uint64 ptr;
   char piler_id[RND_STR_LEN+2];
   int attachment_id;
};


struct list {
   char s[SMALLBUFSIZE];
   struct list *r;
};


struct rule {
#ifdef HAVE_TRE
   regex_t from;
   regex_t to;
   regex_t subject;
   regex_t attachment_type;
#endif
   int size;
   char _size[4];
   int attachment_size;
   char _attachment_size[4];

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
   int mfd;
   int octetstream;
   int realbinary;
   int content_type_is_set;
   int pushed_pointer;
   int saved_size;
   char attachedfile[RND_STR_LEN+SMALLBUFSIZE];
   char message_id[SMALLBUFSIZE];
   char miscbuf[MAX_TOKEN_LEN];
   char qpbuf[MAX_TOKEN_LEN];
   unsigned long n_token;
   unsigned long n_subject_token;
   unsigned long n_body_token;
   unsigned long n_chain_token;

   char filename[TINYBUFSIZE];
   char type[TINYBUFSIZE];

   struct list *boundaries;
   struct list *rcpt;
   struct list *rcpt_domain;

   int n_attachments;
   struct attachment attachments[MAX_ATTACHMENTS];

   char reference[SMALLBUFSIZE];

   char b_from[SMALLBUFSIZE], b_from_domain[SMALLBUFSIZE], b_to[MAXBUFSIZE], b_to_domain[SMALLBUFSIZE], b_subject[MAXBUFSIZE], b_body[BIGBUFSIZE];
};


struct session_data {
   char filename[SMALLBUFSIZE];
   char ttmpfile[SMALLBUFSIZE], tmpframe[SMALLBUFSIZE], tre, restored_copy;
   char mailfrom[SMALLBUFSIZE], rcptto[MAX_RCPT_TO][SMALLBUFSIZE], client_addr[SMALLBUFSIZE];
   char acceptbuf[SMALLBUFSIZE];
   char attachments[SMALLBUFSIZE];
   char internal_sender, internal_recipient, external_recipient;
   int direction;
   int spam_message;
   int fd, hdr_len, tot_len, num_of_rcpt_to, rav;
   int need_scan;
   float __acquire, __parsed, __av, __store, __compress, __encrypt;
   char bodydigest[2*DIGEST_LENGTH+1];
   char digest[2*DIGEST_LENGTH+1];
   time_t now, sent;
#ifdef NEED_MYSQL
   MYSQL mysql;
#endif
#ifdef NEED_SQLITE3
   sqlite3 *db;
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

   char server_ip[16];
   int server_port;

   char initialised;
};
#endif


struct __data {
#ifdef HAVE_TRE
   struct rule *rules;
#endif

#ifdef HAVE_MEMCACHED
   struct memcached_server memc;
#endif

};


struct __counters {
   unsigned long long c_rcvd;
   unsigned long long c_virus;
   unsigned long long c_duplicate;
   unsigned long long c_ignore;
};

#endif /* _DEFS_H */

