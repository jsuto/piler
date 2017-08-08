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


void createdir(char *path, uid_t uid, gid_t gid, mode_t mode){
   struct stat st;

   if(strlen(path) > 2){
      if(path[strlen(path)-1] == '/') path[strlen(path)-1] = '\0';

      if(stat(path, &st)){
         if(mkdir(path, mode) == 0){
            if(chown(path, uid, gid))
               syslog(LOG_PRIORITY, "ERROR: createdir(): chown() failed on %s", path);
            syslog(LOG_PRIORITY, "created directory: *%s*", path);
         }
         else syslog(LOG_PRIORITY, "ERROR: could not create directory: *%s*", path);
      }
   }
}


void check_and_create_directories(struct config *cfg, uid_t uid, gid_t gid){
   char *p, s[SMALLBUFSIZE];
   int i;

   p = strrchr(cfg->workdir, '/');
   if(p){
      *p = '\0';
      createdir(cfg->workdir, uid, gid, 0755);
      *p = '/';
   }
   createdir(cfg->workdir, uid, gid, 0711);

   p = strrchr(cfg->queuedir, '/');
   if(p){
      *p = '\0';
      createdir(cfg->queuedir, uid, gid, 0755);
      *p = '/';
   }
   createdir(cfg->queuedir, uid, gid, 0700);

   snprintf(s, sizeof(s)-1, "%s/%02x", cfg->queuedir, cfg->server_id);
   createdir(s, uid, gid, 0700);

   p = strrchr(cfg->pidfile, '/');
   if(p){
      *p = '\0';
      createdir(cfg->pidfile, uid, gid, 0755);
      *p = '/';
   }

   for(i=0; i<cfg->number_of_worker_processes; i++){
      snprintf(s, sizeof(s)-1, "%s/%d", cfg->workdir, i);
   #ifdef HAVE_ANTIVIRUS
      createdir(s, uid, gid, 0711);
   #else
      createdir(s, uid, gid, 0700);
   #endif
   }

}
