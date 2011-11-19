/*
 * digest.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <piler.h>
#include <openssl/evp.h>


int make_body_digest(struct session_data *sdata, struct __config *cfg){
   int i=0, n, fd;
   char *p, *body=NULL;
   unsigned char buf[MAXBUFSIZE], md[DIGEST_LENGTH];
   SHA256_CTX context;

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: digesting", sdata->ttmpfile);

   memset(sdata->bodydigest, 0, 2*DIGEST_LENGTH+1);
   SHA256_Init(&context);

   fd = open(sdata->ttmpfile, O_RDONLY);
   if(fd == -1) return -1;

   while((n = read(fd, buf, MAXBUFSIZE)) > 0){
      body = (char *)&buf[0];

      i++;
      if(i == 1){
         p = strstr((char*)buf, "\n\n");
         if(p){
            body = p+2;
            n = strlen(body);
         } else {
            p = strstr((char*)buf, "\n\r\n");
            if(p){
               body = p+3;
               n = strlen(body);
            }
         }
      }

      SHA256_Update(&context, body, n);

   }

   close(fd);

   SHA256_Final(md, &context);

   for(i=0;i<DIGEST_LENGTH;i++)
      snprintf(sdata->bodydigest + i*2, 2*DIGEST_LENGTH, "%02x", md[i]);

   return 0;
}


void digest_file(char *filename, char *digest){
   int fd, i, n;
   unsigned char buf[MAXBUFSIZE], md[DIGEST_LENGTH];
   SHA256_CTX context;

   memset(digest, 0, 2*DIGEST_LENGTH+1);

   fd = open(filename, O_RDONLY);
   if(fd == -1) return;

   SHA256_Init(&context);

   while((n = read(fd, buf, MAXBUFSIZE)) > 0){
      SHA256_Update(&context, buf, n);
   }

   close(fd);

   SHA256_Final(md, &context);

   for(i=0;i<DIGEST_LENGTH;i++)
      snprintf(digest + i*2, 2*DIGEST_LENGTH, "%02x", md[i]);

}


