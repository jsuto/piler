/*
 * import_pop3.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>
#include <getopt.h>
#include <syslog.h>
#include <piler.h>


void import_the_file(struct session_data *sdata, struct data *data, struct config *cfg){
   close(data->import->fd);
   data->import->fd = -1;

   if(import_message(sdata, data, cfg) != ERR){
      unlink(data->import->filename);
   }
}


void process_buffer(char *buf, int buflen, uint64 *count, struct session_data *sdata, struct data *data, struct config *cfg){

   if(!strcmp(buf, PILEREXPORT_BEGIN_MARK)){
      if((*count) > 0){
         import_the_file(sdata, data, cfg);
      }

      (*count)++;

      snprintf(data->import->filename, SMALLBUFSIZE-1, "import-%llu", *count);

      data->import->fd = open(data->import->filename, O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR);
      if(data->import->fd == -1){
         // Do some error handling
         printf("error: cannot open %s\n", data->import->filename);
      }

   } else {
      if(write(data->import->fd, buf, buflen) != buflen){
         printf("error: didnt write %d bytes\n", buflen);
      }
   }
}


void import_from_pilerexport(struct session_data *sdata, struct data *data, struct config *cfg){
   int n, rc, savedlen=0, puflen;
   uint64 count=0;
   char *p, copybuf[2*BIGBUFSIZE+1], buf[BIGBUFSIZE], savedbuf[BIGBUFSIZE], puf[BIGBUFSIZE];

   memset(savedbuf, 0, sizeof(savedbuf));

   data->import->fd = -1;

   do {
      memset(buf, 0, sizeof(buf));
      n = fread(buf, 1, sizeof(buf)-1, stdin);

      if(savedlen > 0){
         memset(copybuf, 0, sizeof(copybuf));

         memcpy(copybuf, savedbuf, savedlen);
         memcpy(&copybuf[savedlen], buf, n);

         savedlen = 0;
         memset(savedbuf, 0, sizeof(savedbuf));

         p = &copybuf[0];
      }
      else {
         p = &buf[0];
      }

      do {
         puflen = read_one_line(p, '\n', puf, sizeof(puf), &rc);
         p += puflen;

         if(puflen > 0){
            if(rc == OK){
               process_buffer(puf, puflen, &count, sdata, data, cfg);
            }
            else {
               snprintf(savedbuf, sizeof(savedbuf)-1, "%s", puf);
               savedlen = puflen;
            }
         }

      } while(puflen > 0);

   } while(n > 0);

   if(data->import->fd != -1){
      import_the_file(sdata, data, cfg);
   }
}
