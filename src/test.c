/*
 * test.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>
#include <piler.h>


int main(int argc, char **argv){
   int i, rc;
   struct stat st;
   struct session_data sdata;
   struct _state state;
   struct __config cfg;

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
   sdata.tot_len = st.st_size;
   memset(sdata.rcptto[0], 0, SMALLBUFSIZE);
   snprintf(sdata.ttmpfile, SMALLBUFSIZE-1, "%s", argv[1]);

   state = parse_message(&sdata, &cfg);

   printf("message-id: %s\n", state.message_id);
   printf("from: *%s*\n", state.b_from);
   printf("to: *%s*\n", state.b_to);
   printf("subject: *%s*\n", state.b_subject);
   printf("body: *%s*\n", state.b_body);

   make_body_digest(&sdata, &cfg);

   printf("body digest: %s\n", sdata.bodydigest);

   for(i=0; i<state.n_attachments; i++){
      printf("i:%d, name=*%s*, type: *%s*, size: %d\n", i, state.attachments[i].filename, state.attachments[i].type, state.attachments[i].size);
   }

   printf("\n\n");

   return 0;
}
