/*
 * piler.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>
#include <locale.h>
#include <errno.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <piler.h>


extern char *optarg;
extern int optind;

int sd;
int quit = 0;
int received_sighup = 0;
char *configfile = CONFIG_FILE;
struct __config cfg;
struct __data data;
struct passwd *pwd;

struct child children[MAXCHILDREN];


static void takesig(int sig);
static void child_sighup_handler(int sig);
static void child_main(struct child *ptr);
static pid_t child_make(struct child *ptr);
int search_slot_by_pid(pid_t pid);
void kill_children(int sig);
void p_clean_exit();
void fatal(char *s);
void initialise_configuration();




static void takesig(int sig){
   int i, status;
   pid_t pid;

   switch(sig){
        case SIGHUP:
                initialise_configuration();
                if(read_key(&cfg)) fatal(ERR_READING_KEY);
                kill_children(SIGHUP);
                break;

        case SIGTERM:
        case SIGKILL:
                quit = 1;
                p_clean_exit();
                break;

        case SIGCHLD:
                while((pid = waitpid (-1, &status, WNOHANG)) > 0){

                   //syslog(LOG_PRIORITY, "child (pid: %d) has died", pid);

                   if(quit == 0){
                      i = search_slot_by_pid(pid);
                      if(i >= 0){
                         children[i].serial = i;
                         children[i].status = READY;
                         children[i].pid = child_make(&children[i]);
                      }
                      else syslog(LOG_PRIORITY, "error: couldn't find slot for pid %d", pid);

                   }
                }
                break;
   }

   return;
}


static void child_sighup_handler(int sig){
   if(sig == SIGHUP){
      received_sighup = 1;
   }
}


static void child_main(struct child *ptr){
   int new_sd;
   char s[INET6_ADDRSTRLEN];
   struct sockaddr_storage client_addr;
   socklen_t addr_size;

   ptr->messages = 0;

   if(cfg.verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "child (pid: %d, serial: %d) started main()", getpid(), ptr->serial);

   while(1){
      if(received_sighup == 1){
         if(cfg.verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "child (pid: %d) caught HUP signal", getpid());
         break;
      }

      ptr->status = READY;

      addr_size = sizeof(client_addr);
      new_sd = accept(sd, (struct sockaddr *)&client_addr, &addr_size);

      if(new_sd == -1) continue;

      ptr->status = BUSY;

      inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *)&client_addr), s, sizeof(s));

      syslog(LOG_PRIORITY, "connection from %s", s);

      data.child_serial = ptr->serial;

      sig_block(SIGHUP);
      ptr->messages += handle_smtp_session(new_sd, &data, &cfg);
      sig_unblock(SIGHUP);

      close(new_sd);

      if(cfg.max_requests_per_child > 0 && ptr->messages >= cfg.max_requests_per_child){
         if(cfg.verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "child (pid: %d, serial: %d) served enough: %d", getpid(), ptr->messages, ptr->serial);
         break;
      }

   }

   ptr->status = UNDEF;

#ifdef HAVE_MEMCACHED
   memcached_shutdown(&(data.memc));
#endif

   if(cfg.verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "child decides to exit (pid: %d)", getpid());

   exit(0);
}


static pid_t child_make(struct child *ptr){
   pid_t pid;

   if((pid = fork()) > 0) return pid;

   if(pid == -1) return -1;

   if(cfg.verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "forked a child (pid: %d)", getpid());

   /* reset signals */

   set_signal_handler(SIGCHLD, SIG_DFL);
   set_signal_handler(SIGTERM, SIG_DFL);
   set_signal_handler(SIGHUP, child_sighup_handler);

   child_main(ptr);

   return -1;
}



int child_pool_create(){
   int i;

   for(i=0; i<MAXCHILDREN; i++){
      children[i].pid = 0;
      children[i].messages = 0;
      children[i].status = UNDEF;
      children[i].serial = -1;
   }

   for(i=0; i<cfg.number_of_worker_processes; i++){
      children[i].status = READY;
      children[i].serial = i;
      children[i].pid = child_make(&children[i]);

      if(children[i].pid == -1){
         syslog(LOG_PRIORITY, "error: failed to fork a child");
         p_clean_exit();
      }
   }

   return 0;
}


int search_slot_by_pid(pid_t pid){
   int i;

   for(i=0; i<MAXCHILDREN; i++){
      if(children[i].pid == pid) return i;
   }

   return -1;
}


void kill_children(int sig){
   int i;

   for(i=0; i<MAXCHILDREN; i++){
      if(children[i].status != UNDEF && children[i].pid > 1){
         if(cfg.verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "sending signal to child (pid: %d)", children[i].pid);
         kill(children[i].pid, sig);
      }
   }
}


void p_clean_exit(){
   if(sd != -1) close(sd);

   kill_children(SIGTERM);

   clearrules(data.archiving_rules);
   clearrules(data.retention_rules);

   clearhash(data.mydomains);

   syslog(LOG_PRIORITY, "%s has been terminated", PROGNAME);

   unlink(cfg.pidfile);

   if(data.dedup != MAP_FAILED) munmap(data.dedup, MAXCHILDREN*DIGEST_LENGTH*2);

#ifdef HAVE_STARTTLS
   if(data.ctx){
      SSL_CTX_free(data.ctx);
      ERR_free_strings();
   }
#endif

   exit(1);
}


void fatal(char *s){
   syslog(LOG_PRIORITY, "%s\n", s);
   p_clean_exit();
}


