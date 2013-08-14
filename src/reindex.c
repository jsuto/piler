/*
 * reindex.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>
#include <syslog.h>
#include <getopt.h>
#include <piler.h>


extern char *optarg;
extern int optind;

int progressbar = 0;


void usage(){
   printf("\nusage: reindex \n\n");
   printf("    [-c|--config <config file>] \n");
   printf("    -f <from id>\n");
   printf("    -t <to id>\n");
   printf("    -a\n");
   printf("    [-p]\n");

   exit(0);
}


void p_clean_exit(char *msg, int rc){
   if(msg) printf("error: %s\n", msg);

   exit(rc);
}


uint64 get_max_meta_id(struct session_data *sdata, struct __data *data){
   char s[SMALLBUFSIZE];
   uint64 id=0;

   snprintf(s, sizeof(s)-1, "SELECT MAX(`id`) FROM %s", SQL_METADATA_TABLE);


   if(prepare_sql_statement(sdata, &(data->stmt_generic), s) == ERR) return id;


   p_bind_init(data);

   if(p_exec_query(sdata, data->stmt_generic, data) == ERR) goto ENDE;



   p_bind_init(data);

   data->sql[data->pos] = (char *)&id; data->type[data->pos] = TYPE_LONGLONG; data->len[data->pos] = sizeof(uint64); data->pos++;

   p_store_results(sdata, data->stmt_generic, data);
   p_fetch_results(data->stmt_generic);
   p_free_results(data->stmt_generic);

ENDE:
   close_prepared_statement(data->stmt_generic);


   return id;
}


uint64 retrieve_email_by_metadata_id(struct session_data *sdata, struct __data *data, uint64 from_id, uint64 to_id, struct __config *cfg){
   FILE *f;
   char filename[SMALLBUFSIZE];
   char s[SMALLBUFSIZE];
   int rc=0;
   uint64 stored_id=0, reindexed=0;
   struct _state state;


   snprintf(s, sizeof(s)-1, "SELECT `id`, `piler_id`, `arrived`, `sent` FROM %s WHERE (id BETWEEN %llu AND %llu) AND `deleted`=0", SQL_METADATA_TABLE, from_id, to_id);

   if(prepare_sql_statement(sdata, &(data->stmt_generic), s) == ERR) return reindexed;

   p_bind_init(data);

   if(p_exec_query(sdata, data->stmt_generic, data) == ERR) goto ENDE;

   p_bind_init(data);

   data->sql[data->pos] = (char *)&stored_id; data->type[data->pos] = TYPE_LONGLONG; data->len[data->pos] = sizeof(uint64); data->pos++;
   data->sql[data->pos] = sdata->ttmpfile; data->type[data->pos] = TYPE_STRING; data->len[data->pos] = RND_STR_LEN+2; data->pos++;
   data->sql[data->pos] = (char *)&(sdata->now); data->type[data->pos] = TYPE_LONG; data->len[data->pos] = sizeof(unsigned long); data->pos++;
   data->sql[data->pos] = (char *)&(sdata->sent); data->type[data->pos] = TYPE_LONG; data->len[data->pos] = sizeof(unsigned long); data->pos++;

   p_store_results(sdata, data->stmt_generic, data);

   while(p_fetch_results(data->stmt_generic) == OK){

      if(stored_id > 0){

         snprintf(filename, sizeof(filename)-1, "%llu.eml", stored_id);

         f = fopen(filename, "w");
         if(f){
            rc = retrieve_email_from_archive(sdata, data, f, cfg);
            fclose(f);

            if(rc){
               printf("cannot retrieve: %s\n", filename);
               unlink(filename);
               continue;
            }

            snprintf(sdata->filename, SMALLBUFSIZE-1, "%s", filename);

            state = parse_message(sdata, 0, data, cfg);
            post_parse(sdata, &state, cfg);

            rc = store_index_data(sdata, &state, data, stored_id, cfg);

            if(rc == OK) reindexed++;
            else printf("failed to add to %s table: %s\n", SQL_SPHINX_TABLE, filename);

            unlink(filename);

            if(progressbar && reindexed % 100 == 0) printf(".");
         }
         else printf("cannot open: %s\n", filename);

      }

   }


   p_free_results(data->stmt_generic);


ENDE:
   close_prepared_statement(data->stmt_generic);


   if(progressbar) printf("\n");

   return reindexed;
}


int main(int argc, char **argv){
   int c, all=0;
   uint64 from_id=0, to_id=0, n=0;
   char *configfile=CONFIG_FILE, *folder=NULL;
   struct session_data sdata;
   struct __data data;
   struct __config cfg;


   while(1){
      c = getopt(argc, argv, "c:f:t:F:pahv?");

      if(c == -1) break;

      switch(c){

         case 'c' :
                    configfile = optarg;
                    break;

         case 'f' :
                    from_id = strtoull(optarg, NULL, 10);
                    break;

         case 't' :
                    to_id = strtoull(optarg, NULL, 10);
                    break;

         case 'a' :
                    all = 1;
                    break;

         case 'F' :
                    folder = optarg;
                    break;


         case 'p' :
                    progressbar = 1;
                    break;


         default  :
                    usage();
                    break;
      }

   }


   if(all == 0 && (from_id <= 0 || to_id <= 0) ) usage();


   (void) openlog("reindex", LOG_PID, LOG_MAIL);


   cfg = read_config(configfile);

   if(read_key(&cfg)){
      printf("%s\n", ERR_READING_KEY);
      return 1;
   }

   data.folder = 0;
   data.recursive_folder_names = 0;
   inithash(data.mydomains);
   initrules(data.archiving_rules);
   initrules(data.retention_rules);

   if(folder){
      data.folder = get_folder_id(&sdata, &data, folder, 0);
      if(data.folder == 0){
         printf("error: could not get folder id for '%s'\n", folder);
         return 0;
      }
   }

   init_session_data(&sdata, &cfg);


   if(open_database(&sdata, &cfg) == ERR){
      p_clean_exit("cannot connect to mysql server", 1);
   }


   load_mydomains(&sdata, &data, &cfg);

   if(all == 1){
      from_id = 1;
      to_id = get_max_meta_id(&sdata, &data);
   }

   n = retrieve_email_by_metadata_id(&sdata, &data, from_id, to_id, &cfg);

   printf("put %llu messages to %s table for reindexing\n", n, SQL_SPHINX_TABLE);

   clearhash(data.mydomains);

   close_database(&sdata);

   return 0;
}


