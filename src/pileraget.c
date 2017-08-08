/*
 * pileraget.c, SJ
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
#include <piler.h>


int main(int argc, char **argv){
   int readkey=1;
   char filename[SMALLBUFSIZE];
#ifdef HAVE_SUPPORT_FOR_COMPAT_STORAGE_LAYOUT
   struct stat st;
#endif
   struct config cfg;


   if(argc < 3){
      printf("usage: %s <piler-id> <attachment-id>\n", argv[0]);
      exit(1);
   }


   cfg = read_config(CONFIG_FILE);

   if(argc >= 4) readkey = 0;

   if(readkey == 1 && read_key(&cfg)){
      printf("%s\n", ERR_READING_KEY);
      return 1;
   }


   if(strlen(argv[1]) != RND_STR_LEN){
      printf("invalid piler id: '%s'\n", argv[1]);
      return 1;
   }

   snprintf(filename, sizeof(filename)-1, "%s/%02x/%c%c%c/%c%c/%c%c/%s.a%d", cfg.queuedir, cfg.server_id, argv[1][8], argv[1][9], argv[1][10], argv[1][RND_STR_LEN-4], argv[1][RND_STR_LEN-3], argv[1][RND_STR_LEN-2], argv[1][RND_STR_LEN-1], argv[1], atoi(argv[2]));
#ifdef HAVE_SUPPORT_FOR_COMPAT_STORAGE_LAYOUT
   if(stat(filename, &st)){
      snprintf(filename, sizeof(filename)-1, "%s/%02x/%c%c/%c%c/%c%c/%s.a%d", cfg.queuedir, cfg.server_id, argv[1][RND_STR_LEN-6], argv[1][RND_STR_LEN-5], argv[1][RND_STR_LEN-4], argv[1][RND_STR_LEN-3], argv[1][RND_STR_LEN-2], argv[1][RND_STR_LEN-1], argv[1], atoi(argv[2]));
   }
#endif

   retrieve_file_from_archive(filename, WRITE_TO_STDOUT, NULL, stdout, &cfg);

   return 0;
}


