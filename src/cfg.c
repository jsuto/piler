/*
 * cfg.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "cfg.h"
#include "misc.h"
#include "config.h"


int string_parser(char *src, char *target, int limit){
   snprintf(target, limit, "%s", src);

   return 0;
};

int int_parser(char *src, int *target){
   *target = strtol(src, (char **) NULL, 10);

   return 0;
};

struct _parse_rule {
   char *name;
   char *type;
   int(*parser)(char*,void*,int);
   size_t offset;
   char *def_val;
   int limit;
};


/*
 * all known configuration items in order
 */

struct _parse_rule config_parse_rules[] =
{

   { "archive_emails_not_having_message_id", "integer", (void*) int_parser, offsetof(struct config, archive_emails_not_having_message_id), "0", sizeof(int)},
   { "archive_only_mydomains", "integer", (void*) int_parser, offsetof(struct config, archive_only_mydomains), "0", sizeof(int)},
   { "backlog", "integer", (void*) int_parser, offsetof(struct config, backlog), "20", sizeof(int)},
   { "check_for_client_timeout_interval", "integer", (void*) int_parser, offsetof(struct config, check_for_client_timeout_interval), "20", sizeof(int)},
   { "cipher_list", "string", (void*) string_parser, offsetof(struct config, cipher_list), "ECDH+AESGCM:DH+AESGCM:ECDH+AES256:DH+AES256:ECDH+AES128:DH+AES:ECDH+3DES:DH+3DES:RSA+AESGCM:RSA+AES:RSA+3DES:!aNULL:!MD5:!DSS", MAXVAL-1},
   { "clamd_addr", "string", (void*) string_parser, offsetof(struct config, clamd_addr), "", MAXVAL-1},
   { "clamd_port", "integer", (void*) int_parser, offsetof(struct config, clamd_port), "0", sizeof(int)},
   { "clamd_socket", "string", (void*) string_parser, offsetof(struct config, clamd_socket), CLAMD_SOCKET, MAXVAL-1},
   { "debug", "integer", (void*) int_parser, offsetof(struct config, debug), "0", sizeof(int)},
   { "default_retention_days", "integer", (void*) int_parser, offsetof(struct config, default_retention_days), "2557", sizeof(int)},
   { "enable_chunking", "integer", (void*) int_parser, offsetof(struct config, enable_chunking), "0", sizeof(int)},
   { "enable_cjk", "integer", (void*) int_parser, offsetof(struct config, enable_cjk), "0", sizeof(int)},
   { "enable_folders", "integer", (void*) int_parser, offsetof(struct config, enable_folders), "0", sizeof(int)},
   { "encrypt_messages", "integer", (void*) int_parser, offsetof(struct config, encrypt_messages), "1", sizeof(int)},
   { "extra_to_field", "string", (void*) string_parser, offsetof(struct config, extra_to_field), "", MAXVAL-1},
   { "extract_attachments", "integer", (void*) int_parser, offsetof(struct config, extract_attachments), "1", sizeof(int)},
   { "helper_timeout", "integer", (void*) int_parser, offsetof(struct config, helper_timeout), "20", sizeof(int)},
   { "hostid", "string", (void*) string_parser, offsetof(struct config, hostid), HOSTID, MAXVAL-1},
   { "iv", "string", (void*) string_parser, offsetof(struct config, iv), "", MAXVAL-1},
   { "listen_addr", "string", (void*) string_parser, offsetof(struct config, listen_addr), "0.0.0.0", MAXVAL-1},
   { "listen_port", "integer", (void*) int_parser, offsetof(struct config, listen_port), "25", sizeof(int)},
   { "locale", "string", (void*) string_parser, offsetof(struct config, locale), "", MAXVAL-1},
   { "max_connections", "integer", (void*) int_parser, offsetof(struct config, max_connections), "64", sizeof(int)},
   { "max_requests_per_child", "integer", (void*) int_parser, offsetof(struct config, max_requests_per_child), "10000", sizeof(int)},
   { "memcached_servers", "string", (void*) string_parser, offsetof(struct config, memcached_servers), "127.0.0.1", MAXVAL-1},
   { "memcached_to_db_interval", "integer", (void*) int_parser, offsetof(struct config, memcached_to_db_interval), "900", sizeof(int)},
   { "memcached_ttl", "integer", (void*) int_parser, offsetof(struct config, memcached_ttl), "86400", sizeof(int)},
   { "min_message_size", "integer", (void*) int_parser, offsetof(struct config, min_message_size), "100", sizeof(int)},
   { "min_word_len", "integer", (void*) int_parser, offsetof(struct config, min_word_len), "1", sizeof(int)},
   { "mmap_dedup_test", "integer", (void*) int_parser, offsetof(struct config, mmap_dedup_test), "0", sizeof(int)},
   { "mysqlcharset", "string", (void*) string_parser, offsetof(struct config, mysqlcharset), "utf8mb4", MAXVAL-1},
   { "mysqlhost", "string", (void*) string_parser, offsetof(struct config, mysqlhost), "", MAXVAL-1},
   { "mysqlport", "integer", (void*) int_parser, offsetof(struct config, mysqlport), "", sizeof(int)},
   { "mysqlsocket", "string", (void*) string_parser, offsetof(struct config, mysqlsocket), "/tmp/mysql.sock", MAXVAL-1},
   { "mysqluser", "string", (void*) string_parser, offsetof(struct config, mysqluser), "piler", MAXVAL-1},
   { "mysqlpwd", "string", (void*) string_parser, offsetof(struct config, mysqlpwd), "", MAXVAL-1},
   { "mysqldb", "string", (void*) string_parser, offsetof(struct config, mysqldb), "piler", MAXVAL-1},
   { "mysql_connect_timeout", "integer", (void*) int_parser, offsetof(struct config, mysql_connect_timeout), "2", sizeof(int)},
   { "number_of_worker_processes", "integer", (void*) int_parser, offsetof(struct config, number_of_worker_processes), "2", sizeof(int)},
   { "pemfile", "string", (void*) string_parser, offsetof(struct config, pemfile), "", MAXVAL-1},
   { "pidfile", "string", (void*) string_parser, offsetof(struct config, pidfile), PIDFILE, MAXVAL-1},
   { "piler_header_field", "string", (void*) string_parser, offsetof(struct config, piler_header_field), "X-piler-id:", MAXVAL-1},
   { "process_rcpt_to_addresses", "integer", (void*) int_parser, offsetof(struct config, process_rcpt_to_addresses), "0", sizeof(int)},
   { "queuedir", "string", (void*) string_parser, offsetof(struct config, queuedir), QUEUE_DIR, MAXVAL-1},
   { "security_header", "string", (void*) string_parser, offsetof(struct config, security_header), "", MAXVAL-1},
   { "server_id", "integer", (void*) int_parser, offsetof(struct config, server_id), "0", sizeof(int)},
   { "smtp_timeout", "integer", (void*) int_parser, offsetof(struct config, smtp_timeout), "60", sizeof(int)},
   { "spam_header_line", "string", (void*) string_parser, offsetof(struct config, spam_header_line), "", MAXVAL-1},
   { "syslog_recipients", "integer", (void*) int_parser, offsetof(struct config, syslog_recipients), "0", sizeof(int)},
   { "tls_enable", "integer", (void*) int_parser, offsetof(struct config, tls_enable), "0", sizeof(int)},
   { "tweak_sent_time_offset", "integer", (void*) int_parser, offsetof(struct config, tweak_sent_time_offset), "0", sizeof(int)},
   { "update_counters_to_memcached", "integer", (void*) int_parser, offsetof(struct config, update_counters_to_memcached), "0", sizeof(int)},
   { "username", "string", (void*) string_parser, offsetof(struct config, username), "piler", MAXVAL-1},
   { "use_antivirus", "integer", (void*) int_parser, offsetof(struct config, use_antivirus), "1", sizeof(int)},
   { "verbosity", "integer", (void*) int_parser, offsetof(struct config, verbosity), "1", sizeof(int)},
   { "workdir", "string", (void*) string_parser, offsetof(struct config, workdir), WORK_DIR, MAXVAL-1},

