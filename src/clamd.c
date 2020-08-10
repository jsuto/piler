/*
 * clamd.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <piler.h>


int clamd_scan(char *tmpfile, struct config *cfg){
   int s, n;
   char buf[MAXBUFSIZE], scan_cmd[SMALLBUFSIZE];
   struct sockaddr_un server;

   chmod(tmpfile, 0644);

   strcpy(server.sun_path, cfg->clamd_socket);
   server.sun_family = AF_UNIX;

   if((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
      syslog(LOG_PRIORITY, "ERR: create socket to %s", cfg->clamd_socket);
      return AV_ERROR;
   }

   if(connect(s, (struct sockaddr *)&server, strlen(server.sun_path) + sizeof (server.sun_family)) == -1){
      syslog(LOG_PRIORITY, "CLAMD ERR: connect to %s", cfg->clamd_socket);
      return AV_ERROR;
   }


   /* issue the SCAN command with full path to the temporary directory */


   memset(scan_cmd, 0, SMALLBUFSIZE);
   snprintf(scan_cmd, SMALLBUFSIZE-1, "SCAN %s/%s\r\n", cfg->workdir, tmpfile);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: CLAMD CMD: %s", tmpfile, scan_cmd);

   send(s, scan_cmd, strlen(scan_cmd), 0);

   /* read CLAMD's answers */

   n = recvtimeout(s, buf, MAXBUFSIZE, TIMEOUT);

   close(s);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: CLAMD DEBUG: %d %s", tmpfile, n, buf);

   if(strcasestr(buf, CLAMD_RESP_INFECTED)){
      char *p = strchr(buf, ' ');
      if(p){
         char *q = strrchr(p, ' ');
         if(q){
            *q = '\0';
            p++;
            syslog(LOG_PRIORITY, "%s: VIRUS <%s> found, status=%s", tmpfile, p, S_STATUS_DISCARDED);
         }
      }

      return AV_VIRUS;
   }

   return AV_OK;
}
