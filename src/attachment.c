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
   int i, rc=1, found, affected_rows;
   char s[SMALLBUFSIZE];
   MYSQL_RES *res;
   MYSQL_ROW row;

   MYSQL_STMT *stmt;
   MYSQL_BIND bind[7];
   unsigned long len[7];


   snprintf(s, sizeof(s)-1, "INSERT INTO %s (`piler_id`,`attachment_id`,`sig`,`name`,`type`,`size`,`ptr`) VALUES(?,?,?,?,?,?,?)", SQL_ATTACHMENT_TABLE);

   if(prepare_a_mysql_statement(sdata, &stmt, s) == ERR) return rc;


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
               goto CLOSE;
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
         bind[3].buffer = state->attachments[i].filename;
         bind[3].is_null = 0;
         len[3] = strlen(state->attachments[i].filename); bind[3].length = &len[3];

         bind[4].buffer_type = MYSQL_TYPE_STRING;
         bind[4].buffer = state->attachments[i].type;
         bind[4].is_null = 0;
         len[4] = strlen(state->attachments[i].type); bind[4].length = &len[4];

         bind[5].buffer_type = MYSQL_TYPE_LONG;
         bind[5].buffer = (char *)&(state->attachments[i].size);
         bind[5].is_null = 0;
         bind[5].length = 0;

         bind[6].buffer_type = MYSQL_TYPE_LONGLONG;
         bind[6].buffer = (char *)&id;
         bind[6].is_null = 0;
         bind[6].length = 0;


         if(mysql_stmt_bind_param(stmt, bind)){
            syslog(LOG_PRIORITY, "%s: %s.mysql_stmt_bind_param() error: %s", sdata->ttmpfile, SQL_ATTACHMENT_TABLE, mysql_stmt_error(stmt));
            goto CLOSE;
         }


         if(mysql_stmt_execute(stmt)){
            syslog(LOG_PRIORITY, "%s attachment sql error: *%s*", sdata->ttmpfile, mysql_error(&(sdata->mysql)));
            goto CLOSE;
         }

         affected_rows = mysql_stmt_affected_rows(stmt);
         if(affected_rows != 1){
            syslog(LOG_PRIORITY, "%s attachment sql error: affected rows: %d", sdata->ttmpfile, affected_rows);
            goto CLOSE;
         }

      }
      else {
         syslog(LOG_PRIORITY, "%s: skipping attachment (serial: %d, size: %d, digest: %s)", sdata->ttmpfile, i, state->attachments[i].size, state->attachments[i].digest);
      }

   }

   rc = 0;

CLOSE:
   mysql_stmt_close(stmt);

   return rc;
}


int query_attachment_pointers(struct session_data *sdata, uint64 ptr, char *piler_id, int *id, struct __config *cfg){
   int rc=0;
   char s[SMALLBUFSIZE];
   MYSQL_STMT *stmt;
   MYSQL_BIND bind[2];
   my_bool is_null[2];
   unsigned long len=0;


   snprintf(s, SMALLBUFSIZE-1, "SELECT `piler_id`, `attachment_id` FROM %s WHERE id=?", SQL_ATTACHMENT_TABLE);

   if(prepare_a_mysql_statement(sdata, &stmt, s) == ERR) goto ENDE;

   memset(bind, 0, sizeof(bind));

   bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
   bind[0].buffer = (char *)&ptr;
   bind[0].is_null = 0;
   len = sizeof(uint64); bind[0].length = &len;


   if(mysql_stmt_bind_param(stmt, bind)){
      goto CLOSE;
   }


   if(mysql_stmt_execute(stmt)){
      goto CLOSE;
   }


   memset(bind, 0, sizeof(bind));

   bind[0].buffer_type = MYSQL_TYPE_STRING;
   bind[0].buffer = piler_id;
   bind[0].buffer_length = RND_STR_LEN;
   bind[0].is_null = &is_null[0];
   bind[0].length = &len;

   bind[1].buffer_type = MYSQL_TYPE_LONG;
   bind[1].buffer = (char *)id;
   bind[1].is_null = 0;
   bind[1].length = 0;


   if(mysql_stmt_bind_result(stmt, bind)){
      goto CLOSE;
   }


   if(mysql_stmt_store_result(stmt)){
      goto CLOSE;
   }

   if(!mysql_stmt_fetch(stmt)){
      if(is_null[0] == 0){
         rc = 1;
      }
   }

CLOSE:
   mysql_stmt_close(stmt);

ENDE:

   return rc;
}


int query_attachments(struct session_data *sdata, struct ptr_array *ptr_arr, struct __config *cfg){
   int i, rc, id, attachments=0;
   uint64 ptr;
   char s[SMALLBUFSIZE];
   MYSQL_STMT *stmt;
   MYSQL_BIND bind[2];
   my_bool is_null[2];
   unsigned long len=0;


   for(i=0; i<MAX_ATTACHMENTS; i++) memset((char*)&ptr_arr[i], 0, sizeof(struct ptr_array));


   snprintf(s, SMALLBUFSIZE-1, "SELECT `attachment_id`, `ptr` FROM %s WHERE piler_id=? ORDER BY attachment_id ASC", SQL_ATTACHMENT_TABLE);

   if(prepare_a_mysql_statement(sdata, &stmt, s) == ERR) goto ENDE;


   memset(bind, 0, sizeof(bind));

   bind[0].buffer_type = MYSQL_TYPE_STRING;
   bind[0].buffer = sdata->ttmpfile;
   bind[0].is_null = 0;
   len = strlen(sdata->ttmpfile); bind[0].length = &len;

   if(mysql_stmt_bind_param(stmt, bind)){
      goto CLOSE;
   }


   if(mysql_stmt_execute(stmt)){
      goto CLOSE;
   }


   memset(bind, 0, sizeof(bind));

   bind[0].buffer_type = MYSQL_TYPE_LONG;
   bind[0].buffer = (char *)&id;
   bind[0].is_null = &is_null[0];
   bind[0].length = 0;

   bind[1].buffer_type = MYSQL_TYPE_LONGLONG;
   bind[1].buffer = (char *)&ptr;
   bind[1].is_null = &is_null[1];
   bind[1].length = 0;



   if(mysql_stmt_bind_result(stmt, bind)){
      syslog(LOG_PRIORITY, "%s: %s.mysql_stmt_bind_result() error: %s", sdata->ttmpfile, SQL_METADATA_TABLE, mysql_stmt_error(stmt));
      goto CLOSE;
   }


   if(mysql_stmt_store_result(stmt)){
      syslog(LOG_PRIORITY, "%s: %s.mysql_stmt_store_result() error: %s", sdata->ttmpfile, SQL_METADATA_TABLE, mysql_stmt_error(stmt));
      goto CLOSE;
   }

   while(!mysql_stmt_fetch(stmt)){

      if(id > 0 && id < MAX_ATTACHMENTS){
         if(ptr > 0){
            ptr_arr[id].ptr = ptr;
            rc = query_attachment_pointers(sdata, ptr, &(ptr_arr[id].piler_id[0]), &(ptr_arr[id].attachment_id), cfg);
            if(!rc){
               attachments = -1;
               goto CLOSE;
            }
         }
         else {
            snprintf(ptr_arr[id].piler_id, sizeof(ptr_arr[id].piler_id)-1, "%s", sdata->ttmpfile);
            ptr_arr[id].attachment_id = id;
         }

         attachments++;
      }
   }

CLOSE:
   mysql_stmt_close(stmt);

ENDE:

   return attachments;
}