   {NULL, NULL, NULL, 0, 0, 0}
};


/*
 * parse configfile
 */

int parse_config_file(char *configfile, struct config *target_cfg, struct _parse_rule *rules){
   char line[MAXVAL], *chpos;
   FILE *f;

   if(!configfile) return -1;

   f = fopen(configfile, "r");
   if(!f) return -1;

   while(fgets(&line[0], MAXVAL-1, f)){
      if(line[0] == ';' || line[0] == '#') continue;

      chpos = strchr(line, '=');

      if(chpos){
         trimBuffer(chpos+1);
         *chpos = '\0';
         int i = 0;

         while(rules[i].name){
            if(!strcmp(line, rules[i].name)) {
               if(rules[i].parser(chpos+1, (char*)target_cfg + rules[i].offset, rules[i].limit)){
                  printf("failed to parse %s: \"%s\"\n", line, chpos+1);
               }
               break;
            }

            i++;
         }

         if(!rules[i].name) syslog(LOG_PRIORITY, "unknown key: \"%s\"", line);
      }
   }

   fclose(f);

   return 0;
}


int load_default_config(struct config *cfg, struct _parse_rule *rules){
   int i=0;

   while(rules[i].name){
      rules[i].parser(rules[i].def_val, (char*)cfg + rules[i].offset, rules[i].limit);
      i++;
   }

   return 0;
}


