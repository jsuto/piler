/*
 * smtp.c, SJ
 */

#include "test.h"

extern char *optarg;
extern int optind;

char *testmessage = "From: aaa@aaa.fu\r\nTo: bela@aaa.fu\r\nMessage-Id: ajajajaja\r\nSubject: this is a test\r\n\r\nAaaaaa";
char *testmessage2 = " and the last line\r\n";

char *recipient = "aaa@worker0";

int helo = 0; // 0=HELO, 1=EHLO

void usage(){
   printf("\nusage: smtp\n\n");
   printf("    -s <smtp server>                  SMTP server\n");
   printf("    -p <smtp port>                    SMTP port (25)\n");
   printf("    -r <recipient>                    Envelope recipient\n");
   printf("    -t <timeout>                      Timeout in sec (10)\n");

   exit(0);
}


int countreplies(char *s){
   int replies = 0;

   for(; *s; s++){
      if(*s == '\n') replies++;
   }

   return replies;
}


void connect_to_smtp_server(char *server, int port, struct data *data){
   int rc;
   char port_string[8], buf[MAXBUFSIZE];
   struct addrinfo hints, *res;

   if(data == NULL) return;

   data->net->socket = -1;

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

ENDE:
   freeaddrinfo(res);
}


void send_smtp_command(struct net *net, char *cmd, char *buf, int buflen, int expectedreplies){
   int tot=0;

   if(net == NULL || cmd == NULL) return;

   if(net->socket == -1){
      printf("not connected to remote host\n");
      return;
   }

   write1(net, cmd, strlen(cmd));

   while(1){
      tot += recvtimeoutssl(net, buf+tot, buflen);
      if(countreplies(buf) == expectedreplies) break;
   }
}


void send_helo_command(struct net *net){
   int replies=1;
   char recvbuf[MAXBUFSIZE];

   if(helo == 0){
      send_smtp_command(net, "HELO aaaa.fu\r\n", recvbuf, sizeof(recvbuf)-1, replies);
      ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);
   }
   else {
      //replies = 6;
      replies = 5;
      if(net->use_ssl == 1) replies--;

      send_smtp_command(net, "EHLO aaaa.fu\r\n", recvbuf, sizeof(recvbuf)-1, replies);
      ASSERT(strncmp(recvbuf, "250-", 4) == 0, recvbuf);
      if(net->use_ssl == 0){ ASSERT(strstr(recvbuf, "250-STARTTLS"), recvbuf); }
      else { ASSERT(strstr(recvbuf, "250-STARTTLS") == NULL, recvbuf); }
   }
}


static void test_smtp_commands_one_at_a_time(char *server, int port, struct data *data){
   char recvbuf[MAXBUFSIZE], sendbuf[MAXBUFSIZE];

   TEST_HEADER();

   connect_to_smtp_server(server, port, data);

   send_helo_command(data->net);

   send_smtp_command(data->net, "MAIL FROM: <sender@aaa.fu>\r\n", recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   snprintf(sendbuf, sizeof(sendbuf)-1, "RCPT TO: <%s>\r\n", recipient);
   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   send_smtp_command(data->net, "DATA\r\n", recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "354 ", 4) == 0, recvbuf);

   snprintf(sendbuf, sizeof(sendbuf)-1, "%s\r\n.\r\n", testmessage);

   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   send_smtp_command(data->net, "QUIT\r\n", recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "221 ", 4) == 0, recvbuf);

   close(data->net->socket);

   TEST_FOOTER();
}


static void test_smtp_commands_one_at_a_time_data_in_2_parts(char *server, int port, struct data *data){
   char recvbuf[MAXBUFSIZE], sendbuf[MAXBUFSIZE];

   TEST_HEADER();

   connect_to_smtp_server(server, port, data);

   send_helo_command(data->net);

   send_smtp_command(data->net, "MAIL FROM: <sender@aaa.fu>\r\n", recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   snprintf(sendbuf, sizeof(sendbuf)-1, "RCPT TO: <%s>\r\n", recipient);
   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   send_smtp_command(data->net, "DATA\r\n", recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "354 ", 4) == 0, recvbuf);

   write1(data->net, testmessage, strlen(testmessage));

   snprintf(sendbuf, sizeof(sendbuf)-1, "%s\r\n.\r\n", testmessage2);
   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   send_smtp_command(data->net, "QUIT\r\n", recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "221 ", 4) == 0, recvbuf);

   close(data->net->socket);

   TEST_FOOTER();
}


