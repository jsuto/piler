/*
 * import.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>
#include <syslog.h>
#include <piler.h>


int import_message(char *filename, struct session_data *sdata, struct __data *data, struct __config *cfg){
   int rc=ERR, fd;
   char *rule;
   struct stat st;
   struct _state state;
   struct __counters counters;


   init_session_data(sdata, cfg);

   if(data->import->extra_recipient){
      snprintf(sdata->rcptto[0], SMALLBUFSIZE-1, "%s", data->import->extra_recipient);
      sdata->num_of_rcpt_to = 1;
   }

   if(cfg->verbosity > 1) printf("processing: %s\n", filename);

   if(strcmp(filename, "-") == 0){

      if(read_from_stdin(sdata) == ERR){
         printf("error reading from stdin\n");
         return rc;
      }

      snprintf(sdata->filename, SMALLBUFSIZE-1, "%s", sdata->ttmpfile);

   }
   else {

      if(stat(filename, &st) != 0){
         printf("cannot stat() %s\n", filename);
         return rc;
      }

      if(S_ISREG(st.st_mode) == 0){
         printf("%s is not a file\n", filename);
         return rc;
      }

      fd = open(filename, O_RDONLY);
      if(fd == -1){
         printf("cannot open %s\n", filename);
         return rc;
      }
      close(fd);

      snprintf(sdata->filename, SMALLBUFSIZE-1, "%s", filename);

      sdata->tot_len = st.st_size;

      data->import->total_size += st.st_size;
   }


   
   sdata->delivered = 0;

   sdata->import = 1;

   state = parse_message(sdata, 1, data, cfg);
   post_parse(sdata, &state, cfg);

   rule = check_againt_ruleset(data->archiving_rules, &state, sdata->tot_len, sdata->spam_message);

   if(rule){
      if(data->quiet == 0) printf("discarding %s by archiving policy: %s\n", filename, rule);
      rc = OK;
   }
   else {
      make_digests(sdata, cfg);

      if(sdata->hdr_len < 10){
         printf("%s: invalid message, hdr_len: %d\n", filename, sdata->hdr_len);
         return ERR;
      }

      rc = process_message(sdata, &state, data, cfg);
      unlink(state.message_id_hash);
   }

   unlink(sdata->tmpframe);

   if(strcmp(filename, "-") == 0) unlink(sdata->ttmpfile);


   switch(rc) {
      case OK:
                        bzero(&counters, sizeof(counters));
                        counters.c_rcvd = 1; 
                        counters.c_size += sdata->tot_len;
                        counters.c_stored_size = sdata->stored_len;
                        update_counters(sdata, data, &counters, cfg);

                        break;

      case ERR_EXISTS:
                        rc = OK;

                        bzero(&counters, sizeof(counters));
                        counters.c_duplicate = 1;
                        update_counters(sdata, data, &counters, cfg);

                        if(data->quiet == 0) printf("duplicate: %s (duplicate id: %llu)\n", filename, sdata->duplicate_id);
                        break;

      default:
                        printf("failed to import: %s (id: %s)\n", filename, sdata->ttmpfile);
                        break;
   } 

   return rc;
}


int get_folder_id(struct session_data *sdata, struct __data *data, char *foldername, int parent_id, struct __config *cfg){
   int id=ERR_FOLDER;

   if(prepare_sql_statement(sdata, &(data->stmt_get_folder_id), SQL_PREPARED_STMT_GET_FOLDER_ID, cfg) == ERR) return id;

   p_bind_init(data);
   data->sql[data->pos] = foldername; data->type[data->pos] = TYPE_STRING; data->pos++;
   data->sql[data->pos] = (char *)&parent_id; data->type[data->pos] = TYPE_LONG; data->pos++;

   if(p_exec_query(sdata, data->stmt_get_folder_id, data) == OK){

      p_bind_init(data);
      data->sql[data->pos] = (char *)&id; data->type[data->pos] = TYPE_LONG; data->len[data->pos] = sizeof(unsigned long); data->pos++;

      p_store_results(sdata, data->stmt_get_folder_id, data);
      p_fetch_results(data->stmt_get_folder_id);
      p_free_results(data->stmt_get_folder_id);
   }

   close_prepared_statement(data->stmt_get_folder_id);

   return id;
}


int add_new_folder(struct session_data *sdata, struct __data *data, char *foldername, int parent_id, struct __config *cfg){
   int id=ERR_FOLDER;

   if(foldername == NULL) return id;

   if(prepare_sql_statement(sdata, &(data->stmt_insert_into_folder_table), SQL_PREPARED_STMT_INSERT_INTO_FOLDER_TABLE, cfg) == ERR) return id;

   p_bind_init(data);
   data->sql[data->pos] = foldername; data->type[data->pos] = TYPE_STRING; data->pos++;
   data->sql[data->pos] = (char *)&parent_id; data->type[data->pos] = TYPE_LONG; data->pos++;

   if(p_exec_query(sdata, data->stmt_insert_into_folder_table, data) == OK){
      id = p_get_insert_id(data->stmt_insert_into_folder_table);
   }

   close_prepared_statement(data->stmt_insert_into_folder_table);

   return id;
}


void update_import_job_stat(struct session_data *sdata, struct __data *data, struct __config *cfg){
   char buf[SMALLBUFSIZE];

   snprintf(buf, sizeof(buf)-1, "update import set status=%d, started=%ld, updated=%ld, finished=%ld, total=%d, imported=%d where id=%d", data->import->status, data->import->started, data->import->updated, data->import->finished, data->import->total_messages, data->import->processed_messages, data->import->import_job_id);

   p_query(sdata, buf, cfg);
}



