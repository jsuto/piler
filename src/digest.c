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
   char *q = p;

   if(n < 5) return hdr_len;

   for(; *p; p++){
      if(hdr_len < n-2 && *p == '\n' && *(p+1) == '\r' && *(p+2) == '\n'){ hdr_len += 3; return MAX(hdr_len, n); }
      if(hdr_len < n-1 && *p == '\n' && *(p+1) == '\n'){ hdr_len += 2; return MAX(hdr_len, n); }
      hdr_len++;
   }

   if(*(p-1) == '\n' && strcasestr(q, "Message-ID:")) return n; else return 0;
}


int make_digests(struct session_data *sdata, struct config *cfg){
   int n, fd, offset=3, hdr_len=0;
   char *body=NULL;

   EVP_MD_CTX *ctx, *ctx2;
   const EVP_MD *md, *md2;
   unsigned int i=0, md_len, md_len2;
   unsigned char md_value[EVP_MAX_MD_SIZE], md_value2[EVP_MAX_MD_SIZE];

   memset(sdata->bodydigest, 0, 2*DIGEST_LENGTH+1);
   memset(sdata->digest, 0, 2*DIGEST_LENGTH+1);

   md = EVP_get_digestbyname("sha256");
   md2 = EVP_get_digestbyname("sha256");
   if(md == NULL || md2 == NULL){
      syslog(LOG_PRIORITY, "ERROR: unknown message digest: sha256 in %s:%d", __func__, __LINE__);
      return 1;
   }

   ctx = EVP_MD_CTX_new();
   EVP_DigestInit_ex(ctx, md, NULL);

   ctx2 = EVP_MD_CTX_new();
   EVP_DigestInit_ex(ctx2, md2, NULL);

   fd = open(sdata->filename, O_RDONLY);
   if(fd == -1) {
      syslog(LOG_PRIORITY, "ERROR: open() %s:%d", __func__, __LINE__);
      EVP_MD_CTX_free(ctx);
      EVP_MD_CTX_free(ctx2);
      return 1;
   }

   unsigned char *buf = calloc(1, cfg->max_header_size);
   if(!buf) {
      syslog(LOG_PRIORITY, "ERROR: calloc() %s:%d", __func__, __LINE__);
      close(fd);
      EVP_MD_CTX_free(ctx);
      EVP_MD_CTX_free(ctx2);
      return 1;
   }

   while((n = read(fd, buf, cfg->max_header_size)) > 0){
      EVP_DigestUpdate(ctx2, buf, n);

      body = (char *)&buf[0];

      if(i == 0){

         hdr_len = search_header_end(body, n);

         if(hdr_len > 0){
            body += hdr_len;
            n -= hdr_len;

            if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: hdr_len: %d, offset: %d", sdata->ttmpfile, hdr_len, offset);
         }
      }


      EVP_DigestUpdate(ctx, body, n);

      i++;
   }

   free(buf);

   close(fd);

   sdata->hdr_len = hdr_len;

   EVP_DigestFinal_ex(ctx, md_value, &md_len);
   EVP_MD_CTX_free(ctx);
   EVP_DigestFinal_ex(ctx2, md_value2, &md_len2);
   EVP_MD_CTX_free(ctx2);

   for(i=0;i<md_len;i++){
      snprintf(sdata->bodydigest + i*2, 3, "%02x", md_value[i]);
   }

   for(i=0;i<md_len2;i++){
      snprintf(sdata->digest + i*2, 3, "%02x", md_value2[i]);
   }

   return 0;
}


void raw_digest_file(char *digestname, char *filename, unsigned char *md_value){
   int fd, n;
   unsigned char buf[MAXBUFSIZE];
   EVP_MD_CTX *ctx;
   const EVP_MD *md;
   unsigned int md_len;

   md = EVP_get_digestbyname(digestname);
   if(md == NULL){
      syslog(LOG_PRIORITY, "ERROR: unknown message digest: '%s' in %s:%d", digestname, __func__, __LINE__);
      return;
   }

   fd = open(filename, O_RDONLY);
   if(fd == -1) return;

   ctx = EVP_MD_CTX_new();
   EVP_DigestInit_ex(ctx, md, NULL);

   while((n = read(fd, buf, sizeof(buf))) > 0){
      EVP_DigestUpdate(ctx, buf, n);
   }

   close(fd);

   EVP_DigestFinal_ex(ctx, md_value, &md_len);
   EVP_MD_CTX_free(ctx);
}


void digest_file(char *filename, char *digest){
   unsigned char md[DIGEST_LENGTH];

   raw_digest_file("sha256", filename, &md[0]);

   memset(digest, 0, 2*DIGEST_LENGTH+1);

   for(int i=0;i<SHA256_DIGEST_LENGTH;i++){
      snprintf(digest + i*2, 2*DIGEST_LENGTH, "%02x", md[i]);
   }
}


void digest_string(char *digestname, char *s, char *digest){
   EVP_MD_CTX *ctx;
   const EVP_MD *md;
   unsigned int i, md_len;
   unsigned char md_value[DIGEST_LENGTH];

   memset(digest, 0, 2*DIGEST_LENGTH+2);

   md = EVP_get_digestbyname(digestname);
   if(md == NULL){
      syslog(LOG_PRIORITY, "ERROR: unknown message digest: '%s' in %s:%d", digestname, __func__, __LINE__);
      return;
   }

   ctx = EVP_MD_CTX_new();
   EVP_DigestInit_ex(ctx, md, NULL);
   EVP_DigestUpdate(ctx, s, strlen(s));
   EVP_DigestFinal_ex(ctx, md_value, &md_len);
   EVP_MD_CTX_free(ctx);

   for(i=0;i<md_len;i++){
      snprintf(digest + i*2, 2*DIGEST_LENGTH, "%02x", md_value[i]);
   }
}


void create_md5_from_email_address(char *puf, char *md5buf){
   digest_string("md5", puf, md5buf);
   md5buf[2*MD5_DIGEST_LENGTH] = ' ';
}
