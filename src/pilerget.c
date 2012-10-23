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


int main(int argc, char **argv){
   int rc, readkey=1;
   struct session_data sdata;
   struct __config cfg;


   if(argc < 2){
      printf("usage: %s <piler-id>\n", argv[0]);
      exit(1);
   }

   (void) openlog("pilerget", LOG_PID, LOG_MAIL);

   cfg = read_config(CONFIG_FILE);

   if(argc >= 3) readkey = 0;

   if(readkey == 1 && read_key(&cfg)){
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


   snprintf(sdata.ttmpfile, SMALLBUFSIZE-1, "%s", argv[1]);
   snprintf(sdata.filename, SMALLBUFSIZE-1, "%s", sdata.ttmpfile);
   rc = retrieve_email_from_archive(&sdata, stdout, &cfg);


   mysql_close(&(sdata.mysql));

   return 0;
}


