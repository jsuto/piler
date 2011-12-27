/*
 * pilerget.c, SJ
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
#include <openssl/blowfish.h>
#include <openssl/evp.h>
#include <zlib.h>
#include <assert.h>
#include <piler.h>


char *configfile = CONFIG_FILE;

int fd=-1;
EVP_CIPHER_CTX ctx;
unsigned char *s=NULL;


void clean_exit(){
   if(s) free(s);

   EVP_CIPHER_CTX_cleanup(&ctx);

   if(fd != -1) close(fd);

   exit(0);
}


void zerr(int ret)
{
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


int inf(unsigned char *in, int len, FILE *dest){
   int ret;
   unsigned have;
   z_stream strm;
   unsigned char out[BIGBUFSIZE];

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

   do {
      strm.avail_out = BIGBUFSIZE;
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

      have = BIGBUFSIZE - strm.avail_out;
      if(fwrite(out, 1, have, dest) != have){
         (void)inflateEnd(&strm);
         return Z_ERRNO;
      }

   } while (strm.avail_out == 0);


   (void)inflateEnd(&strm);

   return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}


int retrieve_file_from_archive(char *filename, struct __config *cfg){
   int rc, n, olen, tlen, len;
   unsigned char inbuf[BIGBUFSIZE];
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


   rc = inf(s, tlen, stdout); if(rc != Z_OK) zerr(rc);


   if(s) free(s);

   return 0;
}


int main(int argc, char **argv){
   struct __config cfg;


   cfg = read_config(configfile);

   if(read_key(&cfg)){
      printf("%s\n", ERR_READING_KEY);
      return 1;
   }


   if(argc != 2){
      printf("usage: %s <encrypted file>\n", argv[0]);
      return 1;
   }


   retrieve_file_from_archive(argv[1], &cfg);

   return 0;
}
