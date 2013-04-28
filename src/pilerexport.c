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
char *query=NULL;
regex_t regexp;



void usage(){
   printf("\nusage: pilerexport \n\n");
   printf("    [-c|--config <config file>] \n");
   printf("    -a|--start-date <YYYY.MM.DD> -b|--stop-date <YYYY.MM.DD> \n");
   printf("    -f|--from <email@address> -r|--to <email@address>\n");
   printf("    -s|--minsize <number> -S|--maxsize <number>\n");
   printf("    -A|--all  -d|--dryrun \n");
   printf("\n    use -A if you don't want to specify the start/stop time nor any from/to address\n\n");

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


int export_emails_matching_to_query(struct session_data *sdata, struct __data *data, char *s, struct __config *cfg){
   MYSQL_RES *res;
   MYSQL_ROW row;
   FILE *f;
   uint64 id, n=0;
   char *digest=NULL, *bodydigest=NULL;
   char filename[SMALLBUFSIZE];
   int rc=0;


   rc = mysql_real_query(&(sdata->mysql), s, strlen(s));

   if(rc == 0){
      res = mysql_store_result(&(sdata->mysql));
      if(res){
         while((row = mysql_fetch_row(res))){

            id = strtoull(row[0], NULL, 10);
            if(id > 0){
               snprintf(sdata->ttmpfile, SMALLBUFSIZE-1, "%s", (char*)row[1]);
               digest = (char*)row[2];
               bodydigest = (char*)row[3];

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
         mysql_free_result(res);
      }
      else rc = 1;
   }

   printf("\n");

   return rc;
}


int main(int argc, char **argv){
   int c, rc, exportall=0, minsize=0, maxsize=0;
   int where_condition=0;
   size_t nmatch=0;
   unsigned long startdate=0, stopdate=0;
   char *configfile=CONFIG_FILE;
   char *to=NULL, *from=NULL;
   char s[SMALLBUFSIZE];
   struct session_data sdata;
   struct __data data;
   struct __config cfg;


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
            {"start-date",   required_argument,  0,  'a' },
            {"stop-date",    required_argument,  0,  'b' },
            {"id",           required_argument,  0,  'i' },
            {0,0,0,0}
         };

      int option_index = 0;

      c = getopt_long(argc, argv, "c:s:S:f:r:a:b:i:Adhv?", long_options, &option_index);
#else
      c = getopt(argc, argv, "c:s:S:f:r:a:b:i:Adhv?");
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


         case 'a' :
                    startdate = convert_time(optarg, 0, 0, 0);
                    break;


         case 'b' :
                    stopdate = convert_time(optarg, 23, 59, 59);
                    break;


         case 'd' :
                    dryrun = 1;
                    break;


         default  :
                    usage();
                    break;
      }

   }


   if(from == NULL && to == NULL && startdate == 0 && stopdate == 0 && exportall == 0) usage();


   regfree(&regexp);

   (void) openlog("pilerexport", LOG_PID, LOG_MAIL);

   if(exportall == 1){
      rc = append_string_to_buffer(&query, "SELECT `id`, `piler_id`, `digest`, `bodydigest` FROM ");
      rc += append_string_to_buffer(&query, SQL_METADATA_TABLE);
      goto GO;
   }

   snprintf(s, sizeof(s)-1, "SELECT DISTINCT `id`, `piler_id`, `digest`, `bodydigest` FROM %s WHERE ", SQL_MESSAGES_VIEW);

   rc = append_string_to_buffer(&query, s);

   if(from){
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


GO:
   if(rc) p_clean_exit("malloc problem building query", 1);


   cfg = read_config(configfile);


   if(read_key(&cfg)) p_clean_exit(ERR_READING_KEY, 1);


   init_session_data(&sdata, &cfg);


   if(open_database(&sdata, &cfg) == ERR){
      p_clean_exit("cannot connect to mysql server", 1);
   }



   rc = export_emails_matching_to_query(&sdata, &data, query, &cfg);

   free(query);


   close_database(&sdata);

   return 0;
}


