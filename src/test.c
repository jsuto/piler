/*
 * test.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>
#include <syslog.h>
#include <piler.h>


int main(int argc, char **argv){
   int i;
   struct stat st;
   struct session_data sdata;
   struct _state state;
   struct __config cfg;
   struct __data data;
   char *rule;


   if(argc < 2){
      fprintf(stderr, "usage: %s <message>\n", argv[0]);
      exit(1);
   }

   if(stat(argv[1], &st) != 0){
      fprintf(stderr, "%s is not found\n", argv[1]);
      return 0;
   }

   cfg = read_config(CONFIG_FILE);

   mysql_init(&(sdata.mysql));
   mysql_options(&(sdata.mysql), MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&cfg.mysql_connect_timeout);
   if(mysql_real_connect(&(sdata.mysql), cfg.mysqlhost, cfg.mysqluser, cfg.mysqlpwd, cfg.mysqldb, cfg.mysqlport, cfg.mysqlsocket, 0) == 0){
      printf("cant connect to mysql server\n");
      return 0;
   }

   mysql_real_query(&(sdata.mysql), "SET NAMES utf8", strlen("SET NAMES utf8"));
   mysql_real_query(&(sdata.mysql), "SET CHARACTER SET utf8", strlen("SET CHARACTER SET utf8"));

   printf("locale: %s\n", setlocale(LC_MESSAGES, cfg.locale));
   setlocale(LC_CTYPE, cfg.locale);

   printf("build: %d\n", get_build());

   data.folder = 0;
   data.recursive_folder_names = 0;
   data.archiving_rules = NULL;
   data.retention_rules = NULL;

   load_rules(&sdata, &(data.archiving_rules), SQL_ARCHIVING_RULE_TABLE);
   load_rules(&sdata, &(data.retention_rules), SQL_RETENTION_RULE_TABLE);

   load_mydomains(&sdata, &data, &cfg);

   init_session_data(&sdata, &cfg);
 
   sdata.sent = 0;
   sdata.delivered = 0;
   sdata.tot_len = st.st_size;

   snprintf(sdata.ttmpfile, SMALLBUFSIZE-1, "%s", argv[1]);
   snprintf(sdata.filename, SMALLBUFSIZE-1, "%s", argv[1]);
   snprintf(sdata.tmpframe, SMALLBUFSIZE-1, "%s.m", argv[1]);

   state = parse_message(&sdata, 1, &data, &cfg);
   post_parse(&sdata, &state, &cfg);

   printf("message-id: %s\n", state.message_id);
   printf("from: *%s (%s)*\n", state.b_from, state.b_from_domain);
   printf("to: *%s (%s)*\n", state.b_to, state.b_to_domain);
   printf("reference: *%s*\n", state.reference);
   printf("subject: *%s*\n", state.b_subject);
   printf("body: *%s*\n", state.b_body);

   printf("sent: %ld, delivered-date: %ld\n", sdata.sent, sdata.delivered);

   make_digests(&sdata, &cfg);

   printf("hdr len: %d\n", sdata.hdr_len);

   rule = check_againt_ruleset(data.archiving_rules, &state, st.st_size, sdata.spam_message);
 
   printf("body digest: %s\n", sdata.bodydigest);

   printf("rules check: %s\n", rule);

   sdata.retained = sdata.now + query_retain_period(data.retention_rules, &state, st.st_size, sdata.spam_message, &cfg);

   printf("retention period: %ld\n", sdata.retained);

   free_rule(data.archiving_rules);
   free_rule(data.retention_rules);

   for(i=1; i<=state.n_attachments; i++){
      printf("i:%d, name=*%s*, type: *%s*, size: %d, int.name: %s, digest: %s\n", i, state.attachments[i].filename, state.attachments[i].type, state.attachments[i].size, state.attachments[i].internalname, state.attachments[i].digest);
      unlink(state.attachments[i].internalname);
   }

   unlink(sdata.tmpframe);

   printf("attachments:%s\n", sdata.attachments);

   printf("direction: %d\n", sdata.direction);

   printf("spam: %d\n", sdata.spam_message);

   printf("\n\n");

   mysql_close(&(sdata.mysql));

   return 0;
}


