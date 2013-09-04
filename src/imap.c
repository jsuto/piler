/*
 * imap.c, SJ
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


int get_message_length_from_imap_answer(char *s, int *_1st_line_bytes){
   char *p, *q;
   int len=0;


   p = strstr(s, "\r\n");
   if(!p){
      printf("invalid reply: %s", s);
      return len;
   }

   *p = '\0';

   //*_1st_line_bytes = strlen(s)+2;
   *_1st_line_bytes = p-s+2;

   if(*(p-1) == '}') *(p-1) = '\0';


   q = strchr(s, '{');
   if(q){
      q++;
      len = atoi(q);
   }

   *(p-1) = '}';
   *p = '\r';

   return len;
}


int is_last_complete_packet(char *s, int len, char *tagok, char *tagbad, int *pos){
   char *p;

   *pos = 0;

   if(*(s+len-2) == '\r' && *(s+len-1) == '\n'){
      if((p = strstr(s, tagok))){
         *pos = p - s;
         if(*pos > 3) *pos -= 2;
         return 1;
      }
      if(strstr(s, tagbad)) return 1;
   }

   return 0;
}


int read_response(int sd, char *buf, int buflen, char *tagok, struct __data *data, int use_ssl){
   int n, len=0;
   char puf[MAXBUFSIZE];

   memset(buf, 0, buflen);

   while(!strstr(buf, tagok)){
      n = recvtimeoutssl(sd, puf, sizeof(puf), 10, use_ssl, data->ssl);
      if(n + len < buflen) strncat(buf, puf, n);
      else return 0;

      len += n;
   }

   return 1;
}


int process_imap_folder(int sd, int *seq, char *folder, struct session_data *sdata, struct __data *data, int use_ssl, int dryrun, struct __config *cfg){
   int rc=ERR, i, n, pos, endpos, messages=0, len, readlen, fd, lastpos, nreads;
   char *p, tag[SMALLBUFSIZE], tagok[SMALLBUFSIZE], tagbad[SMALLBUFSIZE], buf[MAXBUFSIZE], filename[SMALLBUFSIZE];
   char aggrbuf[3*MAXBUFSIZE];

   /* imap cmd: SELECT */

   snprintf(tag, sizeof(tag)-1, "A%d", *seq); snprintf(tagok, sizeof(tagok)-1, "\r\nA%d OK", (*seq)++);
   snprintf(buf, sizeof(buf)-1, "%s SELECT \"%s\"\r\n", tag, folder);

   n = write1(sd, buf, strlen(buf), use_ssl, data->ssl);
   read_response(sd, buf, sizeof(buf), tagok, data, use_ssl);

   if(!strstr(buf, tagok)){
      trimBuffer(buf);
      printf("error: %s\n", buf);
      return rc;
   }

   p = strstr(buf, " EXISTS");
   if(p){
      *p = '\0';
      p = strrchr(buf, ' ');
      if(p){
         while(!isdigit(*p)){ p++; }
         messages = atoi(p);
      }
   }

   printf("found %d messages\n", messages);

   if(messages <= 0) return OK;

   data->import->total_messages += messages;

   for(i=1; i<=messages; i++){
      data->import->processed_messages++;
      printf("processed: %7d\r", data->import->processed_messages); fflush(stdout);

      snprintf(tag, sizeof(tag)-1, "A%d", *seq);
      snprintf(tagok, sizeof(tagok)-1, "\r\nA%d OK", (*seq)++);
      snprintf(tagbad, sizeof(tagbad)-1, "\r\n%s BAD", tag);

      snprintf(buf, sizeof(buf)-1, "%s FETCH %d (BODY.PEEK[])\r\n", tag, i);

      snprintf(filename, sizeof(filename)-1, "imap-%d.txt", data->import->processed_messages);
      unlink(filename);

      fd = open(filename, O_CREAT|O_EXCL|O_RDWR|O_TRUNC, S_IRUSR|S_IWUSR);
      if(fd == -1){
         printf("cannot open: %s\n", filename);
         return rc;
      }


      n = write1(sd, buf, strlen(buf), use_ssl, data->ssl);

      readlen = 0;
      pos = 0;
      len = 0;
      nreads = 0;
      endpos = 0;

      memset(aggrbuf, 0, sizeof(aggrbuf));
      lastpos = 0;


      while((n = recvtimeoutssl(sd, buf, sizeof(buf), 15, use_ssl, data->ssl)) > 0){
         nreads++;
         readlen += n;

         if(nreads == 1){
            len = get_message_length_from_imap_answer(buf, &pos);

            if(len < 10){
               printf("%d: too short message! %s\n", i, buf);
               break;
            }
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

         if(is_last_complete_packet(aggrbuf, lastpos, tagok, tagbad, &endpos) == 1){
            write(fd, aggrbuf, lastpos-(lastpos-endpos));
            break;
         }

      } 

      close(fd);

      if(dryrun == 0) rc = import_message(filename, sdata, data, cfg);
      else rc = OK;

      if(rc == ERR) printf("error importing '%s'\n", filename);
      else unlink(filename);
   }

   printf("\n");

   return OK;
}


