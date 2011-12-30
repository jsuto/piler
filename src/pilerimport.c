/*
 * pilerimport.c, SJ
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


int import_message(char *filename, struct session_data *sdata, struct __data *data, struct __config *cfg){
   int i, rc=ERR;
   char *rule;
   struct stat st;
   struct _state state;

   if(stat(filename, &st) != 0){
      printf("cannot read: %s\n", filename);
      return rc;
   }


   create_id(&(sdata->ttmpfile[0]));

   printf("a: %s\n", sdata->ttmpfile);

   link(filename, sdata->ttmpfile);
 
   sdata->num_of_rcpt_to = -1;
   memset(sdata->rcptto[0], 0, SMALLBUFSIZE);

   time(&(sdata->now));
   sdata->sent = 0;
   sdata->hdr_len = 0;
   sdata->tot_len = st.st_size;
   memset(sdata->attachments, 0, SMALLBUFSIZE);

   snprintf(sdata->tmpframe, SMALLBUFSIZE-1, "%s.m", sdata->ttmpfile);

   state = parse_message(sdata, cfg);

   rule = check_againt_ruleset(data->rules, &state, st.st_size);

   if(rule){
      printf("discarding %s by archiving policy: %s\n", filename, rule);
      rc = OK;
      goto ENDE;
   }


   printf("message-id: %s\n", state.message_id);
   printf("from: *%s*\n", state.b_from);
   printf("to: *%s*\n", state.b_to);
   printf("subject: *%s*\n", state.b_subject);

   make_digests(sdata, cfg);

   printf("hdr len: %d\n", sdata->hdr_len);

   printf("body digest: %s\n", sdata->bodydigest);

   for(i=1; i<=state.n_attachments; i++){
      printf("i:%d, name=*%s*, type: *%s*, size: %d, int.name: %s, digest: %s\n", i, state.attachments[i].filename, state.attachments[i].type, state.attachments[i].size, state.attachments[i].internalname, state.attachments[i].digest);
   }

   printf("attachments:%s\n", sdata->attachments);

   printf("\n\n");

   rc = processMessage(sdata, &state, cfg);

ENDE:
   unlink(sdata->ttmpfile);
   unlink(sdata->tmpframe);

   if(rc == ERR) return rc;

   if(rc == ERR_EXISTS) printf("discarding duplicate message: %s\n", sdata->ttmpfile);

   
   return OK;
}


int main(int argc, char **argv){
   int rc;
   struct session_data sdata;
   struct __config cfg;
   struct __data data;


   if(argc < 2){
      printf("usage: %s <message>\n", argv[0]);
      exit(1);
   }

   cfg = read_config(CONFIG_FILE);

   if(read_key(&cfg)){
      printf("%s\n", ERR_READING_KEY);
      return 1;
   }

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



   rc = import_message(argv[1], &sdata, &data, &cfg);



   free_rule(data.rules);

   mysql_close(&(sdata.mysql));

   return 0;
}


