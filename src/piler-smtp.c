/*
 * piler-smtp.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pwd.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <poll.h>
#include <netinet/in.h>
#include <locale.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <syslog.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <piler.h>

#define PROGNAME "piler-smtp"

extern char *optarg;
extern int optind;
 
struct pollfd *poll_set=NULL;
int timeout = 20; // checking for timeout this often [sec]
int numfds = 0;
int listenerfd = -1;

char *configfile = CONFIG_FILE;
struct __config cfg;
struct passwd *pwd;
struct smtp_session *session, **sessions=NULL;


void handle_data(struct smtp_session *session, char *readbuf, int readlen){
   char *p, puf[MAXBUFSIZE];
   int result;

   // process BDAT stuff

   if(session->protocol_state == SMTP_STATE_BDAT){
      if(session->bad == 1){
         // something bad happened in the BDAT processing
         return;
      }

      process_bdat(session, readbuf, readlen);
   }

   // process DATA

   else if(session->protocol_state == SMTP_STATE_DATA){
      process_data(session, readbuf, readlen);
   }

   // process other SMTP commands

   else {
      //printf("len=%d, buf=*%s*\n\n\n", readlen, readbuf);

      if(session->buflen > 0){
         snprintf(puf, sizeof(puf)-1, "%s%s", session->buf, readbuf);
         snprintf(readbuf, BIGBUFSIZE-1, "%s", puf);

         session->buflen = 0;
         memset(session->buf, 0, SMALLBUFSIZE);
      }

      p = readbuf;

      do {
         memset(puf, 0, sizeof(puf));
         p = split(p, '\n', puf, sizeof(puf)-1, &result);

         if(puf[0] == '\0') continue;

         if(result == 1){
            process_smtp_command(session, puf);

            // if chunking is enabled and we have data after BDAT <len>
            // then process the rest

            if(session->cfg->enable_chunking == 1 && p && session->protocol_state == SMTP_STATE_BDAT){
               process_bdat(session, p, strlen(p));
               break;
            }
         }
         else {
            snprintf(session->buf, SMALLBUFSIZE-1, "%s", puf);
            session->buflen = strlen(puf);
         }
      } while(p);

   }

}


void init_smtp_session(struct smtp_session *session, int fd_index, int sd){
   session->socket = sd;
   session->buflen = 0;
   session->protocol_state = SMTP_STATE_INIT;

   session->cfg = &cfg;

   session->use_ssl = 0;  // use SSL/TLS
   session->starttls = 0; // SSL/TLS communication is active (1) or not (0)
   session->ctx = NULL;
   session->ssl = NULL;

   memset(session->buf, 0, SMALLBUFSIZE);
   memset(session->remote_host, 0, INET6_ADDRSTRLEN);

   reset_bdat_counters(session);

   time(&(session->lasttime));
}


void free_smtp_session(struct smtp_session *session){

   if(session){

      if(session->use_ssl == 1){
         SSL_shutdown(session->ssl);
         SSL_free(session->ssl);
      }

      if(session->ctx) SSL_CTX_free(session->ctx);

      free(session);
   }
}



void p_clean_exit(){
   int i;

   if(listenerfd != -1) close(listenerfd);

   for(i=1; i<numfds; i++){
      close(poll_set[i].fd);
      free_smtp_session(sessions[i]);
   }

   if(sessions) free(sessions);
   if(poll_set) free(poll_set);

   syslog(LOG_PRIORITY, "%s has been terminated", PROGNAME);

   //unlink(cfg.pidfile);

   ERR_free_strings();

   exit(1);
}


void fatal(char *s){
   syslog(LOG_PRIORITY, "%s", s);
   p_clean_exit();
}


void tear_down_client(int n){
   int i;

   close(poll_set[n].fd);
   poll_set[n].events = 0;

   syslog(LOG_PRIORITY, "disconnected from %s", sessions[n]->remote_host);

   free_smtp_session(sessions[n]);
   sessions[n] = NULL;

   for(i=n; i<numfds; i++){
      poll_set[i] = poll_set[i+1];
      sessions[i] = sessions[i+1];
   }

   numfds--;
}


void check_for_client_timeout(){
   time_t now;
   int i;

   if(numfds > 1){
      time(&now);

      for(i=1; i<numfds; i++){
         if(now - sessions[i]->lasttime >= cfg.smtp_timeout){
            syslog(LOG_PRIORITY, "client %s timeout", sessions[i]->remote_host);
            tear_down_client(i);
         }
      }
   }

   alarm(timeout);
}


#ifdef HAVE_LIBWRAP
int is_blocked_by_tcp_wrappers(int sd){
   struct request_info req;

   request_init(&req, RQ_DAEMON, PROGNAME, RQ_FILE, sd, 0);

   fromhost(&req);

   if(!hosts_access(&req)){
      send(sd, SMTP_RESP_550_ERR_YOU_ARE_BANNED_BY_LOCAL_POLICY, strlen(SMTP_RESP_550_ERR_YOU_ARE_BANNED_BY_LOCAL_POLICY), 0);
      syslog(LOG_PRIORITY, "denied connection from %s by tcp_wrappers", eval_client(&req));
      return 1;
   }

   return 0;
}
#endif


int create_listener_socket(char *listen_addr, int listen_port){
   int rc, sd, yes=1;
   char port_string[8];
   struct addrinfo hints, *res;

   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;

   snprintf(port_string, sizeof(port_string)-1, "%d", listen_port);

   if((rc = getaddrinfo(listen_addr, port_string, &hints, &res)) != 0){
      syslog(LOG_PRIORITY, "getaddrinfo for '%s': %s", listen_addr, gai_strerror(rc));
      return -1;
   }

   if((sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1){
      syslog(LOG_PRIORITY, "socket() error");
      return -1;
   }

   if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
      syslog(LOG_PRIORITY, "setsockopt() error");
      close(sd);
      return -1;
   }

   if(ioctl(sd, FIONBIO, (char *)&yes) == -1){
      syslog(LOG_PRIORITY, "ioctl() failed");
      close(sd);
      return -1;
   }

   if(bind(sd, res->ai_addr, res->ai_addrlen) == -1){
      syslog(LOG_PRIORITY, "cannot bind to port: %s:%d", listen_addr, listen_port);
      close(sd);
      return -1;
   }

   freeaddrinfo(res);

   if(listen(sd, cfg.backlog) == -1){
      syslog(LOG_PRIORITY, "listen() error");
      close(sd);
      return -1;
   }

   return sd;
}


void start_new_session(int socket, struct sockaddr_storage client_address, int fd_index){
   char smtp_banner[SMALLBUFSIZE], remote_host[INET6_ADDRSTRLEN];

   // Uh-oh! We have enough connections to serve already
   if(numfds >= cfg.max_connections){
      inet_ntop(client_address.ss_family, get_in_addr((struct sockaddr*)&client_address), remote_host, sizeof(remote_host));
      syslog(LOG_PRIORITY, "too many connections (%d), cannot accept %s", numfds, remote_host);
      send(socket, SMTP_RESP_421_ERR_ALL_PORTS_ARE_BUSY, strlen(SMTP_RESP_421_ERR_ALL_PORTS_ARE_BUSY), 0);
      close(socket);
      return;
   }

#ifdef HAVE_LIBWRAP
   if(is_blocked_by_tcp_wrappers(socket) == 1){
      close(socket);
      return;
   }
#endif

   sessions[numfds] = malloc(sizeof(struct smtp_session));

   if(sessions[numfds] == NULL){
      syslog(LOG_PRIORITY, "malloc error()");
      send(socket, SMTP_RESP_421_ERR_TMP, strlen(SMTP_RESP_421_ERR_TMP), 0);
      close(socket);
      return;
   }


   init_smtp_session(sessions[numfds], fd_index, socket);

   snprintf(smtp_banner, sizeof(smtp_banner)-1, SMTP_RESP_220_BANNER, cfg.hostid);
   send(socket, smtp_banner, strlen(smtp_banner), 0);

   inet_ntop(client_address.ss_family, get_in_addr((struct sockaddr*)&client_address), sessions[numfds]->remote_host, INET6_ADDRSTRLEN);

   syslog(LOG_PRIORITY, "connected from %s", sessions[numfds]->remote_host);

   poll_set[numfds].fd = socket;
   poll_set[numfds].events = POLLIN|POLLHUP;

   numfds++;
}


void initialise_configuration(){
   cfg = read_config(configfile);

   if(strlen(cfg.username) > 1){
      pwd = getpwnam(cfg.username);
      if(!pwd) fatal(ERR_NON_EXISTENT_USER);
   }

   if(getuid() == 0 && pwd){
      check_and_create_directories(&cfg, pwd->pw_uid, pwd->pw_gid);
   }

   if(chdir(cfg.workdir)){
      syslog(LOG_PRIORITY, "workdir: *%s*", cfg.workdir);
      fatal(ERR_CHDIR);
   }

   setlocale(LC_MESSAGES, cfg.locale);
   setlocale(LC_CTYPE, cfg.locale);

   syslog(LOG_PRIORITY, "reloaded config: %s", configfile);
}


int main(int argc, char **argv){
   int listenerfd, client_sockfd;
   int i, daemonise=0;
   int client_len = sizeof(struct sockaddr_storage);
   int readlen;
   int bytes_to_read;
   struct sockaddr_storage client_address;
   char readbuf[BIGBUFSIZE];

   while((i = getopt(argc, argv, "c:dvVh")) > 0){
      switch(i){

        case 'c' :
                   configfile = optarg;
                   break;

        case 'd' :
                   daemonise = 1;
                   break;

        case 'v' :
        case 'V' :
                   printf("%s build %d\n", VERSION, get_build());
                   return 0;

        case 'h' :
        default  : 
                   __fatal("usage: ...");
      }
   }

   (void) openlog(PROGNAME, LOG_PID, LOG_MAIL);

   initialise_configuration();

   listenerfd = create_listener_socket(cfg.listen_addr, cfg.listen_port);
   if(listenerfd == -1){
      syslog(LOG_PRIORITY, "create_listener_socket() error");
      exit(1);
   }

   if(drop_privileges(pwd)) fatal(ERR_SETUID);

   set_signal_handler(SIGINT, p_clean_exit);
   set_signal_handler(SIGTERM, p_clean_exit);
   set_signal_handler(SIGALRM, check_for_client_timeout);
   set_signal_handler(SIGHUP, initialise_configuration);

   alarm(timeout);

   // calloc() initialitizes the allocated memory

   sessions = calloc(cfg.max_connections, sizeof(struct smtp_session));
   poll_set = calloc(cfg.max_connections, sizeof(struct pollfd));

   if(!sessions || !poll_set) fatal("calloc() error");

   poll_set[0].fd = listenerfd;
   poll_set[0].events = POLLIN;
   numfds = 1;

   SSL_library_init();
   SSL_load_error_strings();

   srand(getpid());

   syslog(LOG_PRIORITY, "%s %s, build %d starting", PROGNAME, VERSION, get_build());

#if HAVE_DAEMON == 1
   if(daemonise == 1 && daemon(1, 0) == -1) fatal(ERR_DAEMON);
#endif

   for(;;){
      int fd_index;
 
      poll(poll_set, numfds, -1);

      for(fd_index = 0; fd_index < numfds; fd_index++){
         if(poll_set[fd_index].revents & POLLIN){

            // process new connection

            if(poll_set[fd_index].fd == listenerfd){
               client_sockfd = accept(listenerfd, (struct sockaddr *)&client_address, (socklen_t *)&client_len);
               start_new_session(client_sockfd, client_address, fd_index);
            }

            // handle data from an existing connection

            else {
               ioctl(poll_set[fd_index].fd, FIONREAD, &bytes_to_read);

               if(cfg.verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "got %d bytes to read", bytes_to_read);

               if(bytes_to_read == 0){
                  tear_down_client(fd_index);
               }
               else {
                  session = sessions[fd_index];

                  time(&(session->lasttime));

                  // readbuf must be large enough to hold 'bytes_to_read' data
                  // I think there shouldn't be more than MTU size data to be
                  // read from the socket at a time
                  memset(readbuf, 0, sizeof(readbuf));

                  if(session->use_ssl == 1)
                     readlen = SSL_read(session->ssl, (char*)&readbuf[0], sizeof(readbuf)-1);
                  else
                     readlen = recv(poll_set[fd_index].fd, &readbuf[0], sizeof(readbuf)-1, 0);

                  if(readlen < 1) break;

                  readbuf[readlen] = '\0'; // we need either this or memset(readbuf, ...) above

                  handle_data(session, &readbuf[0], readlen);

                  if(session->protocol_state == SMTP_STATE_BDAT && session->bad == 1) tear_down_client(fd_index);
               }
            }
         }
      }
   }

   return 0;
}
