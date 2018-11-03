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
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "../src/piler.h"


extern char *optarg;
extern int optind;

char *testmessage = "From: aaa@aaa.fu\nTo: bela@aaa.fu\nMessage-Id: ajajajaja\nSubject: this is a test\n\nAaaaaa.";

int helo = 0; // 0=HELO, 1=EHLO

void usage(){
   printf("\nusage: smtp\n\n");
   printf("    -s <smtp server>                  SMTP server\n");
   printf("    -p <smtp port>                    SMTP port (25)\n");
   printf("    -t <timeout>                      Timeout in sec (10)\n");

   exit(0);
}


void connect_to_smtp_server(char *server, int port, struct data *data){
   int rc;
   char port_string[8], buf[MAXBUFSIZE];
   struct addrinfo hints, *res;

   data->net->socket = -1;

   if(data == NULL) return;

   snprintf(port_string, sizeof(port_string)-1, "%d", port);

   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;

   if((rc = getaddrinfo(server, port_string, &hints, &res)) != 0){
      printf("getaddrinfo for '%s': %s\n", server, gai_strerror(rc));
      return;
   }

   if((data->net->socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1){
      printf("cannot create socket\n");
      goto ENDE;
   }

   if(connect(data->net->socket, res->ai_addr, res->ai_addrlen) == -1){
      printf("connect()\n");
      goto ENDE;
   }

   recvtimeoutssl(data->net, buf, sizeof(buf));
   printf("rcvd: %s", buf);

ENDE:
   freeaddrinfo(res);
}


void send_smtp_command(struct net *net, char *cmd, char *buf, int buflen){
   if(net == NULL || cmd == NULL) return;

   if(net->socket == -1){
      printf("not connected to remote host\n");
      return;
   }

   printf("sent: %s", cmd);
   write1(net, cmd, strlen(cmd));
   recvtimeoutssl(net, buf, buflen);
   printf("rcvd: %s", buf);
}


void send_helo_command(struct net *net){
   char recvbuf[MAXBUFSIZE];

   if(helo == 0){
      send_smtp_command(net, "HELO aaaa.fu\r\n", recvbuf, sizeof(recvbuf)-1);
      assert(strncmp(recvbuf, "250 ", 4) == 0 && "HELO");
   }
   else {
      send_smtp_command(net, "EHLO aaaa.fu\r\n", recvbuf, sizeof(recvbuf)-1);
      assert(strncmp(recvbuf, "250-", 4) == 0 && "EHLO");
      if(net->use_ssl == 0) assert(strstr(recvbuf, "250-STARTTLS") && "STARTTLS");
      else assert(strstr(recvbuf, "250-STARTTLS") == NULL && "STARTTLS");
   }
}


static void test_smtp_commands_one_at_a_time(char *server, int port, struct data *data){
   char recvbuf[MAXBUFSIZE], sendbuf[MAXBUFSIZE];

   connect_to_smtp_server(server, port, data);

   send_helo_command(data->net);

   send_smtp_command(data->net, "MAIL FROM: <sender@aaa.fu>\r\n", recvbuf, sizeof(recvbuf)-1);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "MAIL");

   send_smtp_command(data->net, "RCPT TO: <archive@aaa.fu>\r\n", recvbuf, sizeof(recvbuf)-1);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "RCPT");

   send_smtp_command(data->net, "DATA\r\n", recvbuf, sizeof(recvbuf)-1);
   assert(strncmp(recvbuf, "354 ", 4) == 0 && "DATA");

   snprintf(sendbuf, sizeof(sendbuf)-1, "%s\r\n.\r\n", testmessage);

   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "PERIOD");

   send_smtp_command(data->net, "QUIT\r\n", recvbuf, sizeof(recvbuf)-1);
   assert(strncmp(recvbuf, "221 ", 4) == 0 && "QUIT");

   close(data->net->socket);
}


static void test_smtp_commands_pipelining(char *server, int port, struct data *data){
   char recvbuf[MAXBUFSIZE], sendbuf[MAXBUFSIZE];

   connect_to_smtp_server(server, port, data);

   send_helo_command(data->net);

   send_smtp_command(data->net, "MAIL FROM: <sender@aaa.fu>\r\nRCPT TO: <archive@aaa.fu>\r\nDATA\r\n", recvbuf, sizeof(recvbuf)-1);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "MAIL");

   snprintf(sendbuf, sizeof(sendbuf)-1, "%s\r\n.\r\nQUIT\r\n", testmessage);

   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "QUIT");

   close(data->net->socket);
}


