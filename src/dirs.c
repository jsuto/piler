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


void createdir(char *path, uid_t uid, gid_t gid, mode_t mode);


void check_and_create_directories(struct __config *cfg, uid_t uid, gid_t gid){
   char *p;

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

   p = strrchr(cfg->pidfile, '/');
   if(p){
      *p = '\0';
      createdir(cfg->pidfile, uid, gid, 0755);
      *p = '/';
   }

}


void createdir(char *path, uid_t uid, gid_t gid, mode_t mode){
   struct stat st;

   if(strlen(path) > 2){
      if(path[strlen(path)-1] == '/') path[strlen(path)-1] = '\0';

      if(stat(path, &st)){
         if(mkdir(path, mode) == 0){
            chown(path, uid, gid);
            syslog(LOG_PRIORITY, "created directory: *%s*", path);
         }
         else syslog(LOG_PRIORITY, "could not create directory: *%s*", path);
      }

   }
}


