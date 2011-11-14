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


int make_body_digest(struct session_data *sdata){
   int i=0, n, fd;
   char *p, *body=NULL;
   unsigned char buf[MAXBUFSIZE];

   memset(sdata->bodydigest, 0, 2*DIGEST_LENGTH+1);
   SHA256_Init(&(sdata->context));

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

      SHA256_Update(&(sdata->context), body, n);

   }

   close(fd);

   SHA256_Final(sdata->md, &(sdata->context));

   for(i=0;i<DIGEST_LENGTH;i++)
      snprintf(sdata->bodydigest + i*2, 2*DIGEST_LENGTH, "%02x", sdata->md[i]);

   return 0;
}


