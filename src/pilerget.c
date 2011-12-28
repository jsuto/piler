/*
 * pilerget.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>
#include <syslog.h>
#include <openssl/blowfish.h>
#include <openssl/evp.h>
#include <zlib.h>
#include <assert.h>
#include <piler.h>


#define WRITE_TO_STDOUT 0
#define WRITE_TO_BUFFER 1
#define REALLYBIGBUFSIZE 524288

int fd=-1;
EVP_CIPHER_CTX ctx;
unsigned char *s=NULL;



void clean_exit(){
   if(s) free(s);

   EVP_CIPHER_CTX_cleanup(&ctx);

   if(fd != -1) close(fd);

   exit(0);
}


void zerr(int ret){
   fputs("zpipe: ", stderr);
   switch (ret) {
   case Z_ERRNO:
        if (ferror(stdin))
            fputs("error reading stdin\n", stderr);
        if (ferror(stdout))
            fputs("error writing stdout\n", stderr);
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
   }
}


int inf(unsigned char *in, int len, int mode, char **buffer, FILE *dest){
   int ret, pos=0;
   unsigned have;
   z_stream strm;
   char *new_ptr;
   unsigned char out[REALLYBIGBUFSIZE];

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

      have = REALLYBIGBUFSIZE - strm.avail_out;

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


int retrieve_file_from_archive(char *filename, int mode, char **buffer, FILE *dest, struct __config *cfg){
   int rc=0, n, olen, tlen, len;
   unsigned char inbuf[REALLYBIGBUFSIZE];
   struct stat st;


   if(filename == NULL) return 1;


   fd = open(filename, O_RDONLY);
   if(fd == -1){
      printf("error reading file: %s\n", filename);
      return 1;
   }


   if(fstat(fd, &st)){
      perror("fstat()");
      close(fd);
      return 1;
   }


   EVP_CIPHER_CTX_init(&ctx);
   EVP_DecryptInit_ex(&ctx, EVP_bf_cbc(), NULL, cfg->key, cfg->iv);

   len = st.st_size+EVP_MAX_BLOCK_LENGTH;

   s = malloc(len);

   if(!s){
      perror("malloc");
      clean_exit();
   }


   tlen = 0;

   while((n = read(fd, inbuf, sizeof(inbuf)))){

      if(!EVP_DecryptUpdate(&ctx, s+tlen, &olen, inbuf, n)){
         fprintf(stderr, "EVP_DecryptUpdate()\n");
         clean_exit();
      }

      tlen += olen;
   }

   close(fd);

   if(EVP_DecryptFinal(&ctx, s + tlen, &olen) != 1){
      fprintf(stderr, "EVP_DecryptFinal()\n");
      clean_exit();
   }

   EVP_CIPHER_CTX_cleanup(&ctx);

   tlen += olen;


   rc = inf(s, tlen, mode, buffer, dest);
   if(rc != Z_OK) zerr(rc);


   if(s) free(s);

   return 0;
}


int retrieve_email_from_archive(struct session_data *sdata, FILE *dest, struct __config *cfg){
   int i, rc, attachments;
   char *buffer=NULL, *saved_buffer, *p, filename[SMALLBUFSIZE], pointer[SMALLBUFSIZE];
   struct ptr_array ptr_arr[MAX_ATTACHMENTS];

   if(strlen(sdata->ttmpfile) != RND_STR_LEN){
      fprintf(stderr, "invalid piler-id: %s\n", sdata->ttmpfile);
      return 1;
   }

   attachments = query_attachments(sdata, &ptr_arr[0], cfg);

   if(attachments == -1){
      fprintf(stderr, "problem querying the attachment of %s\n", sdata->ttmpfile);
      return 1;
   }

   snprintf(filename, sizeof(filename)-1, "%s/%c%c/%c%c/%c%c/%s.m", cfg->queuedir, *(sdata->ttmpfile+RND_STR_LEN-6), *(sdata->ttmpfile+RND_STR_LEN-5), *(sdata->ttmpfile+RND_STR_LEN-4), *(sdata->ttmpfile+RND_STR_LEN-3), *(sdata->ttmpfile+RND_STR_LEN-2), *(sdata->ttmpfile+RND_STR_LEN-1), sdata->ttmpfile);

   if(attachments == 0){
      rc = retrieve_file_from_archive(filename, WRITE_TO_STDOUT, &buffer, dest, cfg);
   }
   else {
      rc = retrieve_file_from_archive(filename, WRITE_TO_BUFFER, &buffer, dest, cfg);

      if(buffer){
         saved_buffer = buffer;

         for(i=1; i<=attachments; i++){
            snprintf(pointer, sizeof(pointer)-1, "ATTACHMENT_POINTER_%s.a%d_XXX_PILER", sdata->ttmpfile, i);

            p = strstr(buffer, pointer);
            if(p){
               *p = '\0';
               //printf("%s", buffer);
               fwrite(buffer, 1, p - buffer, dest);
               buffer = p + strlen(pointer);

               if(strlen(ptr_arr[i].piler_id) == RND_STR_LEN){
                  snprintf(filename, sizeof(filename)-1, "%s/%c%c/%c%c/%c%c/%s.a%d", cfg->queuedir, ptr_arr[i].piler_id[RND_STR_LEN-6], ptr_arr[i].piler_id[RND_STR_LEN-5], ptr_arr[i].piler_id[RND_STR_LEN-4], ptr_arr[i].piler_id[RND_STR_LEN-3], ptr_arr[i].piler_id[RND_STR_LEN-2], ptr_arr[i].piler_id[RND_STR_LEN-1], ptr_arr[i].piler_id, ptr_arr[i].attachment_id);

                  rc = retrieve_file_from_archive(filename, WRITE_TO_STDOUT, NULL, dest, cfg);
               }
            }

         }

         if(buffer){
            //printf("%s", buffer);
            fwrite(buffer, 1, strlen(buffer), dest);
         }

         buffer = saved_buffer;
         free(buffer);
      }
   }

   return 0;
}


int main(int argc, char **argv){
   int rc;
   char filename[SMALLBUFSIZE];
   FILE *f;
   struct session_data sdata;
   struct __config cfg;


   if(argc < 2){
      fprintf(stderr, "usage: %s <piler-id>\n", argv[0]);
      exit(1);
   }


   cfg = read_config(CONFIG_FILE);


   if(read_key(&cfg)){
      printf("%s\n", ERR_READING_KEY);
      return 1;
   }


   mysql_init(&(sdata.mysql));
   mysql_options(&(sdata.mysql), MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&cfg.mysql_connect_timeout);
   if(mysql_real_connect(&(sdata.mysql), cfg.mysqlhost, cfg.mysqluser, cfg.mysqlpwd, cfg.mysqldb, cfg.mysqlport, cfg.mysqlsocket, 0) == 0){
      printf("cannot connect to mysql server\n");
      return 0;
   }

   mysql_real_query(&(sdata.mysql), "SET NAMES utf8", strlen("SET NAMES utf8"));
   mysql_real_query(&(sdata.mysql), "SET CHARACTER SET utf8", strlen("SET CHARACTER SET utf8"));


   if(argv[1][0] == '-'){

      while((rc = read(0, sdata.ttmpfile, RND_STR_LEN+1)) > 0){
         trimBuffer(sdata.ttmpfile);
         snprintf(filename, sizeof(filename)-1, "%s.eml", sdata.ttmpfile);
         f = fopen(filename, "w");
         if(f){
            rc = retrieve_email_from_archive(&sdata, f, &cfg);
            fclose(f);
         }
         else printf("cannot open: %s\n", filename);
      }

   }
   else {
      snprintf(sdata.ttmpfile, SMALLBUFSIZE-1, "%s", argv[1]);
      rc = retrieve_email_from_archive(&sdata, stdout, &cfg);
   }


   mysql_close(&(sdata.mysql));

   return 0;
}


