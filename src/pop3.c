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


int connect_to_pop3_server(int sd, char *server, char *username, char *password, int port, struct __data *data, int use_ssl){
   int n;
   char buf[MAXBUFSIZE];
   unsigned long host=0;
   struct sockaddr_in remote_addr;
   X509* server_cert;
   char *str;

   host = resolve_host(server);

   remote_addr.sin_family = AF_INET;
   remote_addr.sin_port = htons(port);
   remote_addr.sin_addr.s_addr = host;
   bzero(&(remote_addr.sin_zero),8);

   if(connect(sd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == -1){
      printf("connect()\n");
      return ERR;
   }


   if(use_ssl == 1){

      SSL_library_init();
      SSL_load_error_strings();

      data->ctx = SSL_CTX_new(SSLv3_client_method());
      CHK_NULL(data->ctx, "internal SSL error");

      data->ssl = SSL_new(data->ctx);
      CHK_NULL(data->ssl, "internal ssl error");

      SSL_set_fd(data->ssl, sd);
      n = SSL_connect(data->ssl);
      CHK_SSL(n, "internal ssl error");

      server_cert = SSL_get_peer_certificate(data->ssl);
      CHK_NULL(server_cert, "server cert error");

      str = X509_NAME_oneline(X509_get_subject_name(server_cert), 0, 0);
      CHK_NULL(str, "error in server cert");
      OPENSSL_free(str);

      str = X509_NAME_oneline(X509_get_issuer_name(server_cert), 0, 0);
      CHK_NULL(str, "error in server cert");
      OPENSSL_free(str);

      X509_free(server_cert);
   }


   n = recvtimeoutssl(sd, buf, sizeof(buf), 10, use_ssl, data->ssl);


   snprintf(buf, sizeof(buf)-1, "USER %s\r\n", username);

   write1(sd, buf, use_ssl, data->ssl);
   n = recvtimeoutssl(sd, buf, sizeof(buf), 10, use_ssl, data->ssl);


   snprintf(buf, sizeof(buf)-1, "PASS %s\r\n", password);

   write1(sd, buf, use_ssl, data->ssl);
   n = recvtimeoutssl(sd, buf, sizeof(buf), 10, use_ssl, data->ssl);

   if(strncmp(buf, "+OK", 3) == 0) return OK;

   return OK;
}


int process_pop3_emails(int sd, struct session_data *sdata, struct __data *data, int use_ssl, struct __config *cfg){
   int i, rc=ERR, n, messages=0, processed_messages=0, pos, readlen, fd, lastpos, nreads;
   char *p, buf[MAXBUFSIZE], filename[SMALLBUFSIZE];
   char aggrbuf[3*MAXBUFSIZE];

   snprintf(buf, sizeof(buf)-1, "STAT\r\n");
   n = write1(sd, buf, use_ssl, data->ssl);

   n = recvtimeoutssl(sd, buf, sizeof(buf), 10, use_ssl, data->ssl);

   if(strncmp(buf, "+OK ", 4) == 0){
      p = strchr(&buf[4], ' ');
      if(p){
         *p = '\0';
         messages = atoi(&buf[4]);
      }
   }
   else return ERR;


   printf("found %d messages\n", messages);

   if(messages <= 0) return rc;

   for(i=1; i<=messages; i++){
      processed_messages++;
      printf("processed: %7d\r", processed_messages); fflush(stdout);


      snprintf(buf, sizeof(buf)-1, "RETR %d\r\n", i);

      snprintf(filename, sizeof(filename)-1, "pop3-tmp-%d.txt", i);
      unlink(filename);

      fd = open(filename, O_CREAT|O_EXCL|O_RDWR|O_TRUNC, S_IRUSR|S_IWUSR);
      if(fd == -1){
         printf("cannot open: %s\n", filename);
         return rc;
      }

      n = write1(sd, buf, use_ssl, data->ssl);

      readlen = 0;
      pos = 0;
      nreads = 0;

      memset(aggrbuf, 0, sizeof(aggrbuf));
      lastpos = 0;


      while((n = recvtimeoutssl(sd, buf, sizeof(buf), 15, use_ssl, data->ssl)) > 0){
         nreads++;
         readlen += n;

         if(nreads == 1){

            if(strncmp(buf, "+OK ", 4) == 0){
               p = strchr(&buf[4], '\n');
               if(p){
                  *p = '\0';
                  pos = strlen(buf)+1;
                  *p = '\n';
               }
            }
            else { printf("error: %s", buf); return ERR; }

         }

         if(lastpos + 1 + n < sizeof(aggrbuf)){

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
            write(fd, aggrbuf, sizeof(buf));

            memmove(aggrbuf, aggrbuf+sizeof(buf), lastpos-sizeof(buf));
            lastpos -= sizeof(buf);

            memcpy(aggrbuf+lastpos, buf, n);
            lastpos += n;
         }

         if(is_last_complete_pop3_packet(aggrbuf, lastpos) == 1){
            write(fd, aggrbuf, lastpos-3);
            break;
         }

      } 

      close(fd);

      rc = import_message(filename, sdata, data, cfg);

      unlink(filename);
   }


   snprintf(buf, sizeof(buf)-1, "QUIT\r\n");
   n = write1(sd, buf, use_ssl, data->ssl);

   printf("\n");

   return OK;
}


