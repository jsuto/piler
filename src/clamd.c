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


int clamd_scan(char *tmpfile, char *engine, char *avinfo, struct __config *cfg){
   int s, n;
   char *p, *q, buf[MAXBUFSIZE], scan_cmd[SMALLBUFSIZE];
   struct sockaddr_un server;

   memset(avinfo, 0, SMALLBUFSIZE);

   chmod(tmpfile, 0644);

   strcpy(server.sun_path, cfg->clamd_socket);
   server.sun_family = AF_UNIX;

   if((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
      syslog(LOG_PRIORITY, "ERR: create socket");
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
      p = strchr(buf, ' ');
      if(p){
         q = strrchr(p, ' ');
         if(q){
            *q = '\0';
            p++;
            strncpy(avinfo, p, SMALLBUFSIZE-1);
         }
      }

      return AV_VIRUS;
   }

   return AV_OK;
}


int clamd_net_scan(char *tmpfile, char *engine, char *avinfo, struct __config *cfg){
   int n, psd, rc, ret=AV_OK;
   char *p, *q, buf[MAXBUFSIZE], scan_cmd[SMALLBUFSIZE];
   char port_string[6];
   struct addrinfo hints, *res;

   memset(avinfo, 0, SMALLBUFSIZE);

   chmod(tmpfile, 0644);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: trying to pass to clamd", tmpfile);

   snprintf(port_string, sizeof(port_string)-1, "%d", cfg->clamd_port);

   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;

   if((rc = getaddrinfo(cfg->clamd_addr, port_string, &hints, &res)) != 0){
      syslog(LOG_PRIORITY, "%s: getaddrinfo for '%s': %s\n", tmpfile, cfg->clamd_addr, gai_strerror(rc));
      return AV_ERROR;
   }

   if((psd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1){
      syslog(LOG_PRIORITY, "%s: ERR: create socket", tmpfile);
      ret = AV_ERROR;
      goto ENDE_CLAMD;
   }

   if(connect(psd, res->ai_addr, res->ai_addrlen) == -1){
      syslog(LOG_PRIORITY, "%s: CLAMD ERR: connect to %s %d", tmpfile, cfg->clamd_addr, cfg->clamd_port);
      ret = AV_ERROR;
      goto ENDE_CLAMD;
   }


   memset(scan_cmd, 0, SMALLBUFSIZE);
   snprintf(scan_cmd, SMALLBUFSIZE-1, "SCAN %s/%s\r\n", cfg->workdir, tmpfile);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: CLAMD CMD: %s", tmpfile, scan_cmd);

   send(psd, scan_cmd, strlen(scan_cmd), 0);

   n = recvtimeout(psd, buf, MAXBUFSIZE, TIMEOUT);
   close(psd);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: CLAMD DEBUG: %d %s", tmpfile, n, buf);

   if(strcasestr(buf, CLAMD_RESP_INFECTED)){
      p = strchr(buf, ' ');
      if(p){
         q = strrchr(p, ' ');
         if(q){
            *q = '\0';
            p++;
            strncpy(avinfo, p, SMALLBUFSIZE-1);
         }
      }

      ret = AV_VIRUS;
   }


ENDE_CLAMD:
   freeaddrinfo(res);

   return ret;
}

