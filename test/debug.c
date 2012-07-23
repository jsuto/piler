#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <piler.h>


int main(int argc, char **argv){
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


   init_session_data(&sdata);
 
   sdata.sent = 0;
   sdata.tot_len = st.st_size;

   snprintf(sdata.ttmpfile, SMALLBUFSIZE-1, "%s", argv[1]);
   snprintf(sdata.filename, SMALLBUFSIZE-1, "%s", argv[1]);
   snprintf(sdata.tmpframe, SMALLBUFSIZE-1, "%s.m", argv[1]);

   cfg.debug = 1;

   state = parse_message(&sdata, 0, &cfg);
   post_parse(&sdata, &state, &cfg);

   printf("message-id: %s\n", state.message_id);
   printf("from: *%s (%s)*\n", state.b_from, state.b_from_domain);
   printf("to: *%s (%s)*\n", state.b_to, state.b_to_domain);
   printf("reference: *%s*\n", state.reference);
   printf("subject: *%s*\n", state.b_subject);
   printf("body: *%s*\n", state.b_body);

   printf("sent: %ld\n", sdata.sent);

   make_digests(&sdata, &cfg);

   return 0;
}
