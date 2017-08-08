/*
 * pilerexport.c, SJ
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

int dryrun = 0;
int exportall = 0;
int rc = 0;
char *query=NULL;
int verbosity = 0;
int max_matches = 1000;
char *index_list = "main1,dailydelta1,delta1";
regex_t regexp;


int export_emails_matching_to_query(struct session_data *sdata, struct data *data, char *s, struct config *cfg);


void usage(){
   printf("\nusage: pilerexport\n\n");

   printf("    [-c <config file>]                Config file to use if not the default\n");
   printf("    -a <start date>                   Start date in YYYY.MM.DD format\n");
   printf("    -b <stop date>                    Stop date in YYYY.MM.DD format\n");
   printf("    -f <email@address.com>            From address\n");
   printf("    -r <email@address.com>            Recipient address\n");
   printf("    -F <domain.com>                   From domain\n");
   printf("    -R <domain.com>                   Recipient domain\n");
   printf("    -s <size>                         Min. size\n");
   printf("    -S <size>                         Max. size\n");
   printf("    -w <where condition>              Where condition to pass to sphinx, eg. \"match('@subject: piler')\"\n");
   printf("    -m <max. matches>                 Max. matches to apply to sphinx query (default: %d)\n", max_matches);
   printf("    -i <index list>                   Sphinx indices to use  (default: %s)\n", index_list);
   printf("    -A                                Export all emails from archive\n");
   printf("    -d                                Dry run\n");

   regfree(&regexp);

   exit(0);
}


void p_clean_exit(char *msg, int rc){
   if(msg) printf("error: %s\n", msg);

   if(query) free(query);

   exit(rc);
}


unsigned long convert_time(char *yyyymmdd, int h, int m, int s){
   char *p;
   struct tm tm;

   if(yyyymmdd == NULL) return 0;

   memset((char*)&tm, 0, sizeof(tm));
   tm.tm_isdst = -1;

   tm.tm_hour = h;
   tm.tm_min = m;
   tm.tm_sec = s;

   p = strchr(yyyymmdd, '.'); if(!p) return 0;
   *p = '\0'; tm.tm_year = atoi(yyyymmdd) - 1900; yyyymmdd = p+1;

   p = strchr(yyyymmdd, '.'); if(!p) return 0;
   *p = '\0'; tm.tm_mon = atoi(yyyymmdd) - 1; yyyymmdd = p+1;

   tm.tm_mday = atoi(yyyymmdd);


   tm.tm_isdst = -1;

   return mktime(&tm);
}


int append_email_to_buffer(char **buffer, char *email){
   int len, arglen;
   char *s=NULL, emailaddress[SMALLBUFSIZE];

   snprintf(emailaddress, sizeof(emailaddress)-1, "'%s'", email);
   arglen = strlen(emailaddress);

   if(!*buffer){
      *buffer = malloc(arglen+1);
      memset(*buffer, 0, arglen+1);
      memcpy(*buffer, emailaddress, arglen);
   }
   else {
      len = strlen(*buffer);
      s = realloc(*buffer, len + arglen+2);
      if(!s){
         printf("malloc problem!\n");
         return 1;
      }

      *buffer = s;

      memset(*buffer+len, 0, arglen+2);
      strcat(*buffer, ",");
      memcpy(*buffer+len+1, emailaddress, arglen);
   }

   return 0;
}


int append_string_to_buffer(char **buffer, char *str){
   int len, arglen;
   char *s=NULL;

   arglen = strlen(str);

   if(!*buffer){
      *buffer = malloc(arglen+1);
      memset(*buffer, 0, arglen+1);
      memcpy(*buffer, str, arglen);
   }
   else {
      len = strlen(*buffer);
      s = realloc(*buffer, len + arglen+1);
      if(!s) return 1;

      *buffer = s;

      memset(*buffer+len, 0, arglen+1);
      memcpy(*buffer+len, str, arglen);
   }

   return 0;
}


uint64 run_query(struct session_data *sdata, struct session_data *sdata2, struct data *data, char *where_condition, uint64 last_id, int *num, struct config *cfg){
   MYSQL_RES *res;
   MYSQL_ROW row;
   int rc=0;
   uint64 id=0;
   char s[SMALLBUFSIZE];

   *num = 0;

   if(!where_condition) return id;

   snprintf(s, sizeof(s)-1, "SELECT `id`, `piler_id`, `digest`, `bodydigest` FROM %s WHERE id IN (", SQL_METADATA_TABLE);
   rc += append_string_to_buffer(&query, s);

   snprintf(s, sizeof(s)-1, "SELECT id FROM %s WHERE %s AND id > %llu ORDER BY id ASC LIMIT 0,%d", index_list, where_condition, last_id, max_matches);

   if(mysql_real_query(&(sdata2->mysql), s, strlen(s)) == 0){
      res = mysql_store_result(&(sdata2->mysql));
      if(res != NULL){
         while((row = mysql_fetch_row(res))){
            id = strtoull(row[0], NULL, 10);
            (*num)++;
            rc += append_string_to_buffer(&query, row[0]);
            rc += append_string_to_buffer(&query, ",");

         }
         mysql_free_result(res);

         rc += append_string_to_buffer(&query, "-1)");
      }
   }

   if(!rc) export_emails_matching_to_query(sdata, data, query, cfg);

   free(query);
   query = NULL;


   return id;
}


uint64 get_total_found(struct session_data *sdata){
   MYSQL_RES *res;
   MYSQL_ROW row;
   uint64 total_found=0;

   if(mysql_real_query(&(sdata->mysql), "SHOW META LIKE 'total_found'", 28) == 0){
      res = mysql_store_result(&(sdata->mysql));
      if(res != NULL){
         while((row = mysql_fetch_row(res))){
            total_found = strtoull(row[1], NULL, 10);
         }
         mysql_free_result(res);
      }
   }

   return total_found;
}


void export_emails_matching_id_list(struct session_data *sdata, struct session_data *sdata2, struct data *data, char *where_condition, struct config *cfg){
   int n;
   uint64 count=0, last_id=0, total_found=0;

   last_id = run_query(sdata, sdata2, data, where_condition, last_id, &n, cfg);
   count += n;

   total_found = get_total_found(sdata2);

   while(count < total_found){
      last_id = run_query(sdata, sdata2, data, where_condition, last_id, &n, cfg);
      count += n;
   }

}


int build_query_from_args(char *from, char *to, char *fromdomain, char *todomain, int minsize, int maxsize, unsigned long startdate, unsigned long stopdate){
   int where_condition=1;
   char s[SMALLBUFSIZE];

   if(exportall == 1){
      rc = append_string_to_buffer(&query, "SELECT `id`, `piler_id`, `digest`, `bodydigest` FROM ");
      rc += append_string_to_buffer(&query, SQL_METADATA_TABLE);
      rc += append_string_to_buffer(&query, " WHERE deleted=0 ");
      return rc;
   }

   snprintf(s, sizeof(s)-1, "SELECT DISTINCT `id`, `piler_id`, `digest`, `bodydigest` FROM %s WHERE deleted=0 ", SQL_MESSAGES_VIEW);

   rc = append_string_to_buffer(&query, s);

   if(from){
      if(where_condition) rc = append_string_to_buffer(&query, " AND ");

      rc += append_string_to_buffer(&query, "`from` IN (");
      rc += append_string_to_buffer(&query, from);
      rc += append_string_to_buffer(&query, ")");

      free(from);

      where_condition++;
   }

   if(to){
      if(where_condition) rc = append_string_to_buffer(&query, " AND ");

      rc += append_string_to_buffer(&query, "`to` IN (");
      rc += append_string_to_buffer(&query, to);
      rc += append_string_to_buffer(&query, ")");

      free(to);

      where_condition++;
   }

   if(fromdomain){
      if(where_condition) rc = append_string_to_buffer(&query, " AND ");

      rc += append_string_to_buffer(&query, "`fromdomain` IN (");
      rc += append_string_to_buffer(&query, fromdomain);
      rc += append_string_to_buffer(&query, ")");

      free(fromdomain);

      where_condition++;
   }


   if(todomain){
      if(where_condition) rc = append_string_to_buffer(&query, " AND ");

      rc += append_string_to_buffer(&query, "`todomain` IN (");
      rc += append_string_to_buffer(&query, todomain);
      rc += append_string_to_buffer(&query, ")");

      free(todomain);

      where_condition++;
   }

   if(minsize > 0){
      if(where_condition) rc = append_string_to_buffer(&query, " AND ");
      snprintf(s, sizeof(s)-1, " `size` >= %d", minsize);
      rc += append_string_to_buffer(&query, s);

      where_condition++;
   }


   if(maxsize > 0){
      if(where_condition) rc = append_string_to_buffer(&query, " AND ");
      snprintf(s, sizeof(s)-1, " `size` <= %d", maxsize);
      rc += append_string_to_buffer(&query, s);

      where_condition++;
   }


   if(startdate > 0){
      if(where_condition) rc = append_string_to_buffer(&query, " AND ");
      snprintf(s, sizeof(s)-1, " `sent` >= %ld", startdate);
      rc += append_string_to_buffer(&query, s);

      where_condition++;
   }


   if(stopdate > 0){
      if(where_condition) rc = append_string_to_buffer(&query, " AND ");
      snprintf(s, sizeof(s)-1, " `sent` <= %ld", stopdate);
      rc += append_string_to_buffer(&query, s);

      where_condition++;
   }


   rc += append_string_to_buffer(&query, " ORDER BY id ASC");

   return rc;
}


int export_emails_matching_to_query(struct session_data *sdata, struct data *data, char *s, struct config *cfg){
   FILE *f;
   uint64 id, n=0;
   char digest[SMALLBUFSIZE], bodydigest[SMALLBUFSIZE];
   char filename[SMALLBUFSIZE];
   int rc=0;


   if(prepare_sql_statement(sdata, &(data->stmt_generic), s) == ERR) return ERR;


   p_bind_init(data);

   if(p_exec_query(sdata, data->stmt_generic, data) == ERR) goto ENDE;



   p_bind_init(data);

   data->sql[data->pos] = (char *)&id; data->type[data->pos] = TYPE_LONGLONG; data->len[data->pos] = sizeof(uint64); data->pos++;
   data->sql[data->pos] = sdata->ttmpfile; data->type[data->pos] = TYPE_STRING; data->len[data->pos] = RND_STR_LEN; data->pos++;
   data->sql[data->pos] = &digest[0]; data->type[data->pos] = TYPE_STRING; data->len[data->pos] = sizeof(digest)-2; data->pos++;
   data->sql[data->pos] = &bodydigest[0]; data->type[data->pos] = TYPE_STRING; data->len[data->pos] = sizeof(bodydigest)-2; data->pos++;

   p_store_results(data->stmt_generic, data);

   while(p_fetch_results(data->stmt_generic) == OK){

      if(id > 0){

         if(dryrun == 0){

            snprintf(filename, sizeof(filename)-1, "%llu.eml", id);

            f = fopen(filename, "w");
            if(f){
               rc = retrieve_email_from_archive(sdata, data, f, cfg);
               fclose(f);

               n++;

               snprintf(sdata->filename, SMALLBUFSIZE-1, "%s", filename);

               make_digests(sdata, cfg);

               if(strcmp(digest, sdata->digest) == 0 && strcmp(bodydigest, sdata->bodydigest) == 0){
                  printf("exported: %10llu\r", n); fflush(stdout);
               }
               else
                  printf("verification FAILED. %s\n", filename);

            }
            else printf("cannot open: %s\n", filename);
         }
         else {
            printf("id:%llu\n", id);
         }

      }

   }

   p_free_results(data->stmt_generic);

ENDE:
   close_prepared_statement(data->stmt_generic);


   printf("\n");

   return rc;
}


int main(int argc, char **argv){
   int c, minsize=0, maxsize=0;
   size_t nmatch=0;
   unsigned long startdate=0, stopdate=0;
   char *configfile=CONFIG_FILE;
   char *to=NULL, *from=NULL, *todomain=NULL, *fromdomain=NULL, *where_condition=NULL;
   struct session_data sdata, sdata2;
   struct data data;
   struct config cfg;


   if(regcomp(&regexp, "^([\\+a-z0-9_\\.@\\-]+)$", REG_ICASE | REG_EXTENDED)){
      p_clean_exit("cannot compile rule!", 1);
   }


   while(1){

#ifdef _GNU_SOURCE
      static struct option long_options[] =
         {
            {"config",       required_argument,  0,  'c' },
            {"minsize",      required_argument,  0,  's' },
            {"maxsize",      required_argument,  0,  'S' },
            {"all",          no_argument,        0,  'A' },
            {"dry-run",      no_argument,        0,  'd' },
            {"dryrun",       no_argument,        0,  'd' },
            {"help",         no_argument,        0,  'h' },
            {"version",      no_argument,        0,  'v' },
            {"from",         required_argument,  0,  'f' },
            {"to",           required_argument,  0,  'r' },
            {"from-domain",  required_argument,  0,  'F' },
            {"to-domain",    required_argument,  0,  'R' },
            {"start-date",   required_argument,  0,  'a' },
            {"stop-date",    required_argument,  0,  'b' },
            {"where-condition", required_argument,  0,  'w' },
            {"max-matches",  required_argument,  0,  'm' },
            {"index-list",   required_argument,  0,  'i' },
            {0,0,0,0}
         };

      int option_index = 0;

      c = getopt_long(argc, argv, "c:s:S:f:r:F:R:a:b:w:m:i:Adhv?", long_options, &option_index);
#else
      c = getopt(argc, argv, "c:s:S:f:r:F:R:a:b:w:m:i:Adhv?");
#endif

      if(c == -1) break;

      switch(c){

         case 'c' :
                    configfile = optarg;
                    break;

         case 's' :
                    minsize = atoi(optarg);
                    break;

         case 'S' :
                    maxsize = atoi(optarg);
                    break;


         case 'A' :
                    exportall = 1;
                    break;

         case 'f' :

                    if(regexec(&regexp, optarg, nmatch, NULL, 0)){
                       printf("%s is not a valid email address\n", optarg);
                       break;
                    }

                    rc = append_email_to_buffer(&from, optarg);

                    break;

         case 'r' :

                    if(regexec(&regexp, optarg, nmatch, NULL, 0)){
                       printf("%s is not a valid email address\n", optarg);
                       break;
                    }

                    rc = append_email_to_buffer(&to, optarg);

                    break;

         case 'F' :

                    if(regexec(&regexp, optarg, nmatch, NULL, 0)){
                       printf("%s is not a valid domain name\n", optarg);
                       break;
                    }

                    rc = append_email_to_buffer(&fromdomain, optarg);

                    break;           

         case 'R' :

                    if(regexec(&regexp, optarg, nmatch, NULL, 0)){
                       printf("%s is not a valid domain name\n", optarg);
                       break;
                    }

                    rc = append_email_to_buffer(&todomain, optarg);

                    break;                     
                    
         case 'a' :
                    startdate = convert_time(optarg, 0, 0, 0);
                    break;


         case 'b' :
                    stopdate = convert_time(optarg, 23, 59, 59);
                    break;

         case 'w' :
                    where_condition = optarg;
                    break;

         case 'm' :
                    max_matches = atoi(optarg);
                    break;

         case 'i' :
                    index_list = optarg;
                    break;

         case 'd' :
                    dryrun = 1;
                    break;

         default  :
                    usage();
                    break;
      }

   }


   if(from == NULL && to == NULL && fromdomain == NULL && todomain == NULL && where_condition == NULL && startdate == 0 && stopdate == 0 && exportall == 0) usage();


   regfree(&regexp);

   if(!can_i_write_directory(NULL)) __fatal("cannot write current directory!");

   (void) openlog("pilerexport", LOG_PID, LOG_MAIL);


   cfg = read_config(configfile);


   if(read_key(&cfg)) p_clean_exit(ERR_READING_KEY, 1);


   init_session_data(&sdata, &cfg);


   if(open_database(&sdata, &cfg) == ERR){
      p_clean_exit("cannot connect to mysql server", 1);
   }


   if(where_condition){

      init_session_data(&sdata2, &cfg);

      strcpy(cfg.mysqlhost, "127.0.0.1");
      cfg.mysqlport = 9306;
      cfg.mysqlsocket[0] = '\0';

      if(open_database(&sdata2, &cfg) == ERR){
         p_clean_exit("cannot connect to 127.0.0.1:9306", 1);
      }

      export_emails_matching_id_list(&sdata, &sdata2, &data, where_condition, &cfg);

      close_database(&sdata2);
   }
   else {
      if(build_query_from_args(from, to, fromdomain, todomain, minsize, maxsize, startdate, stopdate) > 0) p_clean_exit("malloc problem building query", 1);
      export_emails_matching_to_query(&sdata, &data, query, &cfg);
      free(query);
   }

   close_database(&sdata);

   return 0;
}