/*
 * read configuration file variables
 */

struct config read_config(char *configfile){
   struct config cfg;

   /* reset config structure and fill it with defaults */

   memset((char *)&cfg, 0, sizeof(struct config));

   load_default_config(&cfg, config_parse_rules);


   /* parse the config file */

   if(parse_config_file(configfile, &cfg, config_parse_rules) == -1) printf("error parsing the configfile: %s\n", configfile);

   cfg.hostid_len = strlen(cfg.hostid);

   return cfg;
}


/*
 * print a single configuration item as key=value
 */

void print_config_item(struct config *cfg, struct _parse_rule *rules, int i){
   int j;
   float f;
   char *p, buf[MAXVAL];

   p = (char*)cfg + rules[i].offset;

   if(strcmp(rules[i].type, "integer") == 0){
      memcpy((char*)&j, p, sizeof(int));
      printf("%s=%d\n", rules[i].name, j);
   }
   else if(strcmp(rules[i].type, "float") == 0){
      memcpy((char*)&f, p, sizeof(float));
      printf("%s=%.4f\n", rules[i].name, f);
   }
   else if(strcmp(rules[i].type, "multi_line_string") == 0){
      j = 0;
      do {
         p = split_str(p, "\r\n", buf, MAXVAL-1);
         if(p || !j) printf("%s=%s\n", rules[i].name, buf);
         j++;
      } while(p);
   }
   else {
      trimBuffer(p);
      printf("%s=%s\n", rules[i].name, p);
   }

}


/*
 * print all known configuration items
 */

void print_config_all(struct config *cfg, char *key){
   int i=0;
   struct _parse_rule *rules;

   rules = &config_parse_rules[0];

   while(rules[i].name){
      if(key == NULL){
         print_config_item(cfg, rules, i);
      }
      else {
         if(strcmp(key, rules[i].name) == 0)
            print_config_item(cfg, rules, i);
      }

      i++;
   }
}


/*
 * print all configuration items found in configfile
 */

void print_config(char *configfile, struct config *cfg){
   FILE *f;
   char line[MAXVAL], *chpos, previtem[MAXVAL];
   struct _parse_rule *rules;


   if(!configfile) return;

   f = fopen(configfile, "r");
   if(!f) return;

   rules = &config_parse_rules[0];

   memset(previtem, 0, MAXVAL);

   while(fgets(&line[0], MAXVAL-1, f)){
      if(line[0] == ';' || line[0] == '#') continue;

      chpos = strchr(line, '=');

      if(chpos){
         trimBuffer(chpos+1);
         *chpos = '\0';
         int i = 0;

         while(rules[i].name){
            if(strcmp(line, rules[i].name) == 0) {
               if(strcmp(line, previtem)) print_config_item(cfg, rules, i);

               snprintf(previtem, MAXVAL-1, "%s", line);
               break;
            }

            i++;
         }

         if(!rules[i].name) printf("unknown key: \"%s\" \n", line);
      }
   }

   fclose(f);
}