int connect_to_imap_server(int sd, int *seq, char *username, char *password, int port, struct __data *data, int use_ssl){
   int n;
   char tag[SMALLBUFSIZE], tagok[SMALLBUFSIZE], buf[MAXBUFSIZE];
   X509* server_cert;
   char *str;


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

      //printf("Cipher: %s\n", SSL_get_cipher(data->ssl));

      server_cert = SSL_get_peer_certificate(data->ssl);
      CHK_NULL(server_cert, "server cert error");

      //if(verbose){
         str = X509_NAME_oneline(X509_get_subject_name(server_cert), 0, 0);
         CHK_NULL(str, "error in server cert");
         printf("server cert:\n\t subject: %s\n", str);
         OPENSSL_free(str);

         str = X509_NAME_oneline(X509_get_issuer_name(server_cert), 0, 0);
         CHK_NULL(str, "error in server cert");
         printf("\t issuer: %s\n\n", str);
         OPENSSL_free(str);
      //}

      X509_free(server_cert);
   }


   n = recvtimeoutssl(sd, buf, sizeof(buf), 10, use_ssl, data->ssl);

   /* imap cmd: CAPABILITY */

   snprintf(tag, sizeof(tag)-1, "A%d", *seq); snprintf(tagok, sizeof(tagok)-1, "A%d OK", (*seq)++);
   snprintf(buf, sizeof(buf)-1, "%s CAPABILITY\r\n", tag);

   write1(sd, buf, strlen(buf), use_ssl, data->ssl);
   read_response(sd, buf, sizeof(buf), tagok, data, use_ssl);


   /* imap cmd: LOGIN */

   snprintf(tag, sizeof(tag)-1, "A%d", *seq); snprintf(tagok, sizeof(tagok)-1, "A%d OK", (*seq)++);
   snprintf(buf, sizeof(buf)-1, "%s LOGIN %s \"%s\"\r\n", tag, username, password);

   write1(sd, buf, strlen(buf), use_ssl, data->ssl);
   read_response(sd, buf, sizeof(buf), tagok, data, use_ssl);

   if(strncmp(buf, tagok, strlen(tagok))){
      printf("login failed, server reponse: %s\n", buf);
      return ERR;
   }

   return OK;
}


void close_connection(int sd, struct __data *data, int use_ssl){
   close(sd);
 
   if(use_ssl == 1){
      SSL_shutdown(data->ssl);
      SSL_free(data->ssl);
      SSL_CTX_free(data->ctx);
      ERR_free_strings();
   }
}


int list_folders(int sd, int *seq, int use_ssl, struct __data *data){
   char *p, *q, *buf, tag[SMALLBUFSIZE], tagok[SMALLBUFSIZE], puf[MAXBUFSIZE];
   int len=MAXBUFSIZE+3, pos=0, n, rc=ERR;;

   buf = malloc(len);
   if(!buf) return rc;

   memset(buf, 0, len);

   snprintf(tag, sizeof(tag)-1, "A%d", *seq); snprintf(tagok, sizeof(tagok)-1, "A%d OK", (*seq)++);
   //snprintf(puf, sizeof(puf)-1, "%s LIST \"\" %%\r\n", tag);
   snprintf(puf, sizeof(puf)-1, "%s LIST \"\" \"*\"\r\n", tag);

   write1(sd, puf, strlen(puf), use_ssl, data->ssl);

   while(1){
      n = recvtimeoutssl(sd, puf, sizeof(puf), 10, use_ssl, data->ssl);

      if(pos + n >= len){
         q = realloc(buf, len+MAXBUFSIZE+1);
         if(!q){
            printf("realloc failure: %d bytes\n", pos+MAXBUFSIZE+1);
            goto ENDE_FOLDERS;
         }

         buf = q;
         memset(buf+pos, 0, MAXBUFSIZE+1);
         len += MAXBUFSIZE+1;
      }

      memcpy(buf + pos, puf, n);
      pos += n;

      if(strstr(buf, tagok)) break;
   }

 
   p = buf;
   do {
      memset(puf, 0, sizeof(puf));
      p = split(p, '\n', puf, sizeof(puf)-1);
      trimBuffer(puf);

      if(strncmp(puf, "* LIST ", 7) == 0){

         q = strstr(puf, ") \"");
         if(q){
            q += 5;

            if(*q == ' ') q++;
            if(*q == '"') q++;

            if(q[strlen(q)-1] == '"') q[strlen(q)-1] = '\0';

            addnode(data->imapfolders, q);
         }
      }
      else {
         if(strncmp(puf, tagok, strlen(tagok)) == 0) {}
      }

   } while(p);


   rc = OK;

ENDE_FOLDERS:

   free(buf);

   return rc;
}


