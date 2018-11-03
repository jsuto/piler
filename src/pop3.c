/*
 * pop3.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <fcntl.h>
#include <ctype.h>
#include <syslog.h>
#include <unistd.h>
#include <limits.h>
#include <piler.h>


int is_last_complete_pop3_packet(char *s, int len){

   if(*(s+len-5) == '\r' && *(s+len-4) == '\n' && *(s+len-3) == '.' && *(s+len-2) == '\r' && *(s+len-1) == '\n'){
      return 1;
   }

   return 0;
}


int connect_to_pop3_server(struct data *data){
   char buf[MAXBUFSIZE];

   if(data->net->use_ssl == 1){
      init_ssl(data);
   }

   recvtimeoutssl(data->net, buf, sizeof(buf));


   snprintf(buf, sizeof(buf)-1, "USER %s\r\n", data->import->username);

   write1(data->net, buf, strlen(buf));
   recvtimeoutssl(data->net, buf, sizeof(buf));


   snprintf(buf, sizeof(buf)-1, "PASS %s\r\n", data->import->password);

   write1(data->net, buf, strlen(buf));
   recvtimeoutssl(data->net, buf, sizeof(buf));

   if(strncmp(buf, "+OK", 3) == 0) return OK;

   printf("error: %s", buf);

   return ERR;
}


void get_number_of_total_messages(struct data *data){
   char *p, buf[MAXBUFSIZE];

   data->import->total_messages = 0;

   snprintf(buf, sizeof(buf)-1, "STAT\r\n");
   write1(data->net, buf, strlen(buf));

   recvtimeoutssl(data->net, buf, sizeof(buf));

   if(strncmp(buf, "+OK ", 4) == 0){
      p = strchr(&buf[4], ' ');
      if(p){
         *p = '\0';
         data->import->total_messages = atoi(&buf[4]);
      }
   }
   else {
      printf("ERROR: '%s'", buf);
   }
}


int pop3_download_email(struct data *data, int i){
   int n, fd, pos=0, readlen=0, lastpos=0, nreads=0;
   char *p, buf[MAXBUFSIZE];
   char aggrbuf[3*MAXBUFSIZE];

   data->import->processed_messages++;

   snprintf(data->import->filename, SMALLBUFSIZE-1, "pop3-tmp-%d-%d.txt", getpid(), i);
   unlink(data->import->filename);

   fd = open(data->import->filename, O_CREAT|O_EXCL|O_RDWR|O_TRUNC, S_IRUSR|S_IWUSR);
   if(fd == -1){
      printf("cannot open: %s\n", data->import->filename);
      return ERR;
   }

   snprintf(buf, sizeof(buf)-1, "RETR %d\r\n", i);
   write1(data->net, buf, strlen(buf));

   memset(aggrbuf, 0, sizeof(aggrbuf));

   while((n = recvtimeoutssl(data->net, buf, sizeof(buf))) > 0){
      nreads++;
      readlen += n;

      if(nreads == 1){

         if(strncmp(buf, "+OK", 3) == 0){
            p = strchr(&buf[3], '\n');
            if(p){
               *p = '\0';
               pos = strlen(buf)+1;
               *p = '\n';
            }
         }
         else { printf("error: %s", buf); return ERR; }

      }

      if((uint)(lastpos + 1 + n) < sizeof(aggrbuf)){

         if(nreads == 1){
            memcpy(aggrbuf+lastpos, buf+pos, n-pos);
            lastpos += n-pos;
         }
         else {
            memcpy(aggrbuf+lastpos, buf, n);
            lastpos += n;
         }
      }
      else {
         if(write(fd, aggrbuf, sizeof(buf)) == -1) printf("ERROR: writing to fd\n");

         memmove(aggrbuf, aggrbuf+sizeof(buf), lastpos-sizeof(buf));
         lastpos -= sizeof(buf);

         memcpy(aggrbuf+lastpos, buf, n);
         lastpos += n;
      }

      if(is_last_complete_pop3_packet(aggrbuf, lastpos) == 1){
         if(write(fd, aggrbuf, lastpos-3) == -1) printf("ERROR: writing to fd\n");
         break;
      }
   }

   close(fd);

   return OK;
}


void pop3_delete_message(struct data *data, int i){
   char buf[SMALLBUFSIZE];

   snprintf(buf, sizeof(buf)-1, "DELE %d\r\n", i);
   write1(data->net, buf, strlen(buf));
   recvtimeoutssl(data->net, buf, sizeof(buf));
}


void process_pop3_emails(struct session_data *sdata, struct data *data, struct config *cfg){
   int i=0, rc=ERR;
   char buf[MAXBUFSIZE];

   data->import->processed_messages = 0;

   get_number_of_total_messages(data);

   if(data->quiet == 0) printf("found %d messages\n", data->import->total_messages);

   if(data->import->total_messages <= 0) return;

   for(i=data->import->start_position; i<=data->import->total_messages; i++){
      if(pop3_download_email(data, i) == OK){
         if(data->quiet == 0){ printf("processed: %7d [%3d%%]\r", data->import->processed_messages, 100*i/data->import->total_messages); fflush(stdout); }

         if(data->import->dryrun == 0){
            rc = import_message(sdata, data, cfg);

            if(data->import->remove_after_import == 1 && rc == OK){
               pop3_delete_message(data, i);
            }
         }
      }

      if(data->import->download_only == 0) unlink(data->import->filename);

      /* whether to quit after processing a batch of messages */

      if(data->import->batch_processing_limit > 0 && data->import->processed_messages >= data->import->batch_processing_limit){
         break;
      }
   }


   snprintf(buf, sizeof(buf)-1, "QUIT\r\n");
   write1(data->net, buf, strlen(buf));

   if(data->quiet == 0) printf("\n");
}
