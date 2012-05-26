/*
 * imap.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <openssl/ssl.h>
#include <fcntl.h>
#include <ctype.h>
#include <syslog.h>
#include <unistd.h>
#include <limits.h>
#include <piler.h>



unsigned long resolve_host(char *host){
   struct hostent *h;
   struct in_addr addr;

   if(!host) return 0;

   if((addr.s_addr = inet_addr(host)) == -1){
       if((h = gethostbyname(host)) == NULL){
          return 0;
       }
       else return *(unsigned long*)h->h_addr;
   }
   else return addr.s_addr;
}


int process_imap_folder(int sd, int *seq, char *folder, struct session_data *sdata, struct __data *data, struct __config *cfg){
   int rc=ERR, i, n, pos, messages=0, len, readlen, fd;
   char *p, *q, tag[SMALLBUFSIZE], tagok[SMALLBUFSIZE], buf[MAXBUFSIZE], puf[MAXBUFSIZE], filename[SMALLBUFSIZE];


   snprintf(tag, sizeof(tag)-1, "A%d", *seq); snprintf(tagok, sizeof(tagok)-1, "\r\nA%d OK", (*seq)++);
   snprintf(buf, sizeof(buf)-1, "%s SELECT %s\r\n", tag, folder);
   send(sd, buf, strlen(buf), 0);
   n = recvtimeout(sd, buf, MAXBUFSIZE, 10);


   if(!strstr(buf, tagok)){
      trimBuffer(buf);
      printf("error: %s\n", buf);
      return rc;
   }


   p = &buf[0];
   do {
      memset(puf, 0, sizeof(puf));
      p = split(p, '\n', puf, sizeof(puf)-1);

      q = strstr(puf, " EXISTS");
      if(q){
         *q = '\0';
         messages = atoi(puf+2);
      }

   } while(p);


   printf("found %d messages\n", messages);

   if(messages <= 0) return rc;
   for(i=1; i<=messages; i++){

      snprintf(tag, sizeof(tag)-1, "A%d", *seq); snprintf(tagok, sizeof(tagok)-1, "\r\nA%d OK", (*seq)++);
      snprintf(buf, sizeof(buf)-1, "%s FETCH %d (BODY.PEEK[])\r\n", tag, i);

      snprintf(filename, sizeof(filename)-1, "%s-%d.txt", folder, i);
      unlink(filename);

      fd = open(filename, O_CREAT|O_EXCL|O_RDWR|O_TRUNC, S_IRUSR|S_IWUSR);
      if(fd == -1){
         printf("cannot open: %s\n", filename);
         return rc;
      }


      send(sd, buf, strlen(buf), 0);
      memset(buf, 0, sizeof(buf));
      n = recvtimeout(sd, buf, MAXBUFSIZE, 10);


      len = 0; readlen = n;

      p = strstr(buf, "\r\n");
      if(!p){
         printf("invalid reply: %s", buf);
         continue;
      }

      *p = '\0';
      pos = strlen(buf) + 2;


      if(*(p-1) == '}') *(p-1) = '\0';


      q = strchr(buf, '{');
      if(q){
         q++;
         len = atoi(q);
      }

      if(len < 10){
         printf("too short message: %s\n", buf);
         continue;
      }

      n -= pos;

      q = strstr(p+2, tagok);
      if(q){
         n -= strlen(q) + 1;
      }


      write(fd, p+2, n);


      while(readlen < len){
         memset(buf, 0, sizeof(buf));
         n = recvtimeout(sd, buf, MAXBUFSIZE, 3);
         readlen += n;

         p = strstr(buf, tagok);
         if(p){
            n -= strlen(p)+1;
         }

         write(fd, buf, n);
      
      }

      close(fd);

      rc = import_message(filename, sdata, data, cfg);

      unlink(filename);

   }


   return OK;
}


int connect_to_imap_server(int sd, int *seq, char *imapserver, char *username, char *password){
   int n, pos=0;
   char tag[SMALLBUFSIZE], tagok[SMALLBUFSIZE], buf[MAXBUFSIZE];
   char auth[2*SMALLBUFSIZE];
   unsigned char tmp[SMALLBUFSIZE];
   unsigned long host=0;
   struct sockaddr_in remote_addr;


   host = resolve_host(imapserver);

   remote_addr.sin_family = AF_INET;
   remote_addr.sin_port = htons(143);
   remote_addr.sin_addr.s_addr = host;
   bzero(&(remote_addr.sin_zero),8);


   if(connect(sd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == -1){
      printf("connect()\n");
      return ERR;
   }

   n = recvtimeout(sd, buf, MAXBUFSIZE, 10);
   //printf("connected...\n");


   /*
    * create auth buffer: username + NUL character + username + NUL character + password
    */

   memset(tmp, 0, sizeof(tmp));
   pos = 0;

   memcpy(tmp+pos, username, strlen(username));
   pos = strlen(username) + 1;
   memcpy(tmp+pos, username, strlen(username));
   pos += strlen(username) + 1;
   memcpy(tmp+pos, password, strlen(password));
   pos += strlen(password);


   base64_encode(&tmp[0], pos, &auth[0], sizeof(auth));

   snprintf(tag, sizeof(tag)-1, "A%d", *seq); snprintf(tagok, sizeof(tagok)-1, "A%d OK", (*seq)++);
   snprintf(buf, sizeof(buf)-1, "%s AUTHENTICATE PLAIN %s\r\n", tag, auth);
   send(sd, buf, strlen(buf), 0);
   n = recvtimeout(sd, buf, MAXBUFSIZE, 10);
   if(strncmp(buf, tagok, strlen(tagok))){
      printf("login failed, server reponse: %s\n", buf);
      return ERR;
   }

   //printf("logged in...\n");

   return OK;
}


int list_folders(int sd, int *seq, char *folders, int foldersize){
   int n;
   char *p, *q, tag[SMALLBUFSIZE], tagok[SMALLBUFSIZE], buf[MAXBUFSIZE], puf[MAXBUFSIZE];

   snprintf(folders, foldersize-1, "INBOX");


   snprintf(tag, sizeof(tag)-1, "A%d", *seq); snprintf(tagok, sizeof(tagok)-1, "A%d OK", (*seq)++);
   snprintf(buf, sizeof(buf)-1, "%s LIST \"\" %%\r\n", tag);

   send(sd, buf, strlen(buf), 0);

   n = recvtimeout(sd, buf, MAXBUFSIZE, 10);

   p = &buf[0];
   do {
      memset(puf, 0, sizeof(puf));
      p = split(p, '\n', puf, sizeof(puf)-1);
      trimBuffer(puf);

      if(strncmp(puf, "* LIST ", 7) == 0){
         q = strrchr(puf, ' ');
         if(q){
            if(*(q+1) == '"') q += 2;
            if(puf[strlen(puf)-1] == '"') puf[strlen(puf)-1] = '\0';

            if(strncasecmp(q, "junk", 4) && strncasecmp(q, "trash", 5) && strncasecmp(q, "spam", 4) && strncasecmp(q, "draft", 5)){
               strncat(folders, "\n", foldersize-1);
               strncat(folders, q, foldersize-1);
            }

         }
      }
      else {
         if(strncmp(puf, tagok, strlen(tagok)) == 0) {}
      }

   } while(p);


   return 0;
}


