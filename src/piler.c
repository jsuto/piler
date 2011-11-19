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
int nconn = 0;
char *configfile = CONFIG_FILE;
struct __config cfg;
struct __data data;
struct passwd *pwd;


void clean_exit(){
   if(sd != -1) close(sd);

   free_rule(data.rules);

   syslog(LOG_PRIORITY, "%s has been terminated", PROGNAME);

   unlink(cfg.pidfile);

   exit(1);
}


void fatal(char *s){
   syslog(LOG_PRIORITY, "%s\n", s);
   clean_exit();
}


void sigchld(){
   int pid, wstat;

   while((pid = wait_nohang(&wstat)) > 0){
      if(nconn > 0) nconn--;
   }
}


void initialise_configuration(){
   struct session_data sdata;

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


   free_rule(data.rules);
   data.rules = NULL;

   mysql_init(&(sdata.mysql));
   mysql_options(&(sdata.mysql), MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&cfg.mysql_connect_timeout);
   if(mysql_real_connect(&(sdata.mysql), cfg.mysqlhost, cfg.mysqluser, cfg.mysqlpwd, cfg.mysqldb, cfg.mysqlport, cfg.mysqlsocket, 0) == 0){
      syslog(LOG_PRIORITY, "cannot connect to mysql server");
      return;
   }

   load_archiving_rules(&sdata, &(data.rules));

   mysql_close(&(sdata.mysql));


   syslog(LOG_PRIORITY, "reloaded config: %s", configfile);

#ifdef HAVE_MEMCACHED
   memcached_init(&(data.memc), cfg.memcached_servers, 11211);
#endif
}


int read_key(struct __config *cfg){
   int fd, n;

   fd = open(KEYFILE, O_RDONLY);
   if(fd == -1){
      syslog(LOG_PRIORITY, "cannot read keyfile: %s", KEYFILE);
      return -1;
   }

   n = read(fd, cfg->key, KEYLEN);

   close(fd);

   if(n > 5) return 0;

   return 1;
}


int main(int argc, char **argv){
   int i, new_sd, yes=1, pid, daemonise=0;
   unsigned int clen;
   struct sockaddr_in client_addr, serv_addr;
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

   sig_catch(SIGINT, clean_exit);
   sig_catch(SIGQUIT, clean_exit);
   sig_catch(SIGKILL, clean_exit);
   sig_catch(SIGTERM, clean_exit);
   sig_catch(SIGHUP, initialise_configuration);


   data.rules = NULL;


   sig_block(SIGCHLD);
   sig_catch(SIGCHLD, sigchld);


   initialise_configuration();


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


   /* main accept loop */

   for(;;){

      /* let new connections wait if we are too busy now */

      if(nconn >= cfg.max_connections) sig_pause();

      clen = sizeof(client_addr);

      sig_unblock(SIGCHLD);
      new_sd = accept(sd, (struct sockaddr *)&client_addr, &clen);
      sig_block(SIGCHLD);

      if(new_sd == -1) continue;

      pid = fork();

      if(pid == 0){
          sig_uncatch(SIGCHLD);
          sig_unblock(SIGCHLD);

          sig_uncatch(SIGINT);
          sig_uncatch(SIGQUIT);
          sig_uncatch(SIGKILL);
          sig_uncatch(SIGTERM);
          sig_block(SIGHUP);

          syslog(LOG_PRIORITY, "connection from client: %s", inet_ntoa(client_addr.sin_addr));

          handle_smtp_session(new_sd, &data, &cfg);

          _exit(0);
      }

      else if(pid > 0){
         nconn++;
      }

      else {
         syslog(LOG_PRIORITY, "%s", ERR_FORK_FAILED);
      }

      close(new_sd);
   }

   return 0;
}