/*static void test_smtp_commands_pipelining(char *server, int port, struct data *data){
   char recvbuf[MAXBUFSIZE], sendbuf[MAXBUFSIZE];

   TEST_HEADER();

   connect_to_smtp_server(server, port, data);

   send_helo_command(data->net);

   snprintf(sendbuf, sizeof(sendbuf)-1, "MAIL FROM: <sender@aaa.fu>\r\nRCPT TO: <%s>\r\nDATA\r\n", recipient);
   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1, 3);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   snprintf(sendbuf, sizeof(sendbuf)-1, "%s\r\n.\r\nQUIT\r\n", testmessage);

   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1, 2);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   close(data->net->socket);

   TEST_FOOTER();
}*/


static void test_smtp_commands_with_reset_command(char *server, int port, struct data *data){
   char recvbuf[MAXBUFSIZE], sendbuf[MAXBUFSIZE];

   TEST_HEADER();

   connect_to_smtp_server(server, port, data);

   send_helo_command(data->net);

   send_smtp_command(data->net, "MAIL FROM: <sender@aaa.fu>\r\n", recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   send_smtp_command(data->net, "RSET\r\n", recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   snprintf(sendbuf, sizeof(sendbuf)-1, "RCPT TO: <%s>\r\n", recipient);
   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "503 ", 4) == 0, recvbuf);

   send_smtp_command(data->net, "QUIT\r\n", recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "221 ", 4) == 0, recvbuf);

   close(data->net->socket);

   TEST_FOOTER();
}


static void test_smtp_commands_partial_command(char *server, int port, struct data *data){
   char recvbuf[MAXBUFSIZE], sendbuf[MAXBUFSIZE];

   TEST_HEADER();

   connect_to_smtp_server(server, port, data);

   send_helo_command(data->net);

   write1(data->net, "M", 1);

   send_smtp_command(data->net, "AIL FROM: <sender@aaa.fu>\r\n", recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   snprintf(sendbuf, sizeof(sendbuf)-1, "RCPT TO: <%s>\r\n", recipient);
   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   send_smtp_command(data->net, "DATA\r\n", recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "354 ", 4) == 0, recvbuf);

   snprintf(sendbuf, sizeof(sendbuf)-1, "%s\r\n.\r\n", testmessage);

   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   send_smtp_command(data->net, "QUIT\r\n", recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "221 ", 4) == 0, recvbuf);

   close(data->net->socket);

   TEST_FOOTER();
}


/*static void test_smtp_commands_partial_command_pipelining(char *server, int port, struct data *data){
   char recvbuf[MAXBUFSIZE], sendbuf[MAXBUFSIZE];

   TEST_HEADER();

   connect_to_smtp_server(server, port, data);

   send_helo_command(data->net);

   write1(data->net, "M", 1);

   snprintf(sendbuf, sizeof(sendbuf)-1, "AIL FROM: <sender@aaa.fu>\r\nRCPT TO: <%s>\r\nDATA\r\n", recipient);
   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1, 3);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   snprintf(sendbuf, sizeof(sendbuf)-1, "%s\r\n.\r\nQUIT\r\n", testmessage);

   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1, 2);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   close(data->net->socket);

   TEST_FOOTER();
}*/


static void test_smtp_commands_starttls(char *server, int port, struct data *data){
   char recvbuf[MAXBUFSIZE], sendbuf[MAXBUFSIZE];

   TEST_HEADER();

   connect_to_smtp_server(server, port, data);

   send_helo_command(data->net);

   send_smtp_command(data->net, "STARTTLS\r\n", recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "220 ", 4) == 0, recvbuf);

   init_ssl_to_server(data);
   data->net->use_ssl = 1;

   send_helo_command(data->net);

   send_smtp_command(data->net, "MAIL FROM: <sender@aaa.fu>\r\n", recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   snprintf(sendbuf, sizeof(sendbuf)-1, "RCPT TO: <%s>\r\n", recipient);
   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   send_smtp_command(data->net, "DATA\r\n", recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "354 ", 4) == 0, recvbuf);

   snprintf(sendbuf, sizeof(sendbuf)-1, "%s\r\n.\r\n", testmessage);

   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   send_smtp_command(data->net, "QUIT\r\n", recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "221 ", 4) == 0, recvbuf);

   close(data->net->socket);

   TEST_FOOTER();
}