#ifdef HAVE_STARTTLS
int init_ssl(){

   SSL_library_init();
   SSL_load_error_strings();

   data.ctx = SSL_CTX_new(TLSv1_server_method());

   if(data.ctx == NULL){ syslog(LOG_PRIORITY, "SSL_CTX_new() failed"); return ERR; }

   if(SSL_CTX_set_cipher_list(data.ctx, cfg.cipher_list) == 0){ syslog(LOG_PRIORITY, "failed to set cipher list: '%s'", cfg.cipher_list); return ERR; }

   if(SSL_CTX_use_PrivateKey_file(data.ctx, cfg.pemfile, SSL_FILETYPE_PEM) != 1){ syslog(LOG_PRIORITY, "cannot load private key from %s", cfg.pemfile); return ERR; }

   if(SSL_CTX_use_certificate_file(data.ctx, cfg.pemfile, SSL_FILETYPE_PEM) != 1){ syslog(LOG_PRIORITY, "cannot load certificate from %s", cfg.pemfile); return ERR; }

   return OK;
}
#endif


void initialise_configuration(){
   struct session_data sdata;

   cfg = read_config(configfile);

   if(cfg.number_of_worker_processes < 5) cfg.number_of_worker_processes = 5;
   if(cfg.number_of_worker_processes > MAXCHILDREN) cfg.number_of_worker_processes = MAXCHILDREN;

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


   clearrules(data.archiving_rules);
   clearrules(data.retention_rules);

   clearhash(data.mydomains);

   data.folder = 0;
   data.recursive_folder_names = 0;

   inithash(data.mydomains);
   initrules(data.archiving_rules);
   initrules(data.retention_rules);

#ifdef HAVE_STARTTLS
   if(cfg.tls_enable > 0 && data.ctx == NULL && init_ssl() == OK){
      snprintf(data.starttls, sizeof(data.starttls)-1, "250-STARTTLS\r\n");
   }
#endif

   if(open_database(&sdata, &cfg) == ERR){
      syslog(LOG_PRIORITY, "cannot connect to mysql server");
      return;
   }

   load_rules(&sdata, &data, data.archiving_rules, SQL_ARCHIVING_RULE_TABLE);
   load_rules(&sdata, &data, data.retention_rules, SQL_RETENTION_RULE_TABLE);

   load_mydomains(&sdata, &data, &cfg);

   if(cfg.server_id > 0) insert_offset(&sdata, cfg.server_id);

   close_database(&sdata);


   syslog(LOG_PRIORITY, "reloaded config: %s", configfile);

#ifdef HAVE_MEMCACHED
   memcached_init(&(data.memc), cfg.memcached_servers, 11211);
#endif
}


int main(int argc, char **argv){
   int i, rc, yes=1, daemonise=0, dedupfd;
   char port_string[8];
   struct addrinfo hints, *res;


   while((i = getopt(argc, argv, "c:dvVh")) > 0){
      switch(i){

        case 'c' :
                   configfile = optarg;
                   break;

        case 'd' :
                   daemonise = 1;
                   break;

        case 'v' :
                   printf("%s build %d\n", VERSION, get_build());
                   return 0;

        case 'V' :
                   printf("%s %s, build %d, Janos SUTO <sj@acts.hu>\n\n%s\n\n", PROGNAME, VERSION, get_build(), CONFIGURE_PARAMS);
                   return 0;

        case 'h' :
        default  : 
                   __fatal("usage: ...");
      }
   }

   (void) openlog(PROGNAME, LOG_PID, LOG_MAIL);

   data.folder = 0;
   data.recursive_folder_names = 0;
   inithash(data.mydomains);
   initrules(data.archiving_rules);
   initrules(data.retention_rules);
   data.ctx = NULL;
   data.ssl = NULL;
   data.dedup = MAP_FAILED;
   memset(data.starttls, 0, sizeof(data.starttls));


   initialise_configuration();

   set_signal_handler (SIGPIPE, SIG_IGN);


   if(read_key(&cfg)) fatal(ERR_READING_KEY);


   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;

   snprintf(port_string, sizeof(port_string)-1, "%d", cfg.listen_port);

   if((rc = getaddrinfo(cfg.listen_addr, port_string, &hints, &res)) != 0){
      fprintf(stderr, "getaddrinfo for '%s': %s\n", cfg.listen_addr, gai_strerror(rc));
      return 1;
   }


   if((sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
      fatal(ERR_OPEN_SOCKET);

   if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
      fatal(ERR_SET_SOCK_OPT);

   if(bind(sd, res->ai_addr, res->ai_addrlen) == -1)
      fatal(ERR_BIND_TO_PORT);

   if(listen(sd, cfg.backlog) == -1)
      fatal(ERR_LISTEN);


   freeaddrinfo(res);


   if(drop_privileges(pwd)) fatal(ERR_SETUID);

   dedupfd = open(MESSAGE_ID_DEDUP_FILE, O_RDWR);
   if(dedupfd == -1) fatal(ERR_OPEN_DEDUP_FILE);

   data.dedup = mmap(NULL, MAXCHILDREN*DIGEST_LENGTH*2, PROT_READ|PROT_WRITE, MAP_SHARED, dedupfd, 0);
   close(dedupfd);

   if(data.dedup == MAP_FAILED) syslog(LOG_INFO, "cannot mmap() %s, errno=%d", MESSAGE_ID_DEDUP_FILE, errno);

   syslog(LOG_PRIORITY, "%s %s, build %d starting", PROGNAME, VERSION, get_build());


#if HAVE_DAEMON == 1
   if(daemonise == 1) i = daemon(1, 0);
#endif

   write_pid_file(cfg.pidfile);


   child_pool_create();

   set_signal_handler(SIGCHLD, takesig);
   set_signal_handler(SIGTERM, takesig);
   set_signal_handler(SIGKILL, takesig);
   set_signal_handler(SIGHUP, takesig);


   for(;;){ sleep(1); }

   p_clean_exit();

   return 0;
}

