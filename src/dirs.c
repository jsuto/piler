/*
 * dirs.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <unistd.h>
#include <piler.h>


void createdir(char *path, mode_t mode){
   struct stat st;

   if(strlen(path) > 2){
      if(path[strlen(path)-1] == '/') path[strlen(path)-1] = '\0';

      if(stat(path, &st)){
         if(mkdir(path, mode) == 0){
            syslog(LOG_PRIORITY, "created directory: *%s*", path);
         }
         else syslog(LOG_PRIORITY, "ERROR: could not create directory: *%s*", path);
      }
   }
}


void check_and_create_directories(struct config *cfg){
   char *p, s[SMALLBUFSIZE];
   int i;

   p = strrchr(cfg->workdir, '/');
   if(p){
      *p = '\0';
      createdir(cfg->workdir, 0755);
      *p = '/';
   }
   createdir(cfg->workdir, 0711);

   p = strrchr(cfg->queuedir, '/');
   if(p){
      *p = '\0';
      createdir(cfg->queuedir, 0755);
      *p = '/';
   }
   createdir(cfg->queuedir, 0700);

   snprintf(s, sizeof(s)-1, "%s/%02x", cfg->queuedir, cfg->server_id);
   createdir(s, 0700);

   p = strrchr(cfg->pidfile, '/');
   if(p){
      *p = '\0';
      createdir(cfg->pidfile, 0755);
      *p = '/';
   }

   for(i=0; i<cfg->number_of_worker_processes; i++){
      snprintf(s, sizeof(s)-1, "%s/%d", cfg->workdir, i);
      createdir(s, 0700);
   }

}
