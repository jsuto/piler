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
#include <piler.h>


extern char *optarg;
extern int optind;

char *query=NULL;
regex_t regexp;



void usage(){
   printf("usage: .... \n");
   exit(0);
}


void clean_exit(char *msg, int rc){
   if(msg) printf("error: %s\n", msg);

   if(query) free(query);

   exit(rc);
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


int export_emails_matching_to_query(struct session_data *sdata, char *s, struct __config *cfg){
   MYSQL_RES *res;
   MYSQL_ROW row;
   FILE *f;
   uint64 id;
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


               snprintf(filename, sizeof(filename)-1, "%llu.eml", id);

               f = fopen(filename, "w");
               if(f){
                  rc = retrieve_email_from_archive(sdata, f, cfg);
                  fclose(f);

                  snprintf(sdata->filename, SMALLBUFSIZE-1, "%s", filename);

                  make_digests(sdata, cfg);

                  if(strcmp(digest, sdata->digest) == 0 && strcmp(bodydigest, sdata->bodydigest) == 0)
                     printf("exported %s, verification: OK\n", filename);
                  else
                     printf("exported %s, verification: FAILED\n", filename);

               }
               else printf("cannot open: %s\n", filename);

            }
         }
         mysql_free_result(res);
      }
      else rc = 1;
   }

   return rc;
}


int main(int argc, char **argv){
   int i, rc, exportall=0, minsize=0, maxsize=0;
   int where_condition=0;
   size_t nmatch=0;
   //unsigned long startdate=0, stopdate=0;
   char *configfile=CONFIG_FILE;
   char *to=NULL, *from=NULL;
   char s[SMALLBUFSIZE];
   struct session_data sdata;
   struct __config cfg;


   if(regcomp(&regexp, "^([\\+a-z0-9_\\.@\\-]+)$", REG_ICASE | REG_EXTENDED)){
      clean_exit("cannot compile rule!", 1);
   }


   while((i = getopt(argc, argv, "c:s:S:f:r:ah?")) > 0){

       switch(i){

         case 'c' :
                    configfile = optarg;
                    break;

         case 's' :
                    maxsize = atoi(optarg);
                    break;

         case 'S' :
                    minsize = atoi(optarg);
                    break;


         case 'a' :
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



         case 'h' :
         case '?' :
                    usage();
                    break;


         default  : 
                    break;
       }
   }


   regfree(&regexp);


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
   }


   if(maxsize > 0){
      if(where_condition) rc = append_string_to_buffer(&query, " AND ");
      snprintf(s, sizeof(s)-1, " `size` <= %d", maxsize);
      rc += append_string_to_buffer(&query, s);
   }

   rc += append_string_to_buffer(&query, " ORDER BY id ASC");

   if(rc) clean_exit("malloc problem building query", 1);

   printf("query: *%s*\n", query);




   cfg = read_config(configfile);


   if(read_key(&cfg)) clean_exit(ERR_READING_KEY, 1);


   init_session_data(&sdata);


   mysql_init(&(sdata.mysql));
   mysql_options(&(sdata.mysql), MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&cfg.mysql_connect_timeout);
   if(mysql_real_connect(&(sdata.mysql), cfg.mysqlhost, cfg.mysqluser, cfg.mysqlpwd, cfg.mysqldb, cfg.mysqlport, cfg.mysqlsocket, 0) == 0){
      clean_exit("cannot connect to mysql server", 1);
   }

   mysql_real_query(&(sdata.mysql), "SET NAMES utf8", strlen("SET NAMES utf8"));
   mysql_real_query(&(sdata.mysql), "SET CHARACTER SET utf8", strlen("SET CHARACTER SET utf8"));


   rc = export_emails_matching_to_query(&sdata, query, &cfg);

   free(query);


   mysql_close(&(sdata.mysql));

   return 0;
}


