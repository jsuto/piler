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
#include <sys/epoll.h>
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

struct epoll_event event, *events=NULL;
int timeout = 20; // checking for timeout this often [sec]
int num_connections = 0;
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


void init_smtp_session(struct smtp_session *session, int slot, int sd){
   session->slot = slot;

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

   for(i=0; i<cfg.max_connections; i++){
      if(sessions[i]) free_smtp_session(sessions[i]);
   }

   if(sessions) free(sessions);
   if(events) free(events);

   syslog(LOG_PRIORITY, "%s has been terminated", PROGNAME);

   //unlink(cfg.pidfile);

   ERR_free_strings();

   exit(1);
}


void fatal(char *s){
   syslog(LOG_PRIORITY, "%s", s);
   p_clean_exit();
}


int get_session_slot(){
   int i;

   for(i=0; i<cfg.max_connections; i++){
      if(sessions[i] == NULL) return i;
   }

   return -1;
}


struct smtp_session *get_session_by_socket(int socket){
   int i;

   for(i=0; i<cfg.max_connections; i++){
      if(sessions[i] && sessions[i]->socket == socket) return sessions[i];
   }

   return NULL;
}


void tear_down_client(int slot){
   syslog(LOG_PRIORITY, "disconnected from %s", sessions[slot]->remote_host);

   close(sessions[slot]->socket);

   free_smtp_session(sessions[slot]);
   sessions[slot] = NULL;

   num_connections--;
}


