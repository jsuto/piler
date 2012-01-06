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

int multi_line_string_parser(char *src, char *target, int limit){
   if(strlen(src) > 0 && strlen(target) + strlen(src) + 3 < limit){
      strncat(target, src, limit);
      strncat(target, "\r\n", limit);

      return 0;
   }

   return 1;
};

int int_parser(char *src, int *target, int limit){
   *target = strtol(src, (char **) NULL, 10);

   return 0;
};


int float_parser(char *src, float *target, int limit){
   *target = strtof(src, (char **) NULL);

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

   { "backlog", "integer", (void*) int_parser, offsetof(struct __config, backlog), "20", sizeof(int)},
   { "clamd_addr", "string", (void*) string_parser, offsetof(struct __config, clamd_addr), "", MAXVAL-1},
   { "clamd_port", "integer", (void*) int_parser, offsetof(struct __config, clamd_port), "0", sizeof(int)},
   { "clamd_socket", "string", (void*) string_parser, offsetof(struct __config, clamd_socket), CLAMD_SOCKET, MAXVAL-1},
   { "hostid", "string", (void*) string_parser, offsetof(struct __config, hostid), HOSTID, MAXVAL-1},
   { "iv", "string", (void*) string_parser, offsetof(struct __config, iv), "", MAXVAL-1},
   { "listen_addr", "string", (void*) string_parser, offsetof(struct __config, listen_addr), "127.0.0.1", MAXVAL-1},
   { "listen_port", "integer", (void*) int_parser, offsetof(struct __config, listen_port), "10025", sizeof(int)},
   { "locale", "string", (void*) string_parser, offsetof(struct __config, locale), "", MAXVAL-1},
   { "max_requests_per_child", "integer", (void*) int_parser, offsetof(struct __config, max_requests_per_child), "1000", sizeof(int)},
   { "memcached_servers", "string", (void*) string_parser, offsetof(struct __config, memcached_servers), "127.0.0.1", MAXVAL-1},
   { "memcached_to_db_interval", "integer", (void*) int_parser, offsetof(struct __config, memcached_to_db_interval), "900", sizeof(int)},
   { "memcached_ttl", "integer", (void*) int_parser, offsetof(struct __config, memcached_ttl), "86400", sizeof(int)},
   { "mydomains", "string", (void*) string_parser, offsetof(struct __config, mydomains), "", MAXVAL-1},
   { "mysqlhost", "string", (void*) string_parser, offsetof(struct __config, mysqlhost), "", MAXVAL-1},
   { "mysqlport", "integer", (void*) int_parser, offsetof(struct __config, mysqlport), "", sizeof(int)},
   { "mysqlsocket", "string", (void*) string_parser, offsetof(struct __config, mysqlsocket), "/tmp/mysql.sock", MAXVAL-1},
   { "mysqluser", "string", (void*) string_parser, offsetof(struct __config, mysqluser), "clapf", MAXVAL-1},
   { "mysqlpwd", "string", (void*) string_parser, offsetof(struct __config, mysqlpwd), "", MAXVAL-1},
   { "mysqldb", "string", (void*) string_parser, offsetof(struct __config, mysqldb), "clapf", MAXVAL-1},
   { "mysql_connect_timeout", "integer", (void*) int_parser, offsetof(struct __config, mysql_connect_timeout), "2", sizeof(int)},
   { "number_of_worker_processes", "integer", (void*) int_parser, offsetof(struct __config, number_of_worker_processes), "10", sizeof(int)},
   { "pidfile", "string", (void*) string_parser, offsetof(struct __config, pidfile), PIDFILE, MAXVAL-1},
   { "piler_header_field", "string", (void*) string_parser, offsetof(struct __config, piler_header_field), "", MAXVAL-1},
   { "queuedir", "string", (void*) string_parser, offsetof(struct __config, queuedir), QUEUE_DIR, MAXVAL-1},
   { "session_timeout", "integer", (void*) int_parser, offsetof(struct __config, session_timeout), "420", sizeof(int)},
   { "sqlite3_pragma", "string", (void*) string_parser, offsetof(struct __config, sqlite3_pragma), "", MAXVAL-1},
   { "update_counters_to_memcached", "integer", (void*) int_parser, offsetof(struct __config, update_counters_to_memcached), "0", sizeof(int)},
   { "username", "string", (void*) string_parser, offsetof(struct __config, username), "piler", MAXVAL-1},
   { "use_antivirus", "integer", (void*) int_parser, offsetof(struct __config, use_antivirus), "1", sizeof(int)},
   { "verbosity", "integer", (void*) int_parser, offsetof(struct __config, verbosity), "1", sizeof(int)},
   { "workdir", "string", (void*) string_parser, offsetof(struct __config, workdir), WORK_DIR, MAXVAL-1},

   {NULL, NULL, NULL, 0, 0}
};


/*
 * parse configfile
 */

int parse_config_file(char *configfile, struct __config *target_cfg, struct _parse_rule *rules){
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


int load_default_config(struct __config *cfg, struct _parse_rule *rules){
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

struct __config read_config(char *configfile){
   struct __config cfg;

   /* reset config structure and fill it with defaults */

   memset((char *)&cfg, 0, sizeof(struct __config));

   load_default_config(&cfg, config_parse_rules);


   /* parse the config file */

   if(parse_config_file(configfile, &cfg, config_parse_rules) == -1) printf("error parsing the configfile: %s\n", configfile);

   return cfg;
}


/*
 * print a single configuration item as key=value
 */

void print_config_item(struct __config *cfg, struct _parse_rule *rules, int i){
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

void print_config_all(struct __config *cfg, char *key){
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

void print_config(char *configfile, struct __config *cfg){
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


