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
   time_t retention_seconds=0;
   struct stat st;
   struct session_data sdata;
   struct parser_state state;
   struct __config cfg;
   struct __data data;
   struct import import;
   char *rule;

   srand(getpid());

   if(argc < 2){
      fprintf(stderr, "usage: %s <message> [<extra recipient>]\n", argv[0]);
      exit(1);
   }

   if(!can_i_write_directory(NULL)) __fatal("cannot write current directory!");

   if(stat(argv[1], &st) != 0){
      fprintf(stderr, "%s is not found\n", argv[1]);
      return 0;
   }

   (void) openlog("test", LOG_PID, LOG_MAIL);

   cfg = read_config(CONFIG_FILE);

   if(open_database(&sdata, &cfg) == ERR) return 0;

   printf("locale: %s\n", setlocale(LC_MESSAGES, cfg.locale));
   setlocale(LC_CTYPE, cfg.locale);

   printf("build: %d\n", get_build());

   import.extra_recipient = NULL;

   if(argc > 2) import.extra_recipient = argv[2];

   data.import = &import;

   data.folder = 0;
   data.recursive_folder_names = 0;

   inithash(data.mydomains);

   initrules(data.archiving_rules);
   initrules(data.retention_rules);
   initrules(data.folder_rules);

   load_mydomains(&sdata, &data, &cfg);

   load_rules(&sdata, &data, data.archiving_rules, SQL_ARCHIVING_RULE_TABLE);
   load_rules(&sdata, &data, data.retention_rules, SQL_RETENTION_RULE_TABLE);
   load_rules(&sdata, &data, data.folder_rules, SQL_FOLDER_RULE_TABLE);


   init_session_data(&sdata, &cfg);
 
   sdata.delivered = 0;
   sdata.tot_len = st.st_size;
   sdata.import = 1;

   snprintf(sdata.ttmpfile, SMALLBUFSIZE-1, "%s", argv[1]);
   snprintf(sdata.filename, SMALLBUFSIZE-1, "%s", argv[1]);
   snprintf(sdata.tmpframe, SMALLBUFSIZE-1, "%s.m", argv[1]);

   printf("parsing...\n");
   state = parse_message(&sdata, 1, &data, &cfg);

   printf("post parsing...\n");
   post_parse(&sdata, &state, &cfg);

   printf("message-id: %s / %s\n", state.message_id, state.message_id_hash);
   printf("from: *%s (%s)*\n", state.b_from, state.b_from_domain);
   printf("to: *%s (%s)*\n", state.b_to, state.b_to_domain);
   printf("reference: *%s*\n", state.reference);
   printf("subject: *%s*\n", state.b_subject);
   printf("body: *%s*\n", state.b_body);

   printf("sent: %lu, delivered-date: %lu\n", sdata.sent, sdata.delivered);

   make_digests(&sdata, &cfg);

   printf("hdr len: %d\n", sdata.hdr_len);

   rule = check_againt_ruleset(data.archiving_rules, &state, st.st_size, sdata.spam_message);
 
   printf("body digest: %s\n", sdata.bodydigest);

   printf("rules check: %s\n", rule);

   retention_seconds = query_retain_period(&data, &state, st.st_size, sdata.spam_message, &cfg);
   sdata.retained = sdata.now + retention_seconds;

   printf("folder: %d\n", get_folder_id_by_rule(&data, &state, st.st_size, sdata.spam_message, &cfg));

   printf("retention period: %lu (%ld days)\n", sdata.retained, retention_seconds/86400);

   clearrules(data.archiving_rules);
   clearrules(data.retention_rules);
   clearrules(data.folder_rules);

   clearhash(data.mydomains);

   for(i=1; i<=state.n_attachments; i++){
      printf("i:%d, name=*%s*, type: *%s*, size: %d, int.name: %s, digest: %s\n", i, state.attachments[i].filename, state.attachments[i].type, state.attachments[i].size, state.attachments[i].internalname, state.attachments[i].digest);
      unlink(state.attachments[i].internalname);
   }

   unlink(sdata.tmpframe);

   printf("attachments:%s\n", sdata.attachments);

   printf("direction: %d\n", sdata.direction);

   printf("spam: %d\n", sdata.spam_message);

   if(sdata.internal_sender == 0 && sdata.internal_recipient == 0) printf("NOT IN mydomains\n");

   printf("\n\n");

   close_database(&sdata);

   return 0;
}


