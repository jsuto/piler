/*
 * import_pop3.c
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


int import_from_pop3_server(char *server, char *username, char *password, int port, struct session_data *sdata, struct __data *data, int dryrun, struct __config *cfg){
   int rc, ret=OK, sd, use_ssl=0;
   char port_string[8];
   struct addrinfo hints, *res;

   snprintf(port_string, sizeof(port_string)-1, "%d", port);

   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;

   if((rc = getaddrinfo(server, port_string, &hints, &res)) != 0){
      printf("getaddrinfo for '%s': %s\n", server, gai_strerror(rc));
      return ERR;
   }

   if(port == 995) use_ssl = 1;

   if((sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1){
      printf("cannot create socket\n");
      ret = ERR;
      goto ENDE_POP3;
   }

   if(connect(sd, res->ai_addr, res->ai_addrlen) == -1){
      printf("connect()\n");
      ret = ERR;
      goto ENDE_POP3;
   }


   if(connect_to_pop3_server(sd, username, password, data, use_ssl) == ERR){
      close(sd);
      ret = ERR;
      goto ENDE_POP3;
   }

   if(process_pop3_emails(sd, sdata, data, use_ssl, dryrun, cfg) == ERR) ret = ERR;

   close_connection(sd, data, use_ssl);

ENDE_POP3:
   freeaddrinfo(res);

   return ret;
}