static void test_smtp_commands_period_command_in_2_parts(char *server, int port, char *part1, char *part2, struct data *data){
   char recvbuf[MAXBUFSIZE], sendbuf[MAXBUFSIZE];

   TEST_HEADER();

   connect_to_smtp_server(server, port, data);

   send_helo_command(data->net);

   // As long as pipelining support is not reintroduced
   //
   /*snprintf(sendbuf, sizeof(sendbuf)-1, "MAIL FROM: <sender@aaa.fu>\r\nRCPT TO: <%s>\r\nDATA\r\n", recipient);
   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1, 3);
   if(!strstr(recvbuf, "354 ")) recvtimeoutssl(data->net, recvbuf, sizeof(recvbuf)-1);
   ASSERT(strstr(recvbuf, "354 "), recvbuf);*/

   send_smtp_command(data->net, "MAIL FROM: <sender@aaa.fu>\r\n", recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   snprintf(sendbuf, sizeof(sendbuf)-1, "RCPT TO: <%s>\r\n", recipient);
   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   send_smtp_command(data->net, "DATA\r\n", recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "354 ", 4) == 0, recvbuf);


   snprintf(sendbuf, sizeof(sendbuf)-1, "%s%s", testmessage, part1);
   write1(data->net, sendbuf, strlen(sendbuf));

   snprintf(sendbuf, sizeof(sendbuf), "%s", part2);
   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   snprintf(sendbuf, sizeof(sendbuf)-1, "QUIT\r\n");
   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1, 1);

   ASSERT(strncmp(recvbuf, "221 ", 4) == 0, recvbuf);

   close(data->net->socket);

   TEST_FOOTER();
}


static void test_smtp_commands_period_command_in_its_own_packet(char *server, int port, struct data *data){
   char recvbuf[MAXBUFSIZE], sendbuf[MAXBUFSIZE];

   TEST_HEADER();

   connect_to_smtp_server(server, port, data);

   send_helo_command(data->net);

   // As long as pipelining support is not reintroduced
   //
   /*snprintf(sendbuf, sizeof(sendbuf)-1, "MAIL FROM: <sender@aaa.fu>\r\nRCPT TO: <%s>\r\nDATA\r\n", recipient);
   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1, 3);
   if(!strstr(recvbuf, "354 ")) recvtimeoutssl(data->net, recvbuf, sizeof(recvbuf)-1);
   ASSERT(strstr(recvbuf, "354 "), recvbuf);*/

   send_smtp_command(data->net, "MAIL FROM: <sender@aaa.fu>\r\n", recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   snprintf(sendbuf, sizeof(sendbuf)-1, "RCPT TO: <%s>\r\n", recipient);
   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   send_smtp_command(data->net, "DATA\r\n", recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "354 ", 4) == 0, recvbuf);



   snprintf(sendbuf, sizeof(sendbuf)-1, "%s", testmessage);
   write1(data->net, sendbuf, strlen(sendbuf));

   snprintf(sendbuf, sizeof(sendbuf), "\r\n.\r\n");
   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   snprintf(sendbuf, sizeof(sendbuf)-1, "QUIT\r\n");
   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1, 1);

   ASSERT(strncmp(recvbuf, "221 ", 4) == 0, recvbuf);

   close(data->net->socket);

   TEST_FOOTER();
}


