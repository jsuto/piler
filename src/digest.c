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


#define MAX(m,n) m <= n ? m : n


int search_header_end(char *p, int n){
   int hdr_len=0;

   if(strlen(p) < 5) return hdr_len;

   for(; *p; p++){
      if(hdr_len < n-2 && *p == '\n' && *(p+1) == '\r' && *(p+2) == '\n'){ hdr_len += 3; return MAX(hdr_len, n); }
      if(hdr_len < n-1 && *p == '\n' && *(p+1) == '\n'){ hdr_len += 2; return MAX(hdr_len, n); }
      hdr_len++;
   }

   return 0;
}


int make_digests(struct session_data *sdata, struct __config *cfg){
   int i=0, n, fd, offset=3, hdr_len=0;
   char *body=NULL;
   unsigned char buf[BIGBUFSIZE], md[DIGEST_LENGTH], md2[DIGEST_LENGTH];
   SHA256_CTX context, context2;

   memset(sdata->bodydigest, 0, 2*DIGEST_LENGTH+1);
   memset(sdata->digest, 0, 2*DIGEST_LENGTH+1);
   SHA256_Init(&context);
   SHA256_Init(&context2);


   fd = open(sdata->filename, O_RDONLY);
   if(fd == -1) return -1;

   while((n = read(fd, buf, sizeof(buf))) > 0){

      SHA256_Update(&context2, buf, n);

      body = (char *)&buf[0];

      if(i == 0){

         hdr_len = search_header_end(body, n);

         if(hdr_len > 0){
            body += hdr_len;
            n -= hdr_len;

            if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: hdr_len: %d, offset: %d", sdata->ttmpfile, hdr_len, offset);
         }
      }


      SHA256_Update(&context, body, n);

      i++;
   }

   close(fd);

   sdata->hdr_len = hdr_len;

   SHA256_Final(md, &context);
   SHA256_Final(md2, &context2);

   for(i=0;i<DIGEST_LENGTH;i++){
      snprintf(sdata->bodydigest + i*2, 2*DIGEST_LENGTH, "%02x", md[i]);
      snprintf(sdata->digest + i*2, 2*DIGEST_LENGTH, "%02x", md2[i]);
   }

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

   while((n = read(fd, buf, sizeof(buf))) > 0){
      SHA256_Update(&context, buf, n);
   }

   close(fd);

   SHA256_Final(md, &context);

   for(i=0;i<DIGEST_LENGTH;i++)
      snprintf(digest + i*2, 2*DIGEST_LENGTH, "%02x", md[i]);

}


