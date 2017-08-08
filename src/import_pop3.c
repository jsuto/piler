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


void import_from_pop3_server(struct session_data *sdata, struct data *data, struct config *cfg){
   int rc;
   char port_string[8];
   struct addrinfo hints, *res;

   data->net->use_ssl = 0;

   snprintf(port_string, sizeof(port_string)-1, "%d", data->import->port);

   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;

   if((rc = getaddrinfo(data->import->server, port_string, &hints, &res)) != 0){
      printf("getaddrinfo for '%s': %s\n", data->import->server, gai_strerror(rc));
      return;
   }

   if(data->import->port == 995) data->net->use_ssl = 1;

   if((data->net->socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1){
      printf("cannot create socket\n");
      goto ENDE_POP3;
   }

   if(connect(data->net->socket, res->ai_addr, res->ai_addrlen) == -1){
      printf("connect()\n");
      goto ENDE_POP3;
   }


   if(connect_to_pop3_server(data) == ERR){
      close(data->net->socket);
      goto ENDE_POP3;
   }

   process_pop3_emails(sdata, data, cfg);

   close_connection(data->net);

ENDE_POP3:
   freeaddrinfo(res);
}
