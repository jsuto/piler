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


void usage(){
   printf("\nusage: pilertest\n\n");

   printf("    [-c <config file>]                Config file to use if not the default\n");
   printf("    -m <message eml file>             Message in EML format\n");
   printf("    -a <extra recipient>              Extra recipient\n");

   exit(0);
}


int main(int argc, char **argv){
   int i, c;
   time_t retention_seconds=0;
   struct stat st;
   struct session_data sdata;
   struct parser_state state;
   struct config cfg;
   struct data data;
   struct import import;
   char *configfile=CONFIG_FILE, *rule, *emlfile=NULL;

   import.extra_recipient = NULL;

   srand(getpid());

   while(1){

#ifdef _GNU_SOURCE
      static struct option long_options[] =
         {
            {"config",          required_argument,  0,  'c' },
            {"message",         required_argument,  0,  'm' },
            {"extra-recipient", required_argument,  0,  'a' },
            {0,0,0,0}
         };

      int option_index = 0;


      c = getopt_long(argc, argv, "c:m:a:hv?", long_options, &option_index);
#else
      c = getopt(argc, argv, "c:m:a:hv?");
#endif

      if(c == -1) break;

      switch(c){

         case 'c' :
                    configfile = optarg;
                    break;

         case 'm' :
                    emlfile = optarg;
                    break;

         case 'a':
                    import.extra_recipient = optarg;
                    break;

         default  :
                    usage();
                    break;
      }

   }


   if(emlfile == NULL) usage();

   if(!can_i_write_directory(NULL)) __fatal("cannot write current directory!");

   if(stat(emlfile, &st) != 0){
      fprintf(stderr, "%s is not found\n", emlfile);
      return 0;
   }


   if(!can_i_write_directory(NULL)) __fatal("cannot write current directory!");

   if(stat(emlfile, &st) != 0){
      fprintf(stderr, "%s is not found\n", emlfile);
      return 0;
   }

   (void) openlog("test", LOG_PID, LOG_MAIL);

   cfg = read_config(configfile);

   if(open_database(&sdata, &cfg) == ERR) return 0;

   printf("locale: %s\n", setlocale(LC_MESSAGES, cfg.locale));
   setlocale(LC_CTYPE, cfg.locale);

   printf("build: %d\n", get_build());

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

   snprintf(sdata.ttmpfile, SMALLBUFSIZE-1, "%s", emlfile);
   snprintf(sdata.filename, SMALLBUFSIZE-1, "%s", emlfile);
   snprintf(sdata.tmpframe, SMALLBUFSIZE-1, "%s.m", emlfile);

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