void check_for_client_timeout(){
   time_t now;
   int i;

   if(num_connections > 0){
      time(&now);

      for(i=0; i<cfg.max_connections; i++){
         if(sessions[i] && now - sessions[i]->lasttime >= cfg.smtp_timeout){
            syslog(LOG_PRIORITY, "client %s timeout", sessions[i]->remote_host);
            tear_down_client(sessions[i]->slot);
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


int start_new_session(int socket){
   char smtp_banner[SMALLBUFSIZE];
   int slot;

   // Uh-oh! We have enough connections to serve already
   if(num_connections >= cfg.max_connections){
      syslog(LOG_PRIORITY, "too many connections (%d), cannot accept socket %d", num_connections, socket);
      send(socket, SMTP_RESP_421_ERR_ALL_PORTS_ARE_BUSY, strlen(SMTP_RESP_421_ERR_ALL_PORTS_ARE_BUSY), 0);
      close(socket);
      return -1;
   }

#ifdef HAVE_LIBWRAP
   if(is_blocked_by_tcp_wrappers(socket) == 1){
      close(socket);
      return -1;
   }
#endif

   slot = get_session_slot();

   syslog(LOG_PRIORITY, "INFO: found slot: %d", slot);

   if(slot >= 0 && sessions[slot] == NULL){
      sessions[slot] = malloc(sizeof(struct smtp_session));
      if(sessions[slot]){
         init_smtp_session(sessions[slot], slot, socket);
         snprintf(smtp_banner, sizeof(smtp_banner)-1, SMTP_RESP_220_BANNER, cfg.hostid);
         send(socket, smtp_banner, strlen(smtp_banner), 0);

         num_connections++;

         return 0;
      }
      else {
         syslog(LOG_PRIORITY, "ERROR: malloc() in start_new_session()");
      }
   }
   else {
      syslog(LOG_PRIORITY, "ERROR: couldn't find a slot for the connection");
   }

   send(socket, SMTP_RESP_421_ERR_TMP, strlen(SMTP_RESP_421_ERR_TMP), 0);
   close(socket);

   return -1;
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
   int i, n, daemonise=0;
   int client_len = sizeof(struct sockaddr_storage);
   struct sockaddr_storage client_address;
   char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
   char readbuf[BIGBUFSIZE];
   int efd;

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

   listenerfd = create_and_bind(cfg.listen_addr, cfg.listen_port);
   if(listenerfd == -1){
      exit(1);
   }

   if(listen(listenerfd, cfg.backlog) == -1){
      fatal("ERROR: listen()");
   }

   if(drop_privileges(pwd)) fatal(ERR_SETUID);

   efd = epoll_create1(0);
   if(efd == -1){
      fatal("ERROR: epoll_create()");
   }

   event.data.fd = listenerfd;
   event.events = EPOLLIN | EPOLLET;
   if(epoll_ctl(efd, EPOLL_CTL_ADD, listenerfd, &event) == -1){
      fatal("ERROR: epoll_ctl() on efd");
   }

   set_signal_handler(SIGINT, p_clean_exit);
   set_signal_handler(SIGTERM, p_clean_exit);
   set_signal_handler(SIGALRM, check_for_client_timeout);
   set_signal_handler(SIGHUP, initialise_configuration);

   alarm(timeout);

   // calloc() initialitizes the allocated memory

   sessions = calloc(cfg.max_connections, sizeof(struct smtp_session));
   events = calloc(cfg.max_connections, sizeof(struct epoll_event));

   if(!sessions || !events) fatal("ERROR: calloc()");

   SSL_library_init();
   SSL_load_error_strings();

   srand(getpid());

   syslog(LOG_PRIORITY, "%s %s, build %d starting", PROGNAME, VERSION, get_build());

#if HAVE_DAEMON == 1
   if(daemonise == 1 && daemon(1, 0) == -1) fatal(ERR_DAEMON);
#endif

   for(;;){
      n = epoll_wait(efd, events, cfg.max_connections, -1);
      for(i=0; i<n; i++){

         if((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN))){
            syslog(LOG_PRIORITY, "ERROR: epoll error");
            close(events[i].data.fd);
            // we have to tear_down_client as well if not the listening socket?
            continue;
         }

         // We have 1 or more incoming connections to process

         else if(listenerfd == events[i].data.fd){

            while(1){

               client_sockfd = accept(listenerfd, (struct sockaddr *)&client_address, (socklen_t *)&client_len);
               if(client_sockfd == -1){
                  if((errno == EAGAIN) || (errno == EWOULDBLOCK)){
                     // We have processed all incoming connections
                     break;
                  }
                  else {
                     syslog(LOG_PRIORITY, "ERROR: accept()");
                     break;
                  }
               }

               if(getnameinfo((struct sockaddr *)&client_address, client_len, hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV) == 0){
                  syslog(LOG_PRIORITY, "connected from %s:%s on descriptor %d", hbuf, sbuf, client_sockfd);
               }

               if(make_socket_non_blocking(client_sockfd) == -1){
                  syslog(LOG_PRIORITY, "ERROR: cannot make the socket non blocking");
                  break;
               }

               event.data.fd = client_sockfd;
               event.events = EPOLLIN | EPOLLET;
               if(epoll_ctl(efd, EPOLL_CTL_ADD, client_sockfd, &event) == -1){
                  syslog(LOG_PRIORITY, "ERROR: epoll_ctl() on client_sockfd");
                  break;
               }

               start_new_session(client_sockfd);
            }

            continue;
         }


         // handle data from an existing connection

         else {
            int done = 0;
            ssize_t count;

            // should the following work here as well?
            // ioctl(events[i].data.fd, FIONREAD, &bytes_to_read);

            session = get_session_by_socket(events[i].data.fd);
            if(session == NULL){
               syslog(LOG_PRIORITY, "ERROR: cannot find session for this socket: %d", events[i].data.fd);
               close(events[i].data.fd);
               continue;
            }

            time(&(session->lasttime));

            while(1){
               memset(readbuf, 0, sizeof(readbuf));

               if(session->use_ssl == 1)
                  count = SSL_read(session->ssl, (char*)&readbuf[0], sizeof(readbuf)-1);
               else
                  count = read(events[i].data.fd, (char*)&readbuf[0], sizeof(readbuf)-1);

               if(cfg.verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "got %ld bytes to read", count);

               if(count == -1){
                  /* If errno == EAGAIN, that means we have read all data. So go back to the main loop. */
                  if(errno != EAGAIN){
                     syslog(LOG_PRIORITY, "read");
                     done = 1;
                  }
                  break;
               }
               else if(count == 0){
                  /* End of file. The remote has closed the connection. */
                  done = 1;
                  break;
               }

               handle_data(session, &readbuf[0], count);

               if(session->protocol_state == SMTP_STATE_BDAT && session->bad == 1){
                  tear_down_client(session->slot);
                  done = 0; // to prevent the repeated tear down of connection
                  break;
               }
            }

            if(done){
               printf("Closed connection on descriptor %d\n", events[i].data.fd);

               /* Closing the descriptor will make epoll remove it from the set of descriptors which are monitored. */
               //close(events[i].data.fd);
               tear_down_client(session->slot);
            }
         }


      }
   }

   return 0;
}
