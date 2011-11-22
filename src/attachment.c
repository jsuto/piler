/*
 * attachment.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <piler.h>


int store_attachments(struct session_data *sdata, struct _state *state, struct __config *cfg){
   uint64 id=0;
   int i, found;
   char s[SMALLBUFSIZE];
   MYSQL_RES *res;
   MYSQL_ROW row;

   for(i=1; i<=state->n_attachments; i++){
      found = 0;
      id = 0;

      if(strlen(state->attachments[i].filename) > 4 && state->attachments[i].size > 10){

         snprintf(s, sizeof(s)-1, "SELECT `id` FROM `%s` WHERE `sig`='%s'", SQL_ATTACHMENT_TABLE, state->attachments[i].digest);

         if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: check for attachment sql: *%s*", sdata->ttmpfile, s);

         if(mysql_real_query(&(sdata->mysql), s, strlen(s)) == 0){
            res = mysql_store_result(&(sdata->mysql));
            if(res != NULL){
               row = mysql_fetch_row(res);
               if(row){
                  id = strtoull(row[0], NULL, 10);
                  found = 1;
               }
               mysql_free_result(res);
            }
         }


         if(found == 0){
            if(store_file(sdata, state->attachments[i].internalname, 0, 0, cfg) == 0){
               syslog(LOG_PRIORITY, "%s: error storing attachment: %s", sdata->ttmpfile, state->attachments[i].internalname);
               return 1;
            }
         }

         snprintf(s, sizeof(s)-1, "INSERT INTO %s (`piler_id`,`attachment_id`,`sig`,`ptr`) VALUES('%s',%d,'%s',%llu)", SQL_ATTACHMENT_TABLE, sdata->ttmpfile, i, state->attachments[i].digest, id);

         if(mysql_real_query(&(sdata->mysql), s, strlen(s))){
            syslog(LOG_PRIORITY, "%s attachment sql error: *%s*", sdata->ttmpfile, mysql_error(&(sdata->mysql)));
            return 1;
         }

      }
      else {
         syslog(LOG_PRIORITY, "%s: skipping attachment (serial: %d, size: %d, digest: %s)", sdata->ttmpfile, i, state->attachments[i].size, state->attachments[i].digest);
      }

   }

   return 0;
}

