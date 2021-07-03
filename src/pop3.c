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


int connect_to_pop3_server(struct data *data){
   char buf[MAXBUFSIZE];

   if(data->net->use_ssl == 1){
      init_ssl_to_server(data);
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
   char buf[MAXBUFSIZE];

   data->import->total_messages = 0;

   snprintf(buf, sizeof(buf)-1, "STAT\r\n");
   write1(data->net, buf, strlen(buf));

   recvtimeoutssl(data->net, buf, sizeof(buf));

   if(strncmp(buf, "+OK ", 4) == 0){
      char *p = strchr(&buf[4], ' ');
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
   char *p, buf[MAXBUFSIZE], savedbuf[MAXBUFSIZE], copybuf[2*MAXBUFSIZE];

   data->import->processed_messages++;

   snprintf(data->import->filename, SMALLBUFSIZE-1, "pop3-tmp-%d-%d.txt", getpid(), i);
   unlink(data->import->filename);

   int fd = open(data->import->filename, O_CREAT|O_EXCL|O_RDWR|O_TRUNC, S_IRUSR|S_IWUSR);
   if(fd == -1){
      printf("cannot open: %s\n", data->import->filename);
      return ERR;
   }

   memset(savedbuf, 0, sizeof(savedbuf));

   snprintf(buf, sizeof(buf)-1, "RETR %d\r\n", i);
   write1(data->net, buf, strlen(buf));

   int nlines = 0;
   int endofmessage = 0;
   int savedlen = 0;
   int n = 0;

   while((n = recvtimeoutssl(data->net, buf, sizeof(buf))) > 0){
      if(savedlen){
         memset(copybuf, 0, sizeof(copybuf));
         memcpy(copybuf, savedbuf, savedlen);
         memcpy(&copybuf[savedlen], buf, n);

         savedlen = 0;
         memset(savedbuf, 0, sizeof(savedbuf));

         p = &copybuf[0];
      } else {
         p = &buf[0];
      }

      int puflen=0;
      int rc=OK;
      do {
         char puf[MAXBUFSIZE];

         puflen = read_one_line(p, '\n', puf, sizeof(puf)-1, &rc);
         nlines++;

         if(nlines == 1){
            if(strncmp(puf, "+OK", 3)){
               printf("error: %s", puf);
               return ERR;
            }
         } else {
            if(puf[puflen-1] == '\n'){
               if(puflen == 3 && puf[0] == '.' && puf[1] == '\r' && puf[2] == '\n'){
                  endofmessage = 1;
                  break;
               }

               int dotstuff = 0;
               if(puf[0] == '.' && puf[1] != '\r' && puf[1] != '\n') dotstuff = 1;

               if(write(fd, &puf[dotstuff], puflen-dotstuff) == -1) printf("ERROR: writing to fd\n");

            } else if(puflen > 0) {
               savedlen = puflen;
               snprintf(savedbuf, sizeof(savedbuf)-1, "%s", puf);
            }
         }

         p += puflen;

      } while(puflen > 0);

      if(endofmessage){
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
   char buf[MAXBUFSIZE];

   data->import->processed_messages = 0;

   get_number_of_total_messages(data);

   if(data->quiet == 0) printf("found %d messages\n", data->import->total_messages);

   if(data->import->total_messages <= 0) return;

   for(int i=data->import->start_position; i<=data->import->total_messages; i++){
      if(pop3_download_email(data, i) == OK){
         if(data->quiet == 0){ printf("processed: %7d [%3d%%]\r", data->import->processed_messages, 100*i/data->import->total_messages); fflush(stdout); }

         if(data->import->dryrun == 0){
            int rc = import_message(sdata, data, cfg);

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
