/*
 * imap-seen.c, SJ
 */

#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <locale.h>
#include <getopt.h>
#include <syslog.h>
#include <piler.h>


#define SKIPLIST "junk,trash,spam,draft"
#define MBOX_ARGS 1024

extern char *optarg;
extern int optind;

int quiet=0;

int connect_to_imap_server(int sd, int *seq, char *imapserver, char *username, char *password);


void usage(){
   printf("usage: imap-seen [-c <config file>] -i <imap server> -u <imap username> -p <imap password>\n");
   exit(0);
}


int main(int argc, char **argv){
   int c, rc=0, messages=0;
   char *configfile=CONFIG_FILE;
   char *imapserver=NULL, *username=NULL, *password=NULL, *skiplist=SKIPLIST;
   struct __config cfg;
   int sd, n;
   int seq=1;
   char *p, tag[SMALLBUFSIZE], tagok[SMALLBUFSIZE], buf[MAXBUFSIZE];


   while(1){

#ifdef _GNU_SOURCE
      static struct option long_options[] =
         {
            {"config",       required_argument,  0,  'c' },
            {"imapserver",   required_argument,  0,  'i' },
            {"username",     required_argument,  0,  'u' },
            {"password",     required_argument,  0,  'p' },
            {"skiplist",     required_argument,  0,  'x' },
            {"help",         no_argument,        0,  'h' },
            {0,0,0,0}
         };

      int option_index = 0;

      c = getopt_long(argc, argv, "c:i:u:p:x:h?", long_options, &option_index);
#else
      c = getopt(argc, argv, "c:i:u:p:x:h?");
#endif

      if(c == -1) break;

      switch(c){

         case 'c' :
                    configfile = optarg;
                    break;

         case 'i' :
                    imapserver = optarg;
                    break;

         case 'u' :
                    username = optarg;
                    break;

         case 'p' :
                    password = optarg;
                    break;

         case 'x' :
                    skiplist = optarg;
                    break;

         case 'h' :
         case '?' :
                    usage();
                    break;


         default  : 
                    break;
       }
   }



   if(!imapserver) usage();

   cfg = read_config(configfile);

   (void) openlog("imap-seen", LOG_PID, LOG_MAIL);

   if(imapserver && username && password){

      if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
         printf("cannot create socket\n");
         return ERR;
      }

      if(connect_to_imap_server(sd, &seq, imapserver, username, password) == ERR){
         close(sd);
         return ERR;
      }


      snprintf(tag, sizeof(tag)-1, "A%d", seq); snprintf(tagok, sizeof(tagok)-1, "\r\nA%d OK", (seq)++);
      snprintf(buf, sizeof(buf)-1, "%s SELECT \"INBOX\"\r\n", tag);
      send(sd, buf, strlen(buf), 0);


      n = recvtimeout(sd, buf, MAXBUFSIZE, 10);

      p = strstr(buf, " EXISTS");
      if(p){
         *p = '\0';
         p = strrchr(buf, '\n');
         if(p){
            while(!isdigit(*p)){ p++; }
            messages = atoi(p);
         }
      }

      printf("found %d messages\n", messages);

      snprintf(tag, sizeof(tag)-1, "A%d", seq); snprintf(tagok, sizeof(tagok)-1, "\r\nA%d OK", (seq)++);
      snprintf(buf, sizeof(buf)-1, "%s STORE 1:%d +FLAGS (\\Seen)\r\n", tag, messages);
      send(sd, buf, strlen(buf), 0);

      n = recvtimeout(sd, buf, MAXBUFSIZE, 10);

      close(sd);
   }

   if(quiet == 0) printf("\n");

   return rc;
}


