/*
 * store.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <piler.h>
#include <zlib.h>
#include <openssl/blowfish.h>
#include <openssl/evp.h>


int store_message(struct session_data *sdata, struct _state *state, int stored, struct __config *cfg){
   int ret=0, rc, fd, n, len=sdata->tot_len;
   char *addr, *p0, *p1, *p2, s[SMALLBUFSIZE];
   struct stat st;
   Bytef *z=NULL;
   uLongf dstlen;

   EVP_CIPHER_CTX ctx;
   unsigned char *outbuf=NULL;
   int outlen, tmplen;

   struct timezone tz;
   struct timeval tv1, tv2;


   /* fix data length to store */

   /*if(stored == 1 && sdata->hdr_len > 100){
      len = sdata->hdr_len;
   }*/

   fd = open(sdata->ttmpfile, O_RDONLY);
   if(fd == -1) return ret;

   gettimeofday(&tv1, &tz);

   addr = mmap(NULL, sdata->tot_len, PROT_READ, MAP_PRIVATE, fd, 0);
   close(fd);

   if(addr == MAP_FAILED) return ret;

   dstlen = compressBound(len);

   z = malloc(dstlen);

   if(z == NULL){
      munmap(addr, sdata->tot_len);
      return ret;
   }

   rc = compress(z, &dstlen, (const Bytef *)addr, len);
   gettimeofday(&tv2, &tz);
   sdata->__compress += tvdiff(tv2, tv1);

   munmap(addr, sdata->tot_len);

   if(rc != Z_OK) goto ENDE;

   gettimeofday(&tv1, &tz);

   EVP_CIPHER_CTX_init(&ctx);
   EVP_EncryptInit_ex(&ctx, EVP_bf_cbc(), NULL, cfg->key, cfg->iv);

   outbuf = malloc(dstlen + EVP_MAX_BLOCK_LENGTH);
   if(outbuf == NULL) goto ENDE;

   if(!EVP_EncryptUpdate(&ctx, outbuf, &outlen, z, dstlen)) goto ENDE;
   if(!EVP_EncryptFinal_ex(&ctx, outbuf + outlen, &tmplen)) goto ENDE;
   outlen += tmplen;
   EVP_CIPHER_CTX_cleanup(&ctx);

   gettimeofday(&tv2, &tz);
   sdata->__encrypt += tvdiff(tv2, tv1);


   /* create a filename according to piler_id */

   snprintf(s, sizeof(s)-1, "%s/%c%c/%c%c/%c%c/%s", cfg->queuedir, sdata->ttmpfile[RND_STR_LEN-6], sdata->ttmpfile[RND_STR_LEN-5], sdata->ttmpfile[RND_STR_LEN-4], sdata->ttmpfile[RND_STR_LEN-3], sdata->ttmpfile[RND_STR_LEN-2], sdata->ttmpfile[RND_STR_LEN-1], sdata->ttmpfile);

   p0 = strrchr(s, '/'); if(!p0) goto ENDE;
   *p0 = '\0';

   if(stat(s, &st)){
      p1 = strrchr(s, '/'); if(!p1) goto ENDE;
      *p1 = '\0';
      p2 = strrchr(s, '/'); if(!p2) goto ENDE;
      *p2 = '\0';

      mkdir(s, 0755);
      *p2 = '/';
      mkdir(s, 0755);
      *p1 = '/';
      mkdir(s, 0755);
   }

   *p0 = '/';

   fd = open(s, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
   if(fd == -1) goto ENDE;

   n = write(fd, outbuf, outlen);

   if(n == outlen){
      ret = 1;
   }

   fsync(fd);

   close(fd);

   if(ret == 0){
      unlink(s);
   }


ENDE:
   if(outbuf) free(outbuf);
   if(z) free(z);

   return ret;
}


