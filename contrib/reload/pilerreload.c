/*
 * pilerreload.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>
#include <unistd.h>
#include <errno.h>
#include <piler.h>

extern char *optarg;
extern int optind;

char *configfile = CONFIG_FILE;
struct __config cfg;


void fatal(char *s){
   syslog(LOG_PRIORITY, "fatal: %s", s);
   exit(1);
}


int main(int argc, char **argv){
   char buf[SMALLBUFSIZE];
   int pid;
   FILE *f;

   (void) openlog("pilerreload", LOG_PID, LOG_MAIL);

   cfg = read_config(CONFIG_FILE);

   f = fopen(cfg.pidfile, "r");
   if(!f) fatal("cannot open pidfile");

   if(fgets(buf, sizeof(buf)-2, f) == NULL) fatal("cannot read pidfile");

   fclose(f);

   pid = atoi(buf);

   if(pid > 1){
      if(kill(pid, SIGHUP) == 0) syslog(LOG_PRIORITY, "reloaded");
      else syslog(LOG_PRIORITY, "failed to reload");
   }
   else fatal("invalid pid");

   return 0;
}

