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
int num_connections = 0;
int listenerfd = -1;
int loglevel = 1;

char *configfile = CONFIG_FILE;
struct config cfg;
struct passwd *pwd;
struct smtp_session *session, **sessions=NULL;
struct smtp_acl *smtp_acl[MAXHASH];

time_t prev_timeout_check = 0;

void usage(){
   printf("\nusage: piler\n\n");
   printf("    -c <config file>                  Config file to use if not the default\n");
   printf("    -d                                Fork to the background\n");
   printf("    -v                                Return the version and build number\n");
   printf("    -V                                Return the version and some build parameters\n");
   printf("    -L <log level>                    Set the log level: 1-5\n");

   exit(0);
}


void p_clean_exit(int sig){
   if(sig > 0) syslog(LOG_PRIORITY, "got signal: %d, %s", sig, strsignal(sig));

   if(listenerfd != -1) close(listenerfd);

   if(sessions){
      for(int i=0; i<cfg.max_connections; i++){
         if(sessions[i]) free_smtp_session(sessions[i]);
      }

      free(sessions);
   }

   if(events) free(events);

   clear_smtp_acl(smtp_acl);

   syslog(LOG_PRIORITY, "%s has been terminated", PROGNAME);

   ERR_free_strings();

   exit(1);
}


void fatal(char *s){
   syslog(LOG_PRIORITY, "%s", s);
   p_clean_exit(0);
}


void check_for_client_timeout(){
   time_t now;

   time(&now);

   if(cfg.verbosity >= LOG_DEBUG) syslog(LOG_PRIORITY, "%s @%ld", __func__, now);

   if(now - prev_timeout_check < cfg.smtp_timeout) return;

   if(num_connections > 0){
      for(int i=0; i<cfg.max_connections; i++){
         if(sessions[i] && now - sessions[i]->lasttime >= cfg.smtp_timeout){
            syslog(LOG_PRIORITY, "client %s timeout, lasttime: %ld", sessions[i]->remote_host, sessions[i]->lasttime);
            tear_down_session(sessions, sessions[i]->slot, &num_connections, "timeout");
         }
      }
   }

   time(&prev_timeout_check);
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

   load_smtp_acl(smtp_acl);

   syslog(LOG_PRIORITY, "reloaded config: %s", configfile);
}


int main(int argc, char **argv){
   int client_sockfd;
   int i, daemonise=0;
   int client_len = sizeof(struct sockaddr_storage);
   ssize_t readlen;
   struct sockaddr_storage client_address;
   char readbuf[REALLYBIGBUFSIZE];
   int efd;

   while((i = getopt(argc, argv, "c:L:dvVh")) > 0){
      switch(i){

        case 'c' :
                   configfile = optarg;
                   break;

        case 'L':
                   loglevel = atoi(optarg);
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
                   usage();
      }
   }

   (void) openlog(PROGNAME, LOG_PID, LOG_MAIL);

   initialise_configuration();

   listenerfd = create_and_bind(cfg.listen_addr, cfg.listen_port);
   if(listenerfd == -1){
      exit(1);
   }

   if(make_socket_non_blocking(listenerfd) == -1){
      fatal("make_socket_non_blocking()");
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
   set_signal_handler(SIGKILL, p_clean_exit);
   set_signal_handler(SIGSEGV, p_clean_exit);

   set_signal_handler(SIGPIPE, SIG_IGN);
   set_signal_handler(SIGALRM, SIG_IGN);

   set_signal_handler(SIGHUP, initialise_configuration);

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
      int n = epoll_wait(efd, events, cfg.max_connections, 1000);
      for(i=0; i<n; i++){

         // Office365 sometimes behaves oddly: when it receives the 250 OK
         // message after sending the email, it doesn't send the QUIT command
         // rather it aborts the connection

         if((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN))){
            if(cfg.verbosity >= _LOG_EXTREME) syslog(LOG_PRIORITY, "ERROR: the remote end hung up without sending QUIT");
            session = get_session_by_socket(sessions, cfg.max_connections, events[i].data.fd);
            if(session)
               tear_down_session(sessions, session->slot, &num_connections, "hungup");
            else
               close(events[i].data.fd);
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
                     syslog(LOG_PRIORITY, "ERROR: accept(): '%s'", strerror(errno));
                     break;
                  }
               }

               char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
               memset(hbuf, 0, sizeof(hbuf));
               memset(sbuf, 0, sizeof(sbuf));

               if(getnameinfo((struct sockaddr *)&client_address, client_len, hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV) == 0){
                  // Strictly speaking it's not correct to log num_connections+1 connections
                  // but it still gives a good clue how many connections we have at the moment
                  syslog(LOG_PRIORITY, "connected from %s:%s on fd=%d (active connections: %d)", hbuf, sbuf, client_sockfd, num_connections + 1);
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

               start_new_session(sessions, client_sockfd, &num_connections, smtp_acl, hbuf, &cfg);
            }

            continue;
         }


         // handle data from an existing connection

         else {
            int done = 0;

            session = get_session_by_socket(sessions, cfg.max_connections, events[i].data.fd);
            if(session == NULL){
               syslog(LOG_PRIORITY, "ERROR: cannot find session for this socket: %d", events[i].data.fd);
               close(events[i].data.fd);
               continue;
            }

            time(&(session->lasttime));

            while(1){
               if(session->net.use_ssl == 1)
                  readlen = SSL_read(session->net.ssl, (char*)&readbuf[0], sizeof(readbuf)-1);
               else
                  readlen = read(events[i].data.fd, (char*)&readbuf[0], sizeof(readbuf)-1);

               if(cfg.verbosity >= _LOG_EXTREME && readlen > 0) syslog(LOG_PRIORITY, "got %ld bytes to read", readlen);

               if(readlen == -1){
                  /* If errno == EAGAIN, that means we have read all data. So go back to the main loop. */
                  if(errno != EAGAIN){
                     done = 1;
                  }
                  break;
               }
               else if(readlen == 0){
                  /* End of file. The remote has closed the connection. */
                  done = 1;
                  break;
               }

               readbuf[readlen] = '\0';
               handle_data(session, &readbuf[0], readlen, &cfg);

               if(session->protocol_state == SMTP_STATE_BDAT && session->bad == 1){
                  done = 1;
                  break;
               }
            }

            /* Don't wait until the remote client closes the connection after he sent the QUIT command */

            if(done || session->protocol_state == SMTP_STATE_FINISHED){
               tear_down_session(sessions, session->slot, &num_connections, "done");
            }
         }


      }

      check_for_client_timeout();
   }

   return 0;
}
