/*
 * test.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>
#include <syslog.h>
#include <piler.h>


int main(int argc, char **argv){
   int i, rc;
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

   data.rules = NULL;

   load_archiving_rules(&sdata, &(data.rules));

   rc = 0;
 
   sdata.num_of_rcpt_to = -1;
   time(&(sdata.now));
   sdata.sent = 0;
   sdata.tot_len = st.st_size;
   memset(sdata.rcptto[0], 0, SMALLBUFSIZE);
   snprintf(sdata.ttmpfile, SMALLBUFSIZE-1, "%s", argv[1]);
   snprintf(sdata.tmpframe, SMALLBUFSIZE-1, "%s.m", argv[1]);


   state = parse_message(&sdata, &cfg);

   printf("message-id: %s\n", state.message_id);
   printf("from: *%s*\n", state.b_from);
   printf("to: *%s*\n", state.b_to);
   printf("subject: *%s*\n", state.b_subject);
   //printf("body: *%s*\n", state.b_body);

   make_digests(&sdata, &cfg);

   printf("hdr len: %d\n", sdata.hdr_len);

   rule = check_againt_ruleset(data.rules, &state, st.st_size);
 
   printf("body digest: %s\n", sdata.bodydigest);

   printf("rules check: %s\n", rule);

   free_rule(data.rules);

   for(i=1; i<=state.n_attachments; i++){
      printf("i:%d, name=*%s*, type: *%s*, size: %d, int.name: %s, digest: %s\n", i, state.attachments[i].filename, state.attachments[i].type, state.attachments[i].size, state.attachments[i].internalname, state.attachments[i].digest);
   }


   printf("\n\n");

   mysql_close(&(sdata.mysql));

   return 0;
}


