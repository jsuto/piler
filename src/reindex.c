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
   printf("    [-c|--config <config file>]       Config file to use if not the default\n");
   printf("    -f <from id>                      Start indexing from this id\n");
   printf("    -t <to id>                        Up to this id\n");
   printf("    -a                                Reindex all records\n");
   printf("    -p                                Display progress counters\n");

   exit(0);
}


void p_clean_exit(char *msg, int rc){
   if(msg) printf("error: %s\n", msg);

   exit(rc);
}


uint64 get_max_meta_id(struct session_data *sdata){
   char s[SMALLBUFSIZE];
   uint64 id=0;
   struct sql sql;

   snprintf(s, sizeof(s)-1, "SELECT MAX(`id`) FROM %s", SQL_METADATA_TABLE);


   if(prepare_sql_statement(sdata, &sql, s) == ERR) return id;


   p_bind_init(&sql);

   if(p_exec_stmt(sdata, &sql) == OK){

      p_bind_init(&sql);

      sql.sql[sql.pos] = (char *)&id; sql.type[sql.pos] = TYPE_LONGLONG; sql.len[sql.pos] = sizeof(uint64); sql.pos++;

      p_store_results(&sql);
      p_fetch_results(&sql);
      p_free_results(&sql);
   }

   close_prepared_statement(&sql);


   return id;
}


uint64 retrieve_email_by_metadata_id(struct session_data *sdata, struct data *data, uint64 from_id, uint64 to_id, struct config *cfg){
   FILE *f;
   char filename[SMALLBUFSIZE];
   char s[SMALLBUFSIZE];
   int rc=0;
   uint64 stored_id=0, reindexed=0, delta;
   struct parser_state state;
   struct sql sql;

   delta = to_id - from_id;

   if(cfg->enable_folders == 1)
      snprintf(s, sizeof(s)-1, "SELECT m.`id`, `piler_id`, `arrived`, `sent`, f.folder_id FROM %s m, %s f WHERE m.id=f.id AND (m.id BETWEEN %llu AND %llu) AND `deleted`=0", SQL_METADATA_TABLE, SQL_FOLDER_MESSAGE_TABLE, from_id, to_id);
   else
      snprintf(s, sizeof(s)-1, "SELECT `id`, `piler_id`, `arrived`, `sent` FROM %s WHERE (id BETWEEN %llu AND %llu) AND `deleted`=0", SQL_METADATA_TABLE, from_id, to_id);


   if(prepare_sql_statement(sdata, &sql, s) == ERR) return reindexed;

   p_bind_init(&sql);

   if(p_exec_stmt(sdata, &sql) == OK){

      p_bind_init(&sql);

      sql.sql[sql.pos] = (char *)&stored_id; sql.type[sql.pos] = TYPE_LONGLONG; sql.len[sql.pos] = sizeof(uint64); sql.pos++;
      sql.sql[sql.pos] = sdata->ttmpfile; sql.type[sql.pos] = TYPE_STRING; sql.len[sql.pos] = RND_STR_LEN+2; sql.pos++;
      sql.sql[sql.pos] = (char *)&(sdata->now); sql.type[sql.pos] = TYPE_LONG; sql.len[sql.pos] = sizeof(unsigned long); sql.pos++;
      sql.sql[sql.pos] = (char *)&(sdata->sent); sql.type[sql.pos] = TYPE_LONG; sql.len[sql.pos] = sizeof(unsigned long); sql.pos++;
      if(cfg->enable_folders == 1){
         sql.sql[sql.pos] = (char *)&(data->folder); sql.type[sql.pos] = TYPE_LONG; sql.len[sql.pos] = sizeof(unsigned long); sql.pos++;
      }

      p_store_results(&sql);

      while(p_fetch_results(&sql) == OK){

         if(stored_id > 0){

            snprintf(filename, sizeof(filename)-1, "%llu.eml", stored_id);

            f = fopen(filename, "w");
            if(f){
               rc = retrieve_email_from_archive(sdata, f, cfg);
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

               if(progressbar){
                  printf("processed: %8llu [%3d%%]\r", reindexed, (int)(100*reindexed/delta));
                  fflush(stdout);
               }

            }
            else printf("cannot open: %s\n", filename);

         }

      }

      p_free_results(&sql);
   }

   close_prepared_statement(&sql);


   if(progressbar) printf("\n");

   return reindexed;
}


int main(int argc, char **argv){
   int c, all=0;
   uint64 from_id=0, to_id=0, n=0;
   char *configfile=CONFIG_FILE, *folder=NULL;
   struct session_data sdata;
   struct data data;
   struct config cfg;


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

   if(!can_i_write_directory(NULL)) __fatal("cannot write current directory!");

   (void) openlog("reindex", LOG_PID, LOG_MAIL);

   srand(getpid());

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
   initrules(data.folder_rules);

   init_session_data(&sdata, &cfg);


   if(open_database(&sdata, &cfg) == ERR){
      p_clean_exit("cannot connect to mysql server", 1);
   }

   load_rules(&sdata, data.folder_rules, SQL_FOLDER_RULE_TABLE);

   if(folder){
      data.folder = get_folder_id(&sdata, folder, 0);
      if(data.folder == 0){
         printf("error: could not get folder id for '%s'\n", folder);
         return 0;
      }
   }


   load_mydomains(&sdata, &data, &cfg);

   if(all == 1){
      from_id = 1;
      to_id = get_max_meta_id(&sdata);
   }

   n = retrieve_email_by_metadata_id(&sdata, &data, from_id, to_id, &cfg);

   printf("put %llu messages to %s table for reindexing\n", n, SQL_SPHINX_TABLE);

   clearrules(data.folder_rules);

   clearhash(data.mydomains);

   close_database(&sdata);

   return 0;
}
