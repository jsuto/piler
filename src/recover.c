/*
 * recover.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>
#include <syslog.h>
#include <getopt.h>
#include <piler.h>


void usage(){
   printf("\nusage: recover\n\n");

   printf("    [-c <config file>]                Config file to use if not the default\n");
   printf("    -f <stored .m file>               Stored .m file\n");

   exit(0);
}


int recover_email_from_archive(char *filename, struct config *cfg);

int main(int argc, char **argv){
   struct config cfg;
   char *configfile=CONFIG_FILE, *filename=NULL;

   while(1){

#ifdef _GNU_SOURCE
      static struct option long_options[] =
         {
            {"config",          required_argument,  0,  'c' },
            {"file",            required_argument,  0,  'f' },
            {0,0,0,0}
         };

      int option_index = 0;


      int c = getopt_long(argc, argv, "c:f:hv?", long_options, &option_index);
#else
      int c = getopt(argc, argv, "c:f:hv?");
#endif

      if(c == -1) break;

      switch(c){

         case 'c' :
                    configfile = optarg;
                    break;

         case 'f' :
                    filename = optarg;
                    break;

         default  :
                    usage();
                    break;
      }

   }


   if(filename == NULL) usage();

   cfg = read_config(configfile);

   if(read_key(&cfg)){
      printf("%s\n", ERR_READING_KEY);
      return 1;
   }

   recover_email_from_archive(filename, &cfg);

   return 0;
}
