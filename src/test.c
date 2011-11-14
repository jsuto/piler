/*
 * test.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <locale.h>
#include <piler.h>


int main(int argc, char **argv){
   int i, rc;
   struct timezone tz;
   struct timeval tv_spam_start, tv_spam_stop;
   struct session_data sdata;
   struct _state state;
   struct __config cfg;
   struct stat st;

   if(argc < 2){
      fprintf(stderr, "usage: %s <message>\n", argv[0]);
      exit(1);
   }

   if(stat(argv[1], &st) != 0){
      fprintf(stderr, "%s is not found\n", argv[1]);
      return 0;
   }

   cfg = read_config(CONFIG_FILE);

   printf("locale: %s\n", setlocale(LC_MESSAGES, cfg.locale));
   setlocale(LC_CTYPE, cfg.locale);

   rc = 0;

   sdata.num_of_rcpt_to = -1;
   time(&(sdata.now));
   sdata.sent = 0;
   memset(sdata.rcptto[0], 0, SMALLBUFSIZE);
   snprintf(sdata.ttmpfile, SMALLBUFSIZE-1, "%s", argv[1]);

   state = parseMessage(&sdata, &cfg);

   printf("from: %s\n", state.b_from);
   printf("to: %s\n", state.b_to);
   printf("subject: %s\n", state.b_subject);
   printf("message-id: %s\n", state.message_id);
   printf("body: %s\n", state.b_body);


   printf("body digest: %s\n", sdata.bodydigest);

   for(i=0; i<state.n_attachments; i++){
      printf("i:%d, name=*%s*, type: *%s*, size: %d\n", i, state.attachments[i].filename, state.attachments[i].type, state.attachments[i].size);
   }

   gettimeofday(&tv_spam_start, &tz);

   freeState(&state);

   gettimeofday(&tv_spam_stop, &tz);

   return 0;
}
