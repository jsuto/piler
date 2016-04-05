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


void update_import_job_stat(struct session_data *sdata, struct __data *data);


int get_message_length_from_imap_answer(char *s){
   char *p, *q;
   int len=0;

   p = strstr(s, "\r");
   if(!p){
      printf("invalid reply: %s", s);
      return len;
   }

   *p = '\0';

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


int read_response(int sd, char *buf, int buflen, int *seq, struct __data *data, int use_ssl){
   int i=0, n, len=0, rc=0;
   char puf[MAXBUFSIZE], tagok[SMALLBUFSIZE], tagno[SMALLBUFSIZE], tagbad[SMALLBUFSIZE];

   snprintf(tagok, sizeof(tagok)-1, "A%d OK", *seq);
   snprintf(tagno, sizeof(tagno)-1, "A%d NO", *seq);
   snprintf(tagbad, sizeof(tagbad)-1, "A%d BAD", *seq);

   memset(buf, 0, buflen);

   while(!strstr(buf, tagok)){
      n = recvtimeoutssl(sd, puf, sizeof(puf), data->import->timeout, use_ssl, data->ssl);

      if(n + len < buflen) strncat(buf, puf, n);
      else goto END;

      /*
       * possible error message from the imap server:
       *
       *  * BYE Temporary problem, please try again later\r\n
       */

      if(i == 0 && (strstr(puf, tagno) || strstr(puf, tagbad) || strstr(puf, "* BYE ")) ) goto END;


      len += n;
      i++;
   }

   rc = 1;

END:

   (*seq)++;

   return rc;
}


int process_imap_folder(int sd, int *seq, char *folder, struct session_data *sdata, struct __data *data, int use_ssl, int dryrun, struct __config *cfg){
   int rc=ERR, i, n, messages=0, len, readlen, fd, nreads, readpos, finished, msglen, msg_written_len, tagoklen, tagbadlen, result;
   char *p, tag[SMALLBUFSIZE], tagok[SMALLBUFSIZE], tagbad[SMALLBUFSIZE], buf[MAXBUFSIZE], puf[MAXBUFSIZE], filename[SMALLBUFSIZE];

   /* imap cmd: SELECT */

   snprintf(buf, sizeof(buf)-1, "A%d SELECT %s\r\n", *seq, folder);

   write1(sd, buf, strlen(buf), use_ssl, data->ssl);
   if(read_response(sd, buf, sizeof(buf), seq, data, use_ssl) == 0){
      trimBuffer(buf);
      printf("select cmd error: %s\n", buf);
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


   if(data->recursive_folder_names == 1){
      data->folder = get_folder_id(sdata, data, folder, 0);
      if(data->folder == ERR_FOLDER) data->folder = add_new_folder(sdata, data, folder, 0);
   }


   data->import->total_messages += messages;

   for(i=data->import->start_position; i<=messages; i++){

      /* whether to quit after processing a batch of messages */

      if(data->import->batch_processing_limit > 0 && data->import->processed_messages >= data->import->batch_processing_limit){
         break;
      }

      data->import->processed_messages++;
      if(data->quiet == 0){ printf("processed: %7d [%3d%%]\r", data->import->processed_messages, 100*i/messages); fflush(stdout); }

      snprintf(tag, sizeof(tag)-1, "A%d", *seq);
      snprintf(tagok, sizeof(tagok)-1, "A%d OK", (*seq)++);
      snprintf(tagbad, sizeof(tagbad)-1, "%s BAD", tag);

      tagoklen = strlen(tagok);
      tagbadlen = strlen(tagbad);

      snprintf(buf, sizeof(buf)-1, "%s FETCH %d (BODY.PEEK[])\r\n", tag, i);

      snprintf(filename, sizeof(filename)-1, "%d-imap-%d.txt", getpid(), data->import->processed_messages);
      unlink(filename);

      fd = open(filename, O_CREAT|O_EXCL|O_RDWR|O_TRUNC, S_IRUSR|S_IWUSR);
      if(fd == -1){
         printf("cannot open: %s\n", filename);
         return rc;
      }


      write1(sd, buf, strlen(buf), use_ssl, data->ssl);

      readlen = 0;
      nreads = 0;
      readpos = 0;
      finished = 0;
      msglen = 0;
      msg_written_len = 0;

      while((n = recvtimeoutssl(sd, &buf[readpos], sizeof(buf)-readpos, data->import->timeout, use_ssl, data->ssl)) > 0){

         readlen += n;

         if(strchr(buf, '\n')){
            readpos = 0;
            p = &buf[0];
            do {
               nreads++;
               memset(puf, 0, sizeof(puf));
               p = split(p, '\n', puf, sizeof(puf)-1, &result);
               len = strlen(puf);

               if(result == 1){
                  // process a complete line

                  if(nreads == 1){

                     if(strcasestr(puf, " FETCH ")){
                        msglen = get_message_length_from_imap_answer(puf);

                        if(msglen == 0){
                           finished = 1;
                           break;
                        }
                        continue;
                     }

                     if(strcasestr(puf, " BYE")){
                        printf("imap server sent BYE response: '%s'\n", puf);
                        close(fd);
                        unlink(filename);
                        return ERR;
                     }

                  }
 
                  if(len > 0 && msg_written_len < msglen){
                     write(fd, puf, len);
                     write(fd, "\n", 1);
                     msg_written_len += len + 1;
                  }

                  if(strncmp(puf, tagok, tagoklen) == 0){
                     finished = 1;
                     break;
                  }

                  if(strncmp(puf, tagbad, tagbadlen) == 0){
                     printf("ERROR happened reading the message!\n");
                     finished = 1;
                     break;
                  }

               }
               else {
                  // prepend the last incomplete line back to 'buf'

                  snprintf(buf, sizeof(buf)-2, "%s", puf);
                  readpos = len;
                  break;
               }

            } while(p);



         }
         else {
            readpos += n;
         }

         if(finished == 1) break;
      }


      close(fd);

      if(dryrun == 0 && msglen > 10){
         rc = import_message(filename, sdata, data, cfg);

         if(data->import->processed_messages % 100 == 0){
            time(&(data->import->updated));
            update_import_job_stat(sdata, data);
         }
      }
      else rc = OK;


      if(rc == ERR) printf("error importing '%s'\n", filename);
      else {

         if(data->import->remove_after_import == 1 && dryrun == 0){
            snprintf(buf, sizeof(buf)-1, "A%d STORE %d +FLAGS.SILENT (\\Deleted)\r\n", *seq, i);
            write1(sd, buf, strlen(buf), use_ssl, data->ssl);
            read_response(sd, buf, sizeof(buf), seq, data, use_ssl);
         }


         if(data->import->move_folder && data->import->cap_uidplus == 1 && dryrun == 0){

            snprintf(tagok, sizeof(tagok)-1, "A%d OK", *seq);
            tagoklen = strlen(tagok);

            snprintf(buf, sizeof(buf)-1, "A%d COPY %d %s\r\n", *seq, i, data->import->move_folder);
            write1(sd, buf, strlen(buf), use_ssl, data->ssl);
            read_response(sd, buf, sizeof(buf), seq, data, use_ssl);

            if(strncmp(buf, tagok, tagoklen) == 0){
               snprintf(buf, sizeof(buf)-1, "A%d STORE %d +FLAGS.SILENT (\\Deleted)\r\n", *seq, i);
               write1(sd, buf, strlen(buf), use_ssl, data->ssl);
               read_response(sd, buf, sizeof(buf), seq, data, use_ssl);

            }
         }



         if(data->import->download_only == 0) unlink(filename);
      }


   }

   if((data->import->remove_after_import == 1 || data->import->move_folder) && dryrun == 0){
      snprintf(buf, sizeof(buf)-1, "A%d EXPUNGE\r\n", *seq);
      write1(sd, buf, strlen(buf), use_ssl, data->ssl);
      read_response(sd, buf, sizeof(buf), seq, data, use_ssl);
   }


   printf("\n");

   return OK;
}


int connect_to_imap_server(int sd, int *seq, char *username, char *password, struct __data *data, int use_ssl){
   int n;
   char buf[MAXBUFSIZE];
   X509* server_cert;
   char *str;

   data->import->cap_uidplus = 0;

   if(use_ssl == 1){

      SSL_library_init();
      SSL_load_error_strings();

      data->ctx = SSL_CTX_new(TLSv1_client_method());
      CHK_NULL(data->ctx, "internal SSL error");

      data->ssl = SSL_new(data->ctx);
      CHK_NULL(data->ssl, "internal ssl error");

      SSL_set_fd(data->ssl, sd);
      n = SSL_connect(data->ssl);
      CHK_SSL(n, "internal ssl error");

      printf("Cipher: %s\n", SSL_get_cipher(data->ssl));

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


   recvtimeoutssl(sd, buf, sizeof(buf), data->import->timeout, use_ssl, data->ssl);


   /* imap cmd: LOGIN */

   snprintf(buf, sizeof(buf)-1, "A%d LOGIN %s \"%s\"\r\n", *seq, username, password);

   write1(sd, buf, strlen(buf), use_ssl, data->ssl);
   if(read_response(sd, buf, sizeof(buf), seq, data, use_ssl) == 0){
      printf("login failed, server reponse: %s\n", buf);
      return ERR;
   }

   if(strstr(buf, "UIDPLUS")){
      data->import->cap_uidplus = 1;
   }
   else {

      /* run the CAPABILITY command if the reply doesn't contain the UIDPLUS capability */

      snprintf(buf, sizeof(buf)-1, "A%d CAPABILITY\r\n", *seq);

      write1(sd, buf, strlen(buf), use_ssl, data->ssl);
      read_response(sd, buf, sizeof(buf), seq, data, use_ssl);

      if(strstr(buf, "UIDPLUS")) data->import->cap_uidplus = 1;
   }


   return OK;
}


void send_imap_close(int sd, int *seq, struct __data *data, int use_ssl){
   char puf[SMALLBUFSIZE];  
   snprintf(puf, sizeof(puf)-1, "A%d CLOSE\r\n", *seq);

   write1(sd, puf, strlen(puf), use_ssl, data->ssl);
}


int list_folders(int sd, int *seq, int use_ssl, struct __data *data){
   char *p, *q, *r, *buf, *ruf, tag[SMALLBUFSIZE], tagok[SMALLBUFSIZE], puf[MAXBUFSIZE];
   int len=MAXBUFSIZE+3, pos=0, n, rc=ERR, fldrlen=0, result;

   printf("List of IMAP folders:\n");

   buf = malloc(len);
   if(!buf) return rc;

   memset(buf, 0, len);

   snprintf(tag, sizeof(tag)-1, "A%d", *seq); snprintf(tagok, sizeof(tagok)-1, "A%d OK", (*seq)++);
   //snprintf(puf, sizeof(puf)-1, "%s LIST \"\" %%\r\n", tag);
   snprintf(puf, sizeof(puf)-1, "%s LIST \"\" \"*\"\r\n", tag);

   write1(sd, puf, strlen(puf), use_ssl, data->ssl);

   while(1){
      n = recvtimeoutssl(sd, puf, sizeof(puf), data->import->timeout, use_ssl, data->ssl);
      if(n < 0) return ERR;

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
      p = split(p, '\n', puf, sizeof(puf)-1, &result);
      trimBuffer(puf);

      if(strncmp(puf, "* LIST ", 7) == 0 || fldrlen){

         if (fldrlen)
            q = puf;
         else
            q = strstr(puf, ") \"");
         if(q){
            if (!fldrlen) {
               q += 3;
               while(*q != '"') q++;
               q++;
               if(*q == ' ') q++;
            }

            if(!fldrlen && *q == '{' && q[strlen(q)-1] == '}') {
               q++;
               fldrlen = strtol(q, NULL, 10);
            } else {
               
               if(fldrlen) {
                  ruf = malloc(strlen(q) * 2 + 1);
                  memset(ruf, 0, strlen(q) * 2 + 1);
                  memcpy(ruf, q, strlen(q));
                  r = ruf;
                  while(*r != '\0') {
                     if(*r == '\\') {
                        memmove(r + 1, r, strlen(r));
                        r++;
                     }
                     r++;
                  }
                  addnode(data->imapfolders, ruf);
                  printf("=> '%s'\n", ruf);
                  free(ruf);
                  fldrlen = 0;
               } else {
                  addnode(data->imapfolders, q);
                  printf("=> '%s'\n", q);
               }
               
            }

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


