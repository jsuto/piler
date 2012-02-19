/*
 * piler.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
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


signal_func *set_signal_handler(int signo, signal_func * func);
static void takesig(int sig);
static void child_sighup_handler(int sig);
static void child_main(struct child *ptr);
static pid_t child_make(struct child *ptr);
int search_slot_by_pid(pid_t pid);
void kill_children(int sig);
void clean_exit();
void initialise_configuration();




/*
 * most of the preforking code was taken from the tinyproxy project
 */

signal_func *set_signal_handler(int signo, signal_func * func){
   struct sigaction act, oact;

   act.sa_handler = func;
   sigemptyset (&act.sa_mask);
   act.sa_flags = 0;

   if(sigaction(signo, &act, &oact) < 0) return SIG_ERR;

   return oact.sa_handler;
}


static void takesig(int sig){
   int i, status;
   pid_t pid;

   switch(sig){
        case SIGHUP:
                initialise_configuration();
                kill_children(SIGHUP);
                break;

        case SIGTERM:
        case SIGKILL:
                quit = 1;
                clean_exit();
                break;

        case SIGCHLD:
                while((pid = waitpid (-1, &status, WNOHANG)) > 0){

                   //syslog(LOG_PRIORITY, "child (pid: %d) has died", pid);

                   if(quit == 0){
                      i = search_slot_by_pid(pid);
                      if(i >= 0){
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
   unsigned int clen;
   struct sockaddr_in client_addr;

   ptr->messages = 0;

   if(cfg.verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "child (pid: %d) started main()", getpid());

   while(1){
      if(received_sighup == 1){
         if(cfg.verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "child (pid: %d) caught HUP signal", getpid());
         break;
      }

      ptr->status = READY;

      clen = sizeof(client_addr);

      new_sd = accept(sd, (struct sockaddr *)&client_addr, &clen);

      if(new_sd == -1) continue;

      ptr->status = BUSY;

      syslog(LOG_PRIORITY, "child (pid: %d) connection from %s", getpid(), inet_ntoa(client_addr.sin_addr));

      sig_block(SIGHUP);
      ptr->messages += handle_smtp_session(new_sd, &data, &cfg);
      sig_unblock(SIGHUP);

      close(new_sd);

      if(cfg.max_requests_per_child > 0 && ptr->messages >= cfg.max_requests_per_child){
         if(cfg.verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "child (pid: %d) served enough: %d", getpid(), ptr->messages);
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
   }

   for(i=0; i<cfg.number_of_worker_processes; i++){
      children[i].status = READY;
      children[i].pid = child_make(&children[i]);

      if(children[i].pid == -1){
         syslog(LOG_PRIORITY, "error: failed to fork a child");
         clean_exit();
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


void clean_exit(){
   if(sd != -1) close(sd);

   kill_children(SIGTERM);

   free_rule(data.archiving_rules);
   free_rule(data.retention_rules);

   syslog(LOG_PRIORITY, "%s has been terminated", PROGNAME);

   unlink(cfg.pidfile);

   exit(1);
}


void fatal(char *s){
   syslog(LOG_PRIORITY, "%s\n", s);
   clean_exit();
}


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


   free_rule(data.archiving_rules);
   free_rule(data.retention_rules);

   data.archiving_rules = NULL;
   data.retention_rules = NULL;

   mysql_init(&(sdata.mysql));
   mysql_options(&(sdata.mysql), MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&cfg.mysql_connect_timeout);
   if(mysql_real_connect(&(sdata.mysql), cfg.mysqlhost, cfg.mysqluser, cfg.mysqlpwd, cfg.mysqldb, cfg.mysqlport, cfg.mysqlsocket, 0) == 0){
      syslog(LOG_PRIORITY, "cannot connect to mysql server");
      return;
   }

   load_rules(&sdata, &(data.archiving_rules), SQL_ARCHIVING_RULE_TABLE);
   load_rules(&sdata, &(data.retention_rules), SQL_RETENTION_RULE_TABLE);

   mysql_close(&(sdata.mysql));


   syslog(LOG_PRIORITY, "reloaded config: %s", configfile);

#ifdef HAVE_MEMCACHED
   memcached_init(&(data.memc), cfg.memcached_servers, 11211);
#endif
}


int main(int argc, char **argv){
   int i, yes=1, daemonise=0;
   struct sockaddr_in serv_addr;
   struct in_addr addr;

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
                   __fatal(PROGNAME " " PROGINFO);
                   break;

        case 'h' :
        default  : 
                   __fatal("usage: ...");
      }
   }

   (void) openlog(PROGNAME, LOG_PID, LOG_MAIL);

   data.archiving_rules = NULL;
   data.retention_rules = NULL;


   initialise_configuration();

   set_signal_handler (SIGPIPE, SIG_IGN);


   if(read_key(&cfg)) fatal(ERR_READING_KEY);


   if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
      fatal(ERR_OPEN_SOCKET);

   serv_addr.sin_family = AF_INET;
   serv_addr.sin_port = htons(cfg.listen_port);
   inet_aton(cfg.listen_addr, &addr);
   serv_addr.sin_addr.s_addr = addr.s_addr;
   bzero(&(serv_addr.sin_zero), 8);

   if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
      fatal(ERR_SET_SOCK_OPT);

   if(bind(sd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1)
      fatal(ERR_BIND_TO_PORT);

   if(listen(sd, cfg.backlog) == -1)
      fatal(ERR_LISTEN);


   if(drop_privileges(pwd)) fatal(ERR_SETUID);


   syslog(LOG_PRIORITY, "%s %s starting", PROGNAME, VERSION);


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

   clean_exit();

   return 0;
}