static void test_smtp_commands_with_reset_command(char *server, int port, struct data *data){
   char recvbuf[MAXBUFSIZE];

   connect_to_smtp_server(server, port, data);

   send_helo_command(data->net);

   send_smtp_command(data->net, "MAIL FROM: <sender@aaa.fu>\r\n", recvbuf, sizeof(recvbuf)-1);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "MAIL");

   send_smtp_command(data->net, "RSET\r\n", recvbuf, sizeof(recvbuf)-1);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "RSET");

   send_smtp_command(data->net, "RCPT TO: <archive@aaa.fu>\r\n", recvbuf, sizeof(recvbuf)-1);
   assert(strncmp(recvbuf, "503 ", 4) == 0 && "RCPT");

   send_smtp_command(data->net, "QUIT\r\n", recvbuf, sizeof(recvbuf)-1);
   assert(strncmp(recvbuf, "221 ", 4) == 0 && "QUIT");

   close(data->net->socket);
}


static void test_smtp_commands_partial_command(char *server, int port, struct data *data){
   char recvbuf[MAXBUFSIZE], sendbuf[MAXBUFSIZE];

   connect_to_smtp_server(server, port, data);

   send_helo_command(data->net);

   write1(data->net, "M", 1);
   printf("sent: M\n");

   send_smtp_command(data->net, "AIL FROM: <sender@aaa.fu>\r\n", recvbuf, sizeof(recvbuf)-1);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "MAIL");

   send_smtp_command(data->net, "RCPT TO: <archive@aaa.fu>\r\n", recvbuf, sizeof(recvbuf)-1);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "RCPT");

   send_smtp_command(data->net, "DATA\r\n", recvbuf, sizeof(recvbuf)-1);
   assert(strncmp(recvbuf, "354 ", 4) == 0 && "DATA");

   snprintf(sendbuf, sizeof(sendbuf)-1, "%s\r\n.\r\n", testmessage);

   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "PERIOD");

   send_smtp_command(data->net, "QUIT\r\n", recvbuf, sizeof(recvbuf)-1);
   assert(strncmp(recvbuf, "221 ", 4) == 0 && "QUIT");

   close(data->net->socket);
}


static void test_smtp_commands_partial_command_pipelining(char *server, int port, struct data *data){
   char recvbuf[MAXBUFSIZE], sendbuf[MAXBUFSIZE];

   connect_to_smtp_server(server, port, data);

   send_helo_command(data->net);

   write1(data->net, "M", 1);
   printf("sent: M\n");

   send_smtp_command(data->net, "AIL FROM: <sender@aaa.fu>\r\nRCPT TO: <archive@aaa.fu>\r\nDATA\r\n", recvbuf, sizeof(recvbuf)-1);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "MAIL");

   snprintf(sendbuf, sizeof(sendbuf)-1, "%s\r\n.\r\nQUIT\r\n", testmessage);

   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "QUIT");

   close(data->net->socket);
}


static void test_smtp_commands_starttls(char *server, int port, struct data *data){
   char recvbuf[MAXBUFSIZE], sendbuf[MAXBUFSIZE];

   connect_to_smtp_server(server, port, data);

   send_helo_command(data->net);

   send_smtp_command(data->net, "STARTTLS\r\n", recvbuf, sizeof(recvbuf)-1);
   assert(strncmp(recvbuf, "220 ", 4) == 0 && "STARTTLS");

   init_ssl_to_server(data);
   data->net->use_ssl = 1;

   send_helo_command(data->net);

   send_smtp_command(data->net, "MAIL FROM: <sender@aaa.fu>\r\n", recvbuf, sizeof(recvbuf)-1);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "MAIL");

   send_smtp_command(data->net, "RCPT TO: <archive@aaa.fu>\r\n", recvbuf, sizeof(recvbuf)-1);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "RCPT");

   send_smtp_command(data->net, "DATA\r\n", recvbuf, sizeof(recvbuf)-1);
   assert(strncmp(recvbuf, "354 ", 4) == 0 && "DATA");

   snprintf(sendbuf, sizeof(sendbuf)-1, "%s\r\n.\r\n", testmessage);

   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "PERIOD");

   send_smtp_command(data->net, "QUIT\r\n", recvbuf, sizeof(recvbuf)-1);
   assert(strncmp(recvbuf, "221 ", 4) == 0 && "QUIT");

   close(data->net->socket);
}


