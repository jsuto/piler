/*
 * import_imap.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>
#include <getopt.h>
#include <syslog.h>
#include <piler.h>


int import_from_imap_server(char *server, char *username, char *password, int port, struct session_data *sdata, struct __data *data, char *folder_imap, char *skiplist, int dryrun, struct __config *cfg){
   int i, rc=ERR, ret=OK, sd, seq=1, skipmatch, use_ssl=0;
   char port_string[8], puf[SMALLBUFSIZE];
   struct addrinfo hints, *res;
   struct node *q;


   inithash(data->imapfolders);

   snprintf(port_string, sizeof(port_string)-1, "%d", port);

   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;

   if((rc = getaddrinfo(server, port_string, &hints, &res)) != 0){
      printf("getaddrinfo for '%s': %s\n", server, gai_strerror(rc));
      return ERR;
   }

   if(port == 993) use_ssl = 1;


   if((sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1){
      printf("cannot create socket\n");
      ret = ERR;
      goto ENDE_IMAP;
   }

   if(connect(sd, res->ai_addr, res->ai_addrlen) == -1){
      printf("connect()\n");
      ret = ERR;
      goto ENDE_IMAP;
   }

   if(connect_to_imap_server(sd, &seq, username, password, data, use_ssl) == ERR){
      close(sd);
      ret = ERR;
      goto ENDE_IMAP;
   }


   if(list_folders(sd, &seq, use_ssl, folder_imap, data) == ERR) goto ENDE_IMAP;

   for(i=0;i<MAXHASH;i++){
      q = data->imapfolders[i];
      while(q != NULL){

         if(q && q->str && strlen(q->str) > 1){

            skipmatch = 0;

            if(skiplist && strlen(skiplist) > 0){
               snprintf(puf, sizeof(puf)-1, "%s,", (char *)q->str);
               if(strstr(skiplist, puf)) skipmatch = 1;
            }

            if(skipmatch == 1){
               if(data->quiet == 0) printf("SKIPPING FOLDER: %s\n", (char *)q->str);
            }
            else {
               if(data->quiet == 0) printf("processing folder: %s... ", (char *)q->str);

               if(process_imap_folder(sd, &seq, q->str, sdata, data, use_ssl, dryrun, cfg) == ERR) ret = ERR;
            }

         }

         q = q->r;

      }
   }

   send_imap_close(sd, &seq, data, use_ssl);

   close_connection(sd, data, use_ssl);

ENDE_IMAP:
   freeaddrinfo(res);

   clearhash(data->imapfolders);

   data->import->status = 2;

   return ret;
}


