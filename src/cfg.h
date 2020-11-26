/*
 * cfg.h, SJ
 */

#ifndef _CFG_H
 #define _CFG_H

#include "config.h"

struct config {
   int server_id;
   char username[MAXVAL];

   char hostid[MAXVAL];
   int hostid_len;

   char pidfile[MAXVAL];

   char listen_addr[MAXVAL];
   int listen_port;

   char clamd_addr[MAXVAL];
   int clamd_port;
   char clamd_socket[MAXVAL];

   int encrypt_messages;

   int enable_chunking;
   int tls_enable;
   char pemfile[MAXVAL];
   char cipher_list[MAXVAL];

   int use_antivirus;

   char memcached_servers[MAXVAL];
   int memcached_ttl;

   int max_connections;
   int number_of_worker_processes;
   int max_requests_per_child;

   int backlog;

   int process_rcpt_to_addresses;

   char workdir[MAXVAL];
   char queuedir[MAXVAL];

   int verbosity;
   char locale[MAXVAL];

   int check_for_client_timeout_interval;
   int smtp_timeout;
   int helper_timeout;
   int extract_attachments;

   char piler_header_field[MAXVAL];
   char extra_to_field[MAXVAL];

   unsigned char key[KEYLEN];
   unsigned char iv[MAXVAL];

   char spam_header_line[MAXVAL];

   int default_retention_days;

   char security_header[MAXVAL];

   // mysql stuff

   char mysqlcharset[MAXVAL];
   char mysqlhost[MAXVAL];
   int mysqlport;
   char mysqlsocket[MAXVAL];
   char mysqluser[MAXVAL];
   char mysqlpwd[MAXVAL];
   char mysqldb[MAXVAL];
   int mysql_connect_timeout;

   int update_counters_to_memcached;
   int memcached_to_db_interval;

   int archive_emails_not_having_message_id;

   int archive_only_mydomains;

   int min_word_len;

   int min_message_size;

   int tweak_sent_time_offset;

   int enable_cjk;

   int syslog_recipients;

   int mmap_dedup_test;

   int enable_folders;

   int debug;
};


#endif /* _CFG_H */
