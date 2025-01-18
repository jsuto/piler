/*
 * archive.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>
#include <syslog.h>
#include <openssl/blowfish.h>
#include <openssl/evp.h>
#if OPENSSL_VERSION_MAJOR >= 3
   #include <openssl/provider.h>
#endif
#include <zlib.h>
#include <assert.h>
#include <piler.h>



void zerr(int ret){

   fputs("zpipe: ", stderr);

   switch (ret) {
      case Z_ERRNO:
                    fputs("I/O error\n", stderr);
                    break;

      case Z_STREAM_ERROR:
                    fputs("invalid compression level\n", stderr);
                    break;


      case Z_DATA_ERROR:
                    fputs("invalid or incomplete deflate data\n", stderr);
                    break;

      case Z_MEM_ERROR:
                    fputs("out of memory\n", stderr);
                    break;

      case Z_VERSION_ERROR:
                    fputs("zlib version mismatch!\n", stderr);
                    break;

   }
}


int inf(unsigned char *in, int len, int mode, char **buffer, FILE *dest){
   int ret, pos=0;
   z_stream strm;
   char *new_ptr;
   unsigned char out[REALLYBIGBUFSIZE];

   /* expecting deflate with 32k window size (0x78) */
   if(len > 0 && in[0] != 0x78)
      return Z_DATA_ERROR;

   /* allocate inflate state */

   strm.zalloc = Z_NULL;
   strm.zfree = Z_NULL;
   strm.opaque = Z_NULL;
   strm.avail_in = 0;
   strm.next_in = Z_NULL;
   ret = inflateInit(&strm);


   if(ret != Z_OK) return ret;

   strm.avail_in = len;
   strm.next_in = in;

   if(mode == WRITE_TO_BUFFER){
      *buffer = malloc(REALLYBIGBUFSIZE);
      if(!*buffer) return Z_MEM_ERROR;
      memset(*buffer, 0, REALLYBIGBUFSIZE);
   }


   do {
      strm.avail_out = REALLYBIGBUFSIZE;
      strm.next_out = out;
      ret = inflate(&strm, Z_NO_FLUSH);

      assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
      switch (ret) {
         case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
         case Z_DATA_ERROR:
         case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return ret;
      }

      unsigned have = REALLYBIGBUFSIZE - strm.avail_out;

      /*
       * write the uncompressed result either to stdout
       * or to the buffer
       */

      if(mode == WRITE_TO_STDOUT){
         if(fwrite(out, 1, have, dest) != have){
            (void)inflateEnd(&strm);
            return Z_ERRNO;
         }
      }
      else {
         memcpy(*buffer+pos, out, have);
         pos += have;
         new_ptr = realloc(*buffer, pos+REALLYBIGBUFSIZE);
         if(!new_ptr){
            (void)inflateEnd(&strm);
            return Z_MEM_ERROR;
         }
         *buffer = new_ptr;
         memset(*buffer+pos, 0, REALLYBIGBUFSIZE);
      }


   } while (strm.avail_out == 0);


   (void)inflateEnd(&strm);

   return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}


