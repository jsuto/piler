/*
 * test.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>
#include <syslog.h>
#include <piler.h>


int main(int argc, char **argv){
   int i;
   struct stat st;
   struct session_data sdata;
   struct _state state;
   struct __config cfg;
   struct __data data;
   struct import import;

   srand(getpid());

   if(argc < 2){
      fprintf(stderr, "usage: %s <message>\n", argv[0]);
      exit(1);
   }

   if(!can_i_write_current_directory()) __fatal("cannot write current directory!");

   if(stat(argv[1], &st) != 0){
      fprintf(stderr, "%s is not found\n", argv[1]);
      return 0;
   }

   (void) openlog("test", LOG_PID, LOG_MAIL);

   cfg = read_config(CONFIG_FILE);

   if(open_database(&sdata, &cfg) == ERR) return 0;

   setlocale(LC_MESSAGES, cfg.locale);
   setlocale(LC_CTYPE, cfg.locale);

   import.extra_recipient = NULL;

   data.import = &import;

   data.folder = 0;
   data.recursive_folder_names = 0;

   inithash(data.mydomains);

   init_session_data(&sdata, &cfg);
 
   sdata.sent = 0;
   sdata.delivered = 0;
   sdata.tot_len = st.st_size;
   sdata.import = 1;

   snprintf(sdata.ttmpfile, SMALLBUFSIZE-1, "%s", argv[1]);
   snprintf(sdata.filename, SMALLBUFSIZE-1, "%s", argv[1]);
   snprintf(sdata.tmpframe, SMALLBUFSIZE-1, "%s.m", argv[1]);

   state = parse_message(&sdata, 1, &data, &cfg);

   post_parse(&sdata, &state, &cfg);

   printf("message-id: %s / %s\n", state.message_id, state.message_id_hash);
   printf("from: *%s (%s)*\n", state.b_from, state.b_from_domain);
   printf("to: *%s (%s)*\n", state.b_to, state.b_to_domain);
   printf("reference: *%s*\n", state.reference);
   printf("subject: *%s*\n", state.b_subject);
   printf("body: *%s*\n", state.b_body);

   printf("sent: %ld, delivered-date: %ld\n", sdata.sent, sdata.delivered);

   make_digests(&sdata, &cfg);

   printf("hdr len: %d\n", sdata.hdr_len);

   printf("body digest: %s\n", sdata.bodydigest);

   clearhash(data.mydomains);

   for(i=1; i<=state.n_attachments; i++){
      printf("i:%d, name=*%s*, type: *%s*, size: %d, int.name: %s, digest: %s\n", i, state.attachments[i].filename, state.attachments[i].type, state.attachments[i].size, state.attachments[i].internalname, state.attachments[i].digest);
      unlink(state.attachments[i].internalname);
   }

   unlink(sdata.tmpframe);

   printf("attachments:%s\n", sdata.attachments);

   close_database(&sdata);

   return 0;
}


