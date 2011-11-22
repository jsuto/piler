/*
 * pilerdecrypt.c, SJ
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
#include <piler.h>


char *configfile = CONFIG_FILE;


int main(int argc, char **argv){
   int fd, n, olen, tlen;
   unsigned char inbuf[BIGBUFSIZE], outbuf[BIGBUFSIZE+EVP_MAX_BLOCK_LENGTH];
   EVP_CIPHER_CTX ctx;
   struct __config cfg;

   cfg = read_config(configfile);

   if(read_key(&cfg)){
      printf("%s\n", ERR_READING_KEY);
      return 1;
   }


   EVP_CIPHER_CTX_init(&ctx);
   EVP_DecryptInit_ex(&ctx, EVP_bf_cbc(), NULL, cfg.key, cfg.iv);

   if(argc != 2){
      printf("usage: $0 <encrypted file>\n");
      return 1;
   }

   fd = open(argv[1], O_RDONLY);
   if(fd == -1){
      printf("error reading file: %s\n", argv[0]);
      return 1;
   }


   while((n = read(fd, inbuf, sizeof(inbuf)))){
      bzero(&outbuf, sizeof(outbuf));

      if(EVP_DecryptUpdate(&ctx, outbuf, &olen, inbuf, n) != 1){
         return 0;
      }

      if(EVP_DecryptFinal(&ctx, outbuf + olen, &tlen) != 1){
         return 0;
      }

      olen += tlen;

      write(1, outbuf, olen);
   }

   EVP_CIPHER_CTX_cleanup(&ctx);

   close(fd);

   return 0;
}
