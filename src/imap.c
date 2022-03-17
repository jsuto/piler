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


int read_response(char *buf, int buflen, struct data *data){
   int i=0, n, len=0, rc=0;
   char puf[MAXBUFSIZE], tagok[SMALLBUFSIZE], tagno[SMALLBUFSIZE], tagbad[SMALLBUFSIZE];

   snprintf(tagok, sizeof(tagok)-1, "A%d OK", data->import->seq);
   snprintf(tagno, sizeof(tagno)-1, "A%d NO", data->import->seq);
   snprintf(tagbad, sizeof(tagbad)-1, "A%d BAD", data->import->seq);

   memset(buf, 0, buflen);

   while(!strstr(buf, tagok)){
      n = recvtimeoutssl(data->net, puf, sizeof(puf));

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

   (data->import->seq)++;

   return rc;
}


int connect_to_imap_server(struct data *data){
   char buf[MAXBUFSIZE];

   data->import->cap_uidplus = 0;

   if(data->net->use_ssl == 1){
      init_ssl_to_server(data);
   }


   recvtimeoutssl(data->net, buf, sizeof(buf));


   /* imap cmd: LOGIN */

   snprintf(buf, sizeof(buf)-1, "A%d LOGIN %s \"%s\"\r\n", data->import->seq, data->import->username, data->import->password);

   write1(data->net, buf, strlen(buf));
   if(read_response(buf, sizeof(buf), data) == 0){
      printf("login failed, server reponse: %s\n", buf);
      return ERR;
   }

   if(strstr(buf, "UIDPLUS")){
      data->import->cap_uidplus = 1;
   }
   else {

      /* run the CAPABILITY command if the reply doesn't contain the UIDPLUS capability */

      snprintf(buf, sizeof(buf)-1, "A%d CAPABILITY\r\n", data->import->seq);

      write1(data->net, buf, strlen(buf));
      read_response(buf, sizeof(buf), data);

      if(strstr(buf, "UIDPLUS")) data->import->cap_uidplus = 1;
   }


   return OK;
}


int imap_select_cmd_on_folder(char *folder, struct data *data){
   int messages=0;
   char *p, buf[MAXBUFSIZE];

   if(strchr(folder, '"'))
      snprintf(buf, sizeof(buf)-1, "A%d SELECT %s\r\n", data->import->seq, folder);
   else
      snprintf(buf, sizeof(buf)-1, "A%d SELECT \"%s\"\r\n", data->import->seq, folder);

   write1(data->net, buf, strlen(buf));
   if(read_response(buf, sizeof(buf), data) == 0){
      trimBuffer(buf);
      printf("ERROR: select cmd error: %s\n", buf);
      return messages;
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

   if(data->quiet == 0){printf("found %d messages\n", messages); }

   data->import->total_messages += messages;

   return messages;
}


int imap_download_email(struct data *data, int i){
   int fd, len, result, tagoklen, tagbadlen;
   int n, readlen=0, nreads=0, readpos=0, finished=0, msglen=0, msg_written_len=0;
   char *p, buf[MAXBUFSIZE], puf[MAXBUFSIZE], tag[SMALLBUFSIZE], tagok[SMALLBUFSIZE], tagbad[SMALLBUFSIZE];

   data->import->processed_messages++;

   snprintf(data->import->filename, SMALLBUFSIZE-1, "%d-imap-%d.txt", getpid(), data->import->processed_messages);

   unlink(data->import->filename);

   fd = open(data->import->filename, O_CREAT|O_EXCL|O_RDWR|O_TRUNC, S_IRUSR|S_IWUSR);
   if(fd == -1){
      printf("cannot open: %s\n", data->import->filename);
      return ERR;
   }

   snprintf(tag, sizeof(tag)-1, "A%d", data->import->seq);
   snprintf(tagok, sizeof(tagok)-1, "A%d OK", (data->import->seq)++);
   snprintf(tagbad, sizeof(tagbad)-1, "%s BAD", tag);

   tagoklen = strlen(tagok);
   tagbadlen = strlen(tagbad);

   snprintf(buf, sizeof(buf)-1, "%s FETCH %d (BODY.PEEK[])\r\n", tag, i);
   write1(data->net, buf, strlen(buf));

   while((n = recvtimeoutssl(data->net, &buf[readpos], sizeof(buf)-readpos)) > 0){

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
                     unlink(data->import->filename);
                     return ERR;
                  }

               }

               if(len > 0 && msg_written_len < msglen){
                  if(write(fd, puf, len) == -1) printf("ERROR: writing to fd\n");
                  if(write(fd, "\n", 1) == -1) printf("ERROR: writing to fd\n");
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

   if(msglen > 10) return OK;

   return ERR;
}


void imap_delete_message(struct data *data, int i){
   char buf[SMALLBUFSIZE];

   snprintf(buf, sizeof(buf)-1, "A%d STORE %d +FLAGS.SILENT (\\Deleted)\r\n", data->import->seq, i);
   write1(data->net, buf, strlen(buf));
   read_response(buf, sizeof(buf), data);
}


void imap_move_message_to_folder(struct data *data, int i){
   int tagoklen;
   char buf[SMALLBUFSIZE], tagok[SMALLBUFSIZE];

   snprintf(tagok, sizeof(tagok)-1, "A%d OK", data->import->seq);
   tagoklen = strlen(tagok);

   snprintf(buf, sizeof(buf)-1, "A%d COPY %d %s\r\n", data->import->seq, i, data->import->move_folder);
   write1(data->net, buf, strlen(buf));
   read_response(buf, sizeof(buf), data);

   if(strncmp(buf, tagok, tagoklen) == 0){
      snprintf(buf, sizeof(buf)-1, "A%d STORE %d +FLAGS.SILENT (\\Deleted)\r\n", data->import->seq, i);
      write1(data->net, buf, strlen(buf));
      read_response(buf, sizeof(buf), data);
   }
}


void imap_expunge_message(struct data *data){
   char buf[SMALLBUFSIZE];

   snprintf(buf, sizeof(buf)-1, "A%d EXPUNGE\r\n", data->import->seq);
   write1(data->net, buf, strlen(buf));
   read_response(buf, sizeof(buf), data);
}


int process_imap_folder(char *folder, struct session_data *sdata, struct data *data, struct config *cfg){
   int i, messages=0;

   messages = imap_select_cmd_on_folder(folder, data);

   if(messages <= 0) return OK;

   if(data->recursive_folder_names == 1){
      data->folder = get_folder_id(sdata, folder, 0);
      if(data->folder == ERR_FOLDER) data->folder = add_new_folder(sdata, folder, 0);
   }

   for(i=data->import->start_position; i<=messages; i++){
      if(imap_download_email(data, i) == OK){
         if(data->quiet == 0){ printf("processed: %7d [%3d%%]\r", data->import->processed_messages, 100*i/messages); fflush(stdout); }

         if(data->import->dryrun == 0){
            int rc = import_message(sdata, data, cfg);

            if(data->import->remove_after_import == 1 && rc == OK){
               imap_delete_message(data, i);
            }

            if(data->import->move_folder && data->import->cap_uidplus == 1){
               imap_move_message_to_folder(data, i);
            }
         }

         if(data->import->download_only == 0) unlink(data->import->filename);
      }

      /* whether to quit after processing a batch of messages */

      if(data->import->batch_processing_limit > 0 && data->import->processed_messages >= data->import->batch_processing_limit){
         break;
      }
   }

   if((data->import->remove_after_import == 1 || data->import->move_folder) && data->import->dryrun == 0){
      imap_expunge_message(data);
   }


   if(data->quiet == 0){printf("\n"); }

   return OK;
}


void send_imap_close(struct data *data){
   char puf[SMALLBUFSIZE];
   snprintf(puf, sizeof(puf)-1, "A%d CLOSE\r\n", data->import->seq);

   write1(data->net, puf, strlen(puf));
}


int list_folders(struct data *data){
   char *p, *q, *r, *buf, *ruf, tag[SMALLBUFSIZE], tagok[SMALLBUFSIZE], puf[MAXBUFSIZE];
   char attrs[SMALLBUFSIZE], folder[SMALLBUFSIZE];
   int len=MAXBUFSIZE+3, pos=0, n, rc=ERR, fldrlen=0, result;

   if(data->quiet == 0){printf("List of IMAP folders:\n"); }

   buf = malloc(len);
   if(!buf) return rc;

   memset(buf, 0, len);

   snprintf(tag, sizeof(tag)-1, "A%d", data->import->seq); snprintf(tagok, sizeof(tagok)-1, "A%d OK", (data->import->seq)++);
   if(data->import->folder_imap == NULL)
      snprintf(puf, sizeof(puf)-1, "%s LIST \"\" \"*\"\r\n", tag);
   else
      snprintf(puf, sizeof(puf)-1, "%s LIST \"%s\" \"*\"\r\n", tag, data->import->folder_imap);

   write1(data->net, puf, strlen(puf));

   p = NULL;

   while(1){
      n = recvtimeoutssl(data->net, puf, sizeof(puf));
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

      p = strstr(buf, tagok);
      if(p) break;
   }

   // trim the "A3 OK LIST completed" trailer off
   if(p) *p = '\0'; //-V547

   memset(attrs, 0, sizeof(attrs));

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
               *q = '\0';
               snprintf(attrs, sizeof(attrs)-1, "%s", &puf[8]);

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
                  int ruflen = strlen(q) * 2;
                  ruf = malloc(ruflen + 1);
                  if(ruf){
                     snprintf(ruf, ruflen, "%s", q);
                     r = ruf;
                     while(*r != '\0') {
                        if(*r == '\\') {
                           memmove(r + 1, r, strlen(r));
                           r++;
                        }
                        r++;
                     }

                     snprintf(folder, sizeof(folder)-1, "%s", ruf);

                     free(ruf);
                  }
                  else {
                     printf("error: ruf = malloc()\n");
                  }

                  fldrlen = 0;
               } else {
                  snprintf(folder, sizeof(folder)-1, "%s", q);
               }

               if(!strstr(attrs, "\\Noselect")){
                  addnode(data->imapfolders, folder);
               }
               else if(data->quiet == 0){printf("skipping "); }

               if(data->quiet == 0){printf("=> '%s [%s]'\n", folder, attrs); }

               memset(attrs, 0, sizeof(attrs));
            }

         }
      }

   } while(p);


   rc = OK;

ENDE_FOLDERS:

   free(buf);

   return rc;
}
