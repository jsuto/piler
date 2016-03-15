/*
 * fixmetadata.c, SJ
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


uint64 get_metaid_by_messageid(struct session_data *sdata, struct __data *data, char *message_id, struct __config *cfg);


int update_meta_data(struct session_data *sdata, struct parser_state *state, struct __data *data, struct __config *cfg){
   int ret=ERR;
   uint64 id=0;
   char *subj;

   id = get_metaid_by_messageid(sdata, data, state->message_id, cfg);

   if(id <= 0) return ERR;

   subj = state->b_subject;
   if(*subj == ' ') subj++;

   if(prepare_sql_statement(sdata, &(data->stmt_update_metadata), SQL_PREPARED_STMT_UPDATE_META_TABLE, cfg) == ERR) return ret;

   p_bind_init(data);

   data->sql[data->pos] = subj; data->type[data->pos] = TYPE_STRING; data->pos++;
   data->sql[data->pos] = (char *)&state->n_attachments; data->type[data->pos] = TYPE_LONG; data->pos++;
   data->sql[data->pos] = (char *)&id; data->type[data->pos] = TYPE_LONGLONG; data->pos++;

   if(p_exec_query(sdata, data->stmt_update_metadata, data) == OK) ret = OK;

   close_prepared_statement(data->stmt_update_metadata);

   return ret;
}


int main(int argc, char **argv){
   int i;
   struct stat st;
   struct session_data sdata;
   struct parser_state state;
   struct __config cfg;
   struct __data data;

   srand(getpid());

   if(argc < 2){
      fprintf(stderr, "usage: %s <message>\n", argv[0]);
      exit(1);
   }

   if(!can_i_write_current_directory()) __fatal("cannot write current directory!");

   if(stat(argv[1], &st) != 0){
      fprintf(stderr, "%s is not found\n", argv[1]);
      return 0;
   }

   (void) openlog("fixmetadata", LOG_PID, LOG_MAIL);

   cfg = read_config(CONFIG_FILE);

   if(open_database(&sdata, &cfg) == ERR) return 0;

   setlocale(LC_CTYPE, cfg.locale);

   data.folder = 0;
   data.recursive_folder_names = 0;

   inithash(data.mydomains);

   //initrules(data.archiving_rules);
   //initrules(data.retention_rules);
   //initrules(data.folder_rules);

   load_mydomains(&sdata, &data, &cfg);

   //load_rules(&sdata, &data, data.archiving_rules, SQL_ARCHIVING_RULE_TABLE, &cfg);
   //load_rules(&sdata, &data, data.retention_rules, SQL_RETENTION_RULE_TABLE, &cfg);
   //load_rules(&sdata, &data, data.folder_rules, SQL_FOLDER_RULE_TABLE, &cfg);


   init_session_data(&sdata, &cfg);
 
   sdata.delivered = 0;
   sdata.tot_len = st.st_size;

   snprintf(sdata.ttmpfile, SMALLBUFSIZE-1, "%s", argv[1]);
   snprintf(sdata.filename, SMALLBUFSIZE-1, "%s", argv[1]);
   snprintf(sdata.tmpframe, SMALLBUFSIZE-1, "%s.m", argv[1]);

   state = parse_message(&sdata, 1, &data, &cfg);
   post_parse(&sdata, &state, &cfg);

   update_meta_data(&sdata, &state, &data, &cfg);

   //sdata.retained = sdata.now + query_retain_period(&data, &state, st.st_size, sdata.spam_message, &cfg);

   //clearrules(data.archiving_rules);
   //clearrules(data.retention_rules);
   //clearrules(data.folder_rules);

   clearhash(data.mydomains);

   for(i=1; i<=state.n_attachments; i++){
      unlink(state.attachments[i].internalname);
   }

   unlink(sdata.tmpframe);

   close_database(&sdata);

   return 0;
}


