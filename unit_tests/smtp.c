/*
 * smtp.c, SJ
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
#include <locale.h>
#include <getopt.h>
#include <stdbool.h>
#include <assert.h>
#include "../src/piler.h"


extern char *optarg;
extern int optind;

char *testmessage = "From: aaa@aaa.fu\nTo: bela@aaa.fu\nMessage-Id: ajajajaja\nSubject: this is a test\n\nAaaaaa.";


int connect_to_smtp_server(char *server, int port, int timeout, int use_ssl, struct __data *data);


void usage(){
   printf("\nusage: smtp\n\n");
   printf("    -s <smtp server>                  SMTP server\n");
   printf("    -p <smtp port>                    SMTP port (25)\n");
   printf("    -t <timeout>                      Timeout in sec (10)\n");

   exit(0);
}


void send_smtp_command(int sd, char *cmd, char *buf, int buflen, int timeout, int use_ssl, struct __data *data){
   if(data == NULL || cmd == NULL) return;

   printf("sent: %s", cmd);   
   write1(sd, cmd, strlen(cmd), use_ssl, data->ssl);
   recvtimeoutssl(sd, buf, buflen, timeout, use_ssl, data->ssl);
   printf("rcvd: %s", buf);

}


static void test_smtp_commands_one_at_a_time(char *server, int port, int timeout, int use_ssl, struct __data *data){
   int sd;
   char recvbuf[MAXBUFSIZE], sendbuf[MAXBUFSIZE];

   sd = connect_to_smtp_server(server, port, timeout, use_ssl, data);

   send_smtp_command(sd, "HELO aaaa.fu\r\n", recvbuf, sizeof(recvbuf)-1, timeout, use_ssl, data);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "HELO");

   send_smtp_command(sd, "MAIL FROM: <sender@aaa.fu>\r\n", recvbuf, sizeof(recvbuf)-1, timeout, use_ssl, data);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "MAIL");

   send_smtp_command(sd, "RCPT TO: <archive@aaa.fu>\r\n", recvbuf, sizeof(recvbuf)-1, timeout, use_ssl, data);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "RCPT");

   send_smtp_command(sd, "DATA\r\n", recvbuf, sizeof(recvbuf)-1, timeout, use_ssl, data);
   assert(strncmp(recvbuf, "354 ", 4) == 0 && "DATA");

   snprintf(sendbuf, sizeof(sendbuf)-1, "%s\r\n.\r\n", testmessage);

   send_smtp_command(sd, sendbuf, recvbuf, sizeof(recvbuf)-1, timeout, use_ssl, data);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "PERIOD");

   send_smtp_command(sd, "QUIT\r\n", recvbuf, sizeof(recvbuf)-1, timeout, use_ssl, data);
   assert(strncmp(recvbuf, "221 ", 4) == 0 && "QUIT");

   close(sd);
}


static void test_smtp_commands_pipelining(char *server, int port, int timeout, int use_ssl, struct __data *data){
   int sd;
   char recvbuf[MAXBUFSIZE], sendbuf[MAXBUFSIZE];

   sd = connect_to_smtp_server(server, port, timeout, use_ssl, data);

   send_smtp_command(sd, "HELO aaaa.fu\r\n", recvbuf, sizeof(recvbuf)-1, timeout, use_ssl, data);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "HELO");

   send_smtp_command(sd, "MAIL FROM: <sender@aaa.fu>\r\nRCPT TO: <archive@aaa.fu>\r\nDATA\r\n", recvbuf, sizeof(recvbuf)-1, timeout, use_ssl, data);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "MAIL");

   snprintf(sendbuf, sizeof(sendbuf)-1, "%s\r\n.\r\nQUIT\r\n", testmessage);

   send_smtp_command(sd, sendbuf, recvbuf, sizeof(recvbuf)-1, timeout, use_ssl, data);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "QUIT");

   close(sd);
}


static void test_smtp_commands_with_reset_command(char *server, int port, int timeout, int use_ssl, struct __data *data){
   int sd;
   char recvbuf[MAXBUFSIZE];

   sd = connect_to_smtp_server(server, port, timeout, use_ssl, data);

   send_smtp_command(sd, "HELO aaaa.fu\r\n", recvbuf, sizeof(recvbuf)-1, timeout, use_ssl, data);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "HELO");

   send_smtp_command(sd, "MAIL FROM: <sender@aaa.fu>\r\n", recvbuf, sizeof(recvbuf)-1, timeout, use_ssl, data);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "MAIL");

   send_smtp_command(sd, "RSET\r\n", recvbuf, sizeof(recvbuf)-1, timeout, use_ssl, data);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "RSET");

   send_smtp_command(sd, "RCPT TO: <archive@aaa.fu>\r\n", recvbuf, sizeof(recvbuf)-1, timeout, use_ssl, data);
   assert(strncmp(recvbuf, "503 ", 4) == 0 && "RCPT");

   send_smtp_command(sd, "QUIT\r\n", recvbuf, sizeof(recvbuf)-1, timeout, use_ssl, data);
   assert(strncmp(recvbuf, "221 ", 4) == 0 && "QUIT");

   close(sd);
}


static void test_smtp_commands_partial_command(char *server, int port, int timeout, int use_ssl, struct __data *data){
   int sd;
   char recvbuf[MAXBUFSIZE], sendbuf[MAXBUFSIZE];

   sd = connect_to_smtp_server(server, port, timeout, use_ssl, data);

   send_smtp_command(sd, "HELO aaaa.fu\r\n", recvbuf, sizeof(recvbuf)-1, timeout, use_ssl, data);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "HELO");

   write1(sd, "M", 1, use_ssl, data->ssl);
   printf("sent: M\n");

   send_smtp_command(sd, "AIL FROM: <sender@aaa.fu>\r\n", recvbuf, sizeof(recvbuf)-1, timeout, use_ssl, data);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "MAIL");

   send_smtp_command(sd, "RCPT TO: <archive@aaa.fu>\r\n", recvbuf, sizeof(recvbuf)-1, timeout, use_ssl, data);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "RCPT");

   send_smtp_command(sd, "DATA\r\n", recvbuf, sizeof(recvbuf)-1, timeout, use_ssl, data);
   assert(strncmp(recvbuf, "354 ", 4) == 0 && "DATA");

   snprintf(sendbuf, sizeof(sendbuf)-1, "%s\r\n.\r\n", testmessage);

   send_smtp_command(sd, sendbuf, recvbuf, sizeof(recvbuf)-1, timeout, use_ssl, data);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "PERIOD");

   send_smtp_command(sd, "QUIT\r\n", recvbuf, sizeof(recvbuf)-1, timeout, use_ssl, data);
   assert(strncmp(recvbuf, "221 ", 4) == 0 && "QUIT");

   close(sd);
}


static void test_smtp_commands_partial_command_pipelining(char *server, int port, int timeout, int use_ssl, struct __data *data){
   int sd;
   char recvbuf[MAXBUFSIZE], sendbuf[MAXBUFSIZE];

   sd = connect_to_smtp_server(server, port, timeout, use_ssl, data);

   send_smtp_command(sd, "HELO aaaa.fu\r\n", recvbuf, sizeof(recvbuf)-1, timeout, use_ssl, data);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "HELO");

   write1(sd, "M", 1, use_ssl, data->ssl);
   printf("sent: M\n");

   send_smtp_command(sd, "AIL FROM: <sender@aaa.fu>\r\nRCPT TO: <archive@aaa.fu>\r\nDATA\r\n", recvbuf, sizeof(recvbuf)-1, timeout, use_ssl, data);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "MAIL");

   snprintf(sendbuf, sizeof(sendbuf)-1, "%s\r\n.\r\nQUIT\r\n", testmessage);

   send_smtp_command(sd, sendbuf, recvbuf, sizeof(recvbuf)-1, timeout, use_ssl, data);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "QUIT");

   close(sd);
}


int connect_to_smtp_server(char *server, int port, int timeout, int use_ssl, struct __data *data){
   int rc, sd = -1;
   char port_string[8], buf[MAXBUFSIZE];
   struct addrinfo hints, *res;

   if(data == NULL) return sd;

   snprintf(port_string, sizeof(port_string)-1, "%d", port);

   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;

   if((rc = getaddrinfo(server, port_string, &hints, &res)) != 0){
      printf("getaddrinfo for '%s': %s\n", server, gai_strerror(rc));
      return sd;
   }

   if((sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1){
      printf("cannot create socket\n");
      goto ENDE;
   }

   if(connect(sd, res->ai_addr, res->ai_addrlen) == -1){
      printf("connect()\n");
      goto ENDE;
   }

   recvtimeoutssl(sd, buf, sizeof(buf), timeout, use_ssl, data->ssl);
   printf("rcvd: %s", buf);

ENDE:
   freeaddrinfo(res);

   return sd;
}


int main(int argc, char **argv){
   int c, port=25, timeout=10, use_ssl;
   char *server=NULL;
   struct __data data;

   while(1){

#ifdef _GNU_SOURCE
      static struct option long_options[] =
         {
            {"server",       required_argument,  0,  's' },
            {"port",         required_argument,  0,  'p' },
            {"timeout",      required_argument,  0,  't' },
            {"help",         no_argument,        0,  'h' },
            {0,0,0,0}
         };

      int option_index = 0;

      c = getopt_long(argc, argv, "c:s:p:t:h?", long_options, &option_index);
#else
      c = getopt(argc, argv, "c:s:p:t:h?");
#endif


      if(c == -1) break;

      switch(c){

         case 's' :
                    server = optarg;
                    break;

         case 'p' :
                    port = atoi(optarg);
                    break;

         case 't' :
                    timeout = atoi(optarg);
                    break;

         case 'h' :
         case '?' :
                    usage();
                    break;


         default  : 
                    break;
       }
   }

   if(!server) usage();

   use_ssl = 0;
   data.ssl = NULL;

   test_smtp_commands_one_at_a_time(server, port, timeout, use_ssl, &data);
   test_smtp_commands_pipelining(server, port, timeout, use_ssl, &data);
   test_smtp_commands_with_reset_command(server, port, timeout, use_ssl, &data);
   test_smtp_commands_partial_command(server, port, timeout, use_ssl, &data);
   test_smtp_commands_partial_command_pipelining(server, port, timeout, use_ssl, &data);



   return 0;
}


