/*
 * pilerget.c, SJ
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



uint64 get_id_by_piler_id(struct session_data *sdata, char *digest, char *bodydigest, struct __config *cfg){
   char s[SMALLBUFSIZE];
   MYSQL_STMT *stmt;
   MYSQL_BIND bind[3];
   unsigned long len=0;
   uint64 id=0;

   memset(digest, 0, 2*DIGEST_LENGTH+1);
   memset(bodydigest, 0, 2*DIGEST_LENGTH+1);

   stmt = mysql_stmt_init(&(sdata->mysql));
   if(!stmt){
      goto ENDE;
   }

   snprintf(s, SMALLBUFSIZE-1, "SELECT `id`,`digest`,`bodydigest` FROM %s WHERE piler_id=?", SQL_METADATA_TABLE);


   if(mysql_stmt_prepare(stmt, s, strlen(s))){
      goto ENDE;
   }

   memset(bind, 0, sizeof(bind));

   bind[0].buffer_type = MYSQL_TYPE_STRING;
   bind[0].buffer = sdata->ttmpfile;
   bind[0].is_null = 0;
   len = strlen(sdata->ttmpfile); bind[0].length = &len;


   if(mysql_stmt_bind_param(stmt, bind)){
      goto ENDE;
   }


   if(mysql_stmt_execute(stmt)){
      goto ENDE;
   }


   memset(bind, 0, sizeof(bind));

   bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
   bind[0].buffer = (char *)&id;
   bind[0].is_null = 0;
   bind[0].length = 0;

   bind[1].buffer_type = MYSQL_TYPE_STRING;
   bind[1].buffer = digest;
   bind[1].buffer_length = 2*DIGEST_LENGTH+1;
   bind[1].is_null = 0;
   bind[1].length = &len;

   bind[2].buffer_type = MYSQL_TYPE_STRING;
   bind[2].buffer = bodydigest;
   bind[2].buffer_length = 2*DIGEST_LENGTH+1;
   bind[2].is_null = 0;
   bind[2].length = &len;

   if(mysql_stmt_bind_result(stmt, bind)){
      goto ENDE;
   }


   if(mysql_stmt_store_result(stmt)){
      goto ENDE;
   }

   mysql_stmt_fetch(stmt);

   mysql_stmt_close(stmt);

ENDE:

   return id;
}


int main(int argc, char **argv){
   int rc;
   uint64 id;
   char filename[SMALLBUFSIZE], digest[2*DIGEST_LENGTH+1], bodydigest[2*DIGEST_LENGTH+1];
   FILE *f;
   struct session_data sdata;
   struct __config cfg;


   if(argc < 2){
      printf("usage: %s <piler-id>\n", argv[0]);
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
      printf("cannot connect to mysql server\n");
      return 0;
   }

   mysql_real_query(&(sdata.mysql), "SET NAMES utf8", strlen("SET NAMES utf8"));
   mysql_real_query(&(sdata.mysql), "SET CHARACTER SET utf8", strlen("SET CHARACTER SET utf8"));


   if(argv[1][0] == '-'){

      memset(sdata.ttmpfile, 0, sizeof(sdata.ttmpfile));

      while((rc = read(0, sdata.ttmpfile, RND_STR_LEN+1)) > 0){
         snprintf(sdata.filename, SMALLBUFSIZE-1, "%s", sdata.ttmpfile);

         trimBuffer(sdata.ttmpfile);

         id = get_id_by_piler_id(&sdata, &digest[0], &bodydigest[0], &cfg);

         if(id > 0){
            snprintf(filename, sizeof(filename)-1, "%llu.eml", id);
            f = fopen(filename, "w");
            if(f){
               rc = retrieve_email_from_archive(&sdata, f, &cfg);
               fclose(f);

               snprintf(sdata.ttmpfile, SMALLBUFSIZE-1, "%s", filename);
               make_digests(&sdata, &cfg);

               if(strcmp(digest, sdata.digest) == 0 && strcmp(bodydigest, sdata.bodydigest) == 0)
                  printf("exported %s, verification: OK\n", sdata.ttmpfile);
               else
                  printf("exported %s, verification: FAILED\n", sdata.ttmpfile);

            }
            else printf("cannot open: %s\n", filename);
         }
         else printf("%s was not found in archive\n", sdata.ttmpfile);

         memset(sdata.ttmpfile, 0, sizeof(sdata.ttmpfile));

      }

   }
   else {
      snprintf(sdata.ttmpfile, SMALLBUFSIZE-1, "%s", argv[1]);
      snprintf(sdata.filename, SMALLBUFSIZE-1, "%s", sdata.ttmpfile);
      rc = retrieve_email_from_archive(&sdata, stdout, &cfg);
   }


   mysql_close(&(sdata.mysql));

   return 0;
}


