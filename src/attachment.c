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
   int i, found, affected_rows;
   char s[SMALLBUFSIZE];
   MYSQL_RES *res;
   MYSQL_ROW row;

   MYSQL_STMT *stmt;
   MYSQL_BIND bind[6];
   unsigned long len[6];


   stmt = mysql_stmt_init(&(sdata->mysql));
   if(!stmt){
      if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: %s.mysql_stmt_init() error", sdata->ttmpfile, SQL_ATTACHMENT_TABLE);
      return 1;
   }

   snprintf(s, sizeof(s)-1, "INSERT INTO %s (`piler_id`,`attachment_id`,`sig`,`type`,`size`,`ptr`) VALUES(?,?,?,?,?,?)", SQL_ATTACHMENT_TABLE);

   if(mysql_stmt_prepare(stmt, s, strlen(s))){
      syslog(LOG_PRIORITY, "%s: %s.mysql_stmt_prepare() error: %s", sdata->ttmpfile, SQL_ATTACHMENT_TABLE, mysql_stmt_error(stmt));
      return 1;
   }


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


         memset(bind, 0, sizeof(bind));

         bind[0].buffer_type = MYSQL_TYPE_STRING;
         bind[0].buffer = sdata->ttmpfile;
         bind[0].is_null = 0;
         len[0] = strlen(sdata->ttmpfile); bind[0].length = &len[0];

         bind[1].buffer_type = MYSQL_TYPE_LONG;
         bind[1].buffer = (char *)&i;
         bind[1].is_null = 0;
         bind[1].length = 0;

         bind[2].buffer_type = MYSQL_TYPE_STRING;
         bind[2].buffer = state->attachments[i].digest;
         bind[2].is_null = 0;
         len[2] = strlen(state->attachments[i].digest); bind[2].length = &len[2];

         bind[3].buffer_type = MYSQL_TYPE_STRING;
         bind[3].buffer = state->attachments[i].type;
         bind[3].is_null = 0;
         len[3] = strlen(state->attachments[i].digest); bind[3].length = &len[3];

         bind[4].buffer_type = MYSQL_TYPE_LONG;
         bind[4].buffer = (char *)&(state->attachments[i].size);
         bind[4].is_null = 0;
         bind[4].length = 0;

         bind[5].buffer_type = MYSQL_TYPE_LONGLONG;
         bind[5].buffer = (char *)&id;
         bind[5].is_null = 0;
         bind[5].length = 0;


         if(mysql_stmt_bind_param(stmt, bind)){
            syslog(LOG_PRIORITY, "%s: %s.mysql_stmt_bind_param() error: %s", sdata->ttmpfile, SQL_ATTACHMENT_TABLE, mysql_stmt_error(stmt));
            return 1;
         }


         if(mysql_stmt_execute(stmt)){
            syslog(LOG_PRIORITY, "%s attachment sql error: *%s*", sdata->ttmpfile, mysql_error(&(sdata->mysql)));
            return 1;
         }

         affected_rows = mysql_stmt_affected_rows(stmt);
         if(affected_rows != 1){
            syslog(LOG_PRIORITY, "%s attachment sql error: affected rows: %d", sdata->ttmpfile, affected_rows);
            return 1;
         }

      }
      else {
         syslog(LOG_PRIORITY, "%s: skipping attachment (serial: %d, size: %d, digest: %s)", sdata->ttmpfile, i, state->attachments[i].size, state->attachments[i].digest);
      }

   }

   return 0;
}

