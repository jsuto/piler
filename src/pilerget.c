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
   int readkey=1;
   struct session_data sdata;
   struct __data data;
   struct __config cfg;


   if(argc < 2){
      printf("usage: %s <piler-id>\n", argv[0]);
      exit(1);
   }

   (void) openlog("pilerget", LOG_PID, LOG_MAIL);

   cfg = read_config(CONFIG_FILE);

   if(argc >= 3){
      readkey = 0;
      cfg.encrypt_messages = 0;
   }

   if(readkey == 1 && read_key(&cfg)){
      printf("%s\n", ERR_READING_KEY);
      return 1;
   }


   if(open_database(&sdata, &cfg) == ERR) return 0;


   snprintf(sdata.ttmpfile, SMALLBUFSIZE-1, "%s", argv[1]);
   snprintf(sdata.filename, SMALLBUFSIZE-1, "%s", sdata.ttmpfile);
   retrieve_email_from_archive(&sdata, &data, stdout, &cfg);


   close_database(&sdata);

   return 0;
}