static void test_smtp_commands_period_command_in_2_parts(char *server, int port, char *part1, char *part2, struct data *data){
   char recvbuf[MAXBUFSIZE], sendbuf[MAXBUFSIZE];

   connect_to_smtp_server(server, port, data);

   send_helo_command(data->net);

   send_smtp_command(data->net, "MAIL FROM: <sender@aaa.fu>\r\nRCPT TO: <archive@aaa.fu>\r\nDATA\r\n", recvbuf, sizeof(recvbuf)-1);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "MAIL");

   snprintf(sendbuf, sizeof(sendbuf)-1, "%s%s", testmessage, part1);
   write1(data->net, sendbuf, strlen(sendbuf));

   snprintf(sendbuf, sizeof(sendbuf), "%s", part2);
   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1);

   snprintf(sendbuf, sizeof(sendbuf)-1, "QUIT\r\n");
   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1);

   assert(strncmp(recvbuf, "250 ", 4) == 0 && "QUIT");

   close(data->net->socket);
}


static void test_smtp_commands_period_command_in_its_own_packet(char *server, int port, struct data *data){
   char recvbuf[MAXBUFSIZE], sendbuf[MAXBUFSIZE];

   connect_to_smtp_server(server, port, data);

   send_helo_command(data->net);

   send_smtp_command(data->net, "MAIL FROM: <sender@aaa.fu>\r\nRCPT TO: <archive@aaa.fu>\r\nDATA\r\n", recvbuf, sizeof(recvbuf)-1);
   assert(strncmp(recvbuf, "250 ", 4) == 0 && "MAIL");

   snprintf(sendbuf, sizeof(sendbuf)-1, "%s", testmessage);
   write1(data->net, sendbuf, strlen(sendbuf));

   snprintf(sendbuf, sizeof(sendbuf), "\r\n.\r\n");
   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1);

   snprintf(sendbuf, sizeof(sendbuf)-1, "QUIT\r\n");
   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1);

   assert(strncmp(recvbuf, "250 ", 4) == 0 && "QUIT");

   close(data->net->socket);
}


int main(int argc, char **argv){
   int c, port=25;
   char *server=NULL;
   struct data data;
   struct net net;

   net.timeout = 10;
   net.use_ssl = 0;
   net.ssl = NULL;

   while(1){

#ifdef _GNU_SOURCE
      static struct option long_options[] =
         {
            {"server",       required_argument,  0,  's' },
            {"port",         required_argument,  0,  'p' },
            {"timeout",      required_argument,  0,  't' },
            {"lhlo",         no_argument,        0,  'l' },
            {"help",         no_argument,        0,  'h' },
            {0,0,0,0}
         };

      int option_index = 0;

      c = getopt_long(argc, argv, "c:s:p:t:lh?", long_options, &option_index);
#else
      c = getopt(argc, argv, "c:s:p:t:lh?");
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
                    net.timeout = atoi(optarg);
                    break;

         case 'l' :
                    helo = 1;
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

   data.net = &net;

   test_smtp_commands_one_at_a_time(server, port, &data);
   test_smtp_commands_pipelining(server, port, &data);
   test_smtp_commands_with_reset_command(server, port, &data);
   test_smtp_commands_partial_command(server, port, &data);
   test_smtp_commands_partial_command_pipelining(server, port, &data);
   test_smtp_commands_period_command_in_2_parts(server, port, "\r", "\n.\r\n", &data);
   test_smtp_commands_period_command_in_2_parts(server, port, "\r\n", ".\r\n", &data);
   test_smtp_commands_period_command_in_2_parts(server, port, "\r\n.", "\r\n", &data);
   test_smtp_commands_period_command_in_2_parts(server, port, "\r\n.\r", "\n", &data);
   test_smtp_commands_period_command_in_its_own_packet(server, port, &data);

   helo = 1; // we must use EHLO to get the STARTTLS in the response
   test_smtp_commands_starttls(server, port, &data);


   return 0;
}