int retrieve_file_from_archive(char *filename, int mode, char **buffer, FILE *dest, struct config *cfg){
   int rc=0, n, olen, tlen, len, fd=-1;
   unsigned char *s=NULL, *addr=NULL, inbuf[REALLYBIGBUFSIZE];
   struct stat st;
#if OPENSSL_VERSION_NUMBER < 0x10100000L
   EVP_CIPHER_CTX ctx;
#else
   EVP_CIPHER_CTX *ctx=NULL;
#endif
   int blocklen;


   if(filename == NULL) return 1;


   fd = open(filename, O_RDONLY);
   if(fd == -1){
      syslog(LOG_PRIORITY, "%s: cannot open()", filename);
      return 1;
   }


   if(fstat(fd, &st)){
      syslog(LOG_PRIORITY, "%s: cannot fstat()", filename);
      close(fd);
      return 1;
   }

   // The new encryption scheme uses piler id starting with 5000....

   if(cfg->encrypt_messages == 1){
   #if OPENSSL_VERSION_NUMBER < 0x10100000L
      EVP_CIPHER_CTX_init(&ctx);
      if(strstr(filename, "/5000")){
         rc = EVP_DecryptInit_ex(&ctx, EVP_aes_256_cbc(), NULL, cfg->key, cfg->iv);
      } else {
         rc = EVP_DecryptInit_ex(&ctx, EVP_bf_cbc(), NULL, cfg->key, cfg->iv);
      }

      if(!rc){
         syslog(LOG_PRIORITY, "ERROR: EVP_DecryptInit_ex()");
         goto CLEANUP;
      }

      blocklen = EVP_CIPHER_CTX_block_size(&ctx);
   #else
      ctx = EVP_CIPHER_CTX_new();
      if(!ctx) goto CLEANUP;

      EVP_CIPHER_CTX_init(ctx);
      if(strstr(filename, "/5000")){
         rc = EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, cfg->key, cfg->iv);
      } else {
      #if OPENSSL_VERSION_MAJOR >= 3
         OSSL_PROVIDER_load(NULL, "legacy");
         OSSL_PROVIDER_load(NULL, "default");
      #endif
         rc = EVP_DecryptInit_ex(ctx, EVP_bf_cbc(), NULL, cfg->key, cfg->iv);
      }

      if(!rc){
         syslog(LOG_PRIORITY, "ERROR: EVP_DecryptInit_ex()");
         goto CLEANUP;
      }

      blocklen = EVP_CIPHER_CTX_block_size(ctx);
   #endif

      len = st.st_size+blocklen;

      s = malloc(len);

      if(!s){
         printf("malloc()\n");
         goto CLEANUP;
      }

      tlen = 0;

      while((n = read(fd, inbuf, sizeof(inbuf)))){

      #if OPENSSL_VERSION_NUMBER < 0x10100000L
         if(!EVP_DecryptUpdate(&ctx, s+tlen, &olen, inbuf, n)){
      #else
         if(!EVP_DecryptUpdate(ctx, s+tlen, &olen, inbuf, n)){
      #endif
            syslog(LOG_PRIORITY, "%s: EVP_DecryptUpdate()", filename);
            goto CLEANUP;
         }

         tlen += olen;
      }


   #if OPENSSL_VERSION_NUMBER < 0x10100000L
      if(EVP_DecryptFinal(&ctx, s + tlen, &olen) != 1){
   #else
      if(EVP_DecryptFinal(ctx, s + tlen, &olen) != 1){
   #endif
         syslog(LOG_PRIORITY, "%s: EVP_DecryptFinal()", filename);
         goto CLEANUP;
      }


      tlen += olen;

      // old fileformat with static IV
      rc = inf(s, tlen, mode, buffer, dest);
      // new fileformat, starting with blocklen bytes of garbage
      if(rc != Z_OK && tlen >= blocklen){
         rc = inf(s+blocklen, tlen-blocklen, mode, buffer, dest);
      }
   }
   else {
      addr = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
      rc = inf(addr, st.st_size, mode, buffer, dest);
      munmap(addr, st.st_size);
   }


   if(rc != Z_OK) zerr(rc);


CLEANUP:
   if(fd != -1) close(fd); //-V547
   if(s) free(s);
   if(cfg->encrypt_messages == 1)
   #if OPENSSL_VERSION_NUMBER < 0x10100000L
      EVP_CIPHER_CTX_cleanup(&ctx);
   #else
      if(ctx) EVP_CIPHER_CTX_free(ctx);
   #endif

   return 0;
}


int retrieve_email_from_archive(struct session_data *sdata, FILE *dest, struct config *cfg){
   int attachments;
   char *buffer=NULL, *saved_buffer, *p, filename[SMALLBUFSIZE];
   struct ptr_array ptr_arr[MAX_ATTACHMENTS];
#ifdef HAVE_SUPPORT_FOR_COMPAT_STORAGE_LAYOUT
   struct stat st;
#endif

   if(strlen(sdata->ttmpfile) != RND_STR_LEN){
      printf("invalid piler-id: %s\n", sdata->ttmpfile);
      return 1;
   }

   attachments = query_attachments(sdata, &ptr_arr[0]);

   if(attachments == -1){
      printf("problem querying the attachment of %s\n", sdata->ttmpfile);
      return 1;
   }

   snprintf(filename, sizeof(filename)-1, "%s/%c%c/%c%c%c/%c%c/%c%c/%s.m", cfg->queuedir, *(sdata->ttmpfile+24), *(sdata->ttmpfile+25), *(sdata->ttmpfile+8), *(sdata->ttmpfile+9), *(sdata->ttmpfile+10), *(sdata->ttmpfile+RND_STR_LEN-4), *(sdata->ttmpfile+RND_STR_LEN-3), *(sdata->ttmpfile+RND_STR_LEN-2), *(sdata->ttmpfile+RND_STR_LEN-1), sdata->ttmpfile);
#ifdef HAVE_SUPPORT_FOR_COMPAT_STORAGE_LAYOUT
   if(stat(filename, &st)){
      snprintf(filename, sizeof(filename)-1, "%s/%02x/%c%c/%c%c/%c%c/%s.m", cfg->queuedir, cfg->server_id, *(sdata->ttmpfile+RND_STR_LEN-6), *(sdata->ttmpfile+RND_STR_LEN-5), *(sdata->ttmpfile+RND_STR_LEN-4), *(sdata->ttmpfile+RND_STR_LEN-3), *(sdata->ttmpfile+RND_STR_LEN-2), *(sdata->ttmpfile+RND_STR_LEN-1), sdata->ttmpfile);
   }
#endif

   if(attachments == 0){
      retrieve_file_from_archive(filename, WRITE_TO_STDOUT, &buffer, dest, cfg);
   }
   else {
      retrieve_file_from_archive(filename, WRITE_TO_BUFFER, &buffer, dest, cfg);

      if(buffer){
         saved_buffer = buffer;

         for(int i=1; i<=attachments; i++){
            char pointer[SMALLBUFSIZE];
            snprintf(pointer, sizeof(pointer)-1, "ATTACHMENT_POINTER_%s.a%d_XXX_PILER", sdata->ttmpfile, i);

            p = strstr(buffer, pointer);
            if(p){
               *p = '\0';
               fwrite(buffer, 1, p - buffer, dest);
               buffer = p + strlen(pointer);

               if(strlen(ptr_arr[i].piler_id) == RND_STR_LEN){
                  snprintf(filename, sizeof(filename)-1, "%s/%c%c/%c%c%c/%c%c/%c%c/%s.a%d", cfg->queuedir, ptr_arr[i].piler_id[24], ptr_arr[i].piler_id[25], ptr_arr[i].piler_id[8], ptr_arr[i].piler_id[9], ptr_arr[i].piler_id[10], ptr_arr[i].piler_id[RND_STR_LEN-4], ptr_arr[i].piler_id[RND_STR_LEN-3], ptr_arr[i].piler_id[RND_STR_LEN-2], ptr_arr[i].piler_id[RND_STR_LEN-1], ptr_arr[i].piler_id, ptr_arr[i].attachment_id);

               #ifdef HAVE_SUPPORT_FOR_COMPAT_STORAGE_LAYOUT
                  if(stat(filename, &st)){
                     snprintf(filename, sizeof(filename)-1, "%s/%02x/%c%c/%c%c/%c%c/%s.a%d", cfg->queuedir, cfg->server_id, ptr_arr[i].piler_id[RND_STR_LEN-6], ptr_arr[i].piler_id[RND_STR_LEN-5], ptr_arr[i].piler_id[RND_STR_LEN-4], ptr_arr[i].piler_id[RND_STR_LEN-3], ptr_arr[i].piler_id[RND_STR_LEN-2], ptr_arr[i].piler_id[RND_STR_LEN-1], ptr_arr[i].piler_id, ptr_arr[i].attachment_id);
                  }
               #endif

                  retrieve_file_from_archive(filename, WRITE_TO_STDOUT, NULL, dest, cfg);
               }
            }

         }

         if(buffer){
            fwrite(buffer, 1, strlen(buffer), dest);
         }

         buffer = saved_buffer;
         free(buffer);
      }
   }

   return 0;
}


int recover_email_from_archive(char *filename, struct config *cfg){
   char *buffer=NULL;

   retrieve_file_from_archive(filename, WRITE_TO_BUFFER, &buffer, stdout, cfg);

   char *B = buffer;
   do {
      char line[MAXBUFSIZE];
      int result;

      memset(line, 0, sizeof(line));
      B = split(B, '\n', line, sizeof(line)-1, &result);

      char *p = strstr(line, "ATTACHMENT_POINTER_");
      if(p){
         char *q = strstr(p, "_XXX_PILER");
         if(q){
            *q = '\0';
            char afile[SMALLBUFSIZE];

            char *a = p+strlen("ATTACHMENT_POINTER_");
            snprintf(afile, sizeof(afile)-1, "%s/%c%c/%c%c%c/%c%c/%c%c/%s", cfg->queuedir, *(a+24), *(a+25), *(a+8), *(a+9), *(a+10), *(a+RND_STR_LEN-4), *(a+RND_STR_LEN-3), *(a+RND_STR_LEN-2), *(a+RND_STR_LEN-1), a);

            //printf("afile: %s\n", afile);

            retrieve_file_from_archive(afile, WRITE_TO_STDOUT, NULL, stdout, cfg);

            printf("%s\n", q+strlen("_XXX_PILER"));
         }
      }
      else printf("%s\n", line);

      //if(*line == '\0') continue;

   } while(B);


   //ATTACHMENT_POINTER_500000006789e73628ea923c005452d5ffe5.a1_XXX_PILER

   free(buffer);

   return 0;
}