static void test_smtp_commands_with_partial_data_lines(char *server, int port, struct data *data){
   char recvbuf[MAXBUFSIZE], sendbuf[MAXBUFSIZE];
   int yes=1;

   TEST_HEADER();

   connect_to_smtp_server(server, port, data);

   setsockopt(data->net->socket, IPPROTO_TCP, TCP_NODELAY, &yes, (socklen_t)sizeof(int));

   send_helo_command(data->net);

   send_smtp_command(data->net, "MAIL FROM: <sender@aaa.fu>\r\n", recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   snprintf(sendbuf, sizeof(sendbuf)-1, "RCPT TO: <%s>\r\n", recipient);
   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   send_smtp_command(data->net, "DATA\r\n", recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "354 ", 4) == 0, recvbuf);


   snprintf(sendbuf, sizeof(sendbuf)-1, "From: aaa@aaa.fu\r\nTo: bela@aaa.fu\r\nMessage-Id: ajajajaja\r\nSubject: this is a test\r\n\r\nAaaaaa");
   write1(data->net, sendbuf, strlen(sendbuf)); sleep(2);

   snprintf(sendbuf, sizeof(sendbuf)-1, "jjdkdjkd dkd some garbage.");
   write1(data->net, sendbuf, strlen(sendbuf)); sleep(2);

   snprintf(sendbuf, sizeof(sendbuf)-1, "\r\nSome shit again  au aua ua au aua uuu");
   write1(data->net, sendbuf, strlen(sendbuf)); sleep(2);

   snprintf(sendbuf, sizeof(sendbuf)-1, ".\r\nAnd the last line.\r\n.\r\n");
   write1(data->net, sendbuf, strlen(sendbuf));

   snprintf(sendbuf, sizeof(sendbuf)-1, "%s\r\n.\r\nQUIT\r\n", testmessage);
   recvtimeoutssl(data->net, recvbuf, sizeof(recvbuf));

   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);


   close(data->net->socket);

   TEST_FOOTER();
}


static void test_smtp_bdat_last_one_at_a_time(char *server, int port, struct data *data){
   char recvbuf[MAXBUFSIZE], sendbuf[MAXBUFSIZE];

   TEST_HEADER();

   connect_to_smtp_server(server, port, data);

   send_helo_command(data->net);

   send_smtp_command(data->net, "MAIL FROM: <sender@aaa.fu>\r\n", recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   snprintf(sendbuf, sizeof(sendbuf)-1, "RCPT TO: <%s>\r\n", recipient);
   send_smtp_command(data->net, sendbuf, recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   snprintf(sendbuf, sizeof(sendbuf)-1, "BDAT %ld LAST\r\n", strlen(testmessage)+strlen(testmessage2));
   write1(data->net, sendbuf, strlen(sendbuf));

   write1(data->net, testmessage, strlen(testmessage));

   send_smtp_command(data->net, testmessage2, recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "250 ", 4) == 0, recvbuf);

   send_smtp_command(data->net, "QUIT\r\n", recvbuf, sizeof(recvbuf)-1, 1);
   ASSERT(strncmp(recvbuf, "221 ", 4) == 0, recvbuf);

   close(data->net->socket);

   TEST_FOOTER();
}


int main(int argc, char **argv){
   int port=25;
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
            {"rcpt",         required_argument,  0,  'r' },
            {"timeout",      required_argument,  0,  't' },
            {"lhlo",         no_argument,        0,  'l' },
            {"help",         no_argument,        0,  'h' },
            {0,0,0,0}
         };

      int option_index = 0;

      int c = getopt_long(argc, argv, "c:s:p:t:r:lh?", long_options, &option_index);
#else
      int c = getopt(argc, argv, "c:s:p:t:r:lh?");
#endif


      if(c == -1) break;

      switch(c){

         case 's' :
                    server = optarg;
                    break;

         case 'p' :
                    port = atoi(optarg);
                    break;

         case 'r' :
                    recipient = optarg;
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
   test_smtp_commands_one_at_a_time_data_in_2_parts(server, port, &data);
   ////test_smtp_commands_pipelining(server, port, &data);
   test_smtp_commands_with_reset_command(server, port, &data);
   test_smtp_commands_partial_command(server, port, &data);
   ////test_smtp_commands_partial_command_pipelining(server, port, &data);

   test_smtp_commands_period_command_in_2_parts(server, port, "\r", "\n.\r\n", &data);
   test_smtp_commands_period_command_in_2_parts(server, port, "\r\n", ".\r\n", &data);
   test_smtp_commands_period_command_in_2_parts(server, port, "\r\n.", "\r\n", &data);
   test_smtp_commands_period_command_in_2_parts(server, port, "\r\n.\r", "\n", &data);
   test_smtp_commands_period_command_in_its_own_packet(server, port, &data);

   test_smtp_commands_with_partial_data_lines(server, port, &data);

   helo = 1; // we must use EHLO to get the STARTTLS in the response

   test_smtp_commands_starttls(server, port, &data);
   data.net->use_ssl = 0;

   test_smtp_bdat_last_one_at_a_time(server, port, &data);

   return 0;
}
