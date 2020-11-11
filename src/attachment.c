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


int store_attachments(struct session_data *sdata, struct parser_state *state, struct config *cfg){
   int i, rc=1, found, affected_rows;
   struct sql sql, sql2;

   if(prepare_sql_statement(sdata, &sql, SQL_PREPARED_STMT_INSERT_INTO_ATTACHMENT_TABLE) == ERR) return rc;
   if(prepare_sql_statement(sdata, &sql2, SQL_PREPARED_STMT_GET_ATTACHMENT_ID_BY_SIGNATURE) == ERR) return rc;


   for(i=1; i<=state->n_attachments; i++){
      found = 0;
      uint64 id = 0;

      if(state->attachments[i].size > 0){

         p_bind_init(&sql2);

         sql2.sql[sql2.pos] = state->attachments[i].digest; sql2.type[sql2.pos] = TYPE_STRING; sql2.pos++;
         sql2.sql[sql2.pos] = (char *)&(state->attachments[i].size); sql2.type[sql2.pos] = TYPE_LONG; sql2.pos++;

         if(p_exec_stmt(sdata, &sql2) == OK){

            p_bind_init(&sql2);

            sql2.sql[sql2.pos] = (char *)&id; sql2.type[sql2.pos] = TYPE_LONGLONG; sql2.len[sql2.pos] = sizeof(uint64); sql2.pos++;

            p_store_results(&sql2);
            if(p_fetch_results(&sql2) == OK) found = 1;
            p_free_results(&sql2);
         }

         if(found == 0){
            if(store_file(sdata, state->attachments[i].internalname, 0, cfg) == 0){
               syslog(LOG_PRIORITY, "%s: error storing attachment: %s", sdata->ttmpfile, state->attachments[i].internalname);
               goto CLOSE;
            }
         }


         p_bind_init(&sql);

         sql.sql[sql.pos] = sdata->ttmpfile; sql.type[sql.pos] = TYPE_STRING; sql.pos++;
         sql.sql[sql.pos] = (char *)&i; sql.type[sql.pos] = TYPE_LONG; sql.pos++;
         sql.sql[sql.pos] = state->attachments[i].digest; sql.type[sql.pos] = TYPE_STRING; sql.pos++;
         sql.sql[sql.pos] = state->attachments[i].filename; sql.type[sql.pos] = TYPE_STRING; sql.pos++;
         sql.sql[sql.pos] = state->attachments[i].type; sql.type[sql.pos] = TYPE_STRING; sql.pos++;
         sql.sql[sql.pos] = (char *)&(state->attachments[i].size); sql.type[sql.pos] = TYPE_LONG; sql.pos++;
         sql.sql[sql.pos] = (char *)&id; sql.type[sql.pos] = TYPE_LONGLONG; sql.pos++;

         if(p_exec_stmt(sdata, &sql) == ERR) goto CLOSE;


         affected_rows = p_get_affected_rows(&sql);
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
   close_prepared_statement(&sql);
   close_prepared_statement(&sql2);

   return rc;
}


int query_attachment_pointers(struct session_data *sdata, uint64 ptr, char *piler_id, int *id){
   int rc=0;
   struct sql sql;

   if(prepare_sql_statement(sdata, &sql, SQL_PREPARED_STMT_GET_ATTACHMENT_POINTER) == ERR) return rc;

   p_bind_init(&sql);

   sql.sql[sql.pos] = (char *)&ptr; sql.type[sql.pos] = TYPE_LONGLONG; sql.pos++;

   if(p_exec_stmt(sdata, &sql) == OK){

      p_bind_init(&sql);

      sql.sql[sql.pos] = piler_id; sql.type[sql.pos] = TYPE_STRING; sql.len[sql.pos] = RND_STR_LEN; sql.pos++;
      sql.sql[sql.pos] = (char *)id; sql.type[sql.pos] = TYPE_LONG; sql.len[sql.pos] = sizeof(int); sql.pos++;

      p_store_results(&sql);

      if(p_fetch_results(&sql) == OK) rc = 1;
      p_free_results(&sql);
   }

   close_prepared_statement(&sql);

   return rc;
}


int query_attachments(struct session_data *sdata, struct ptr_array *ptr_arr){
   int i, rc, id, attachments=0;
   uint64 ptr;
   struct sql sql;

   for(i=0; i<MAX_ATTACHMENTS; i++) memset((char*)&ptr_arr[i], 0, sizeof(struct ptr_array));

   if(prepare_sql_statement(sdata, &sql, SQL_PREPARED_STMT_QUERY_ATTACHMENT) == ERR) return attachments;

   p_bind_init(&sql);

   sql.sql[sql.pos] = sdata->ttmpfile; sql.type[sql.pos] = TYPE_STRING; sql.pos++;

   if(p_exec_stmt(sdata, &sql) == ERR) goto CLOSE;

   p_bind_init(&sql);

   sql.sql[sql.pos] = (char *)&id; sql.type[sql.pos] = TYPE_LONG; sql.len[sql.pos] = sizeof(int); sql.pos++;
   sql.sql[sql.pos] = (char *)&ptr; sql.type[sql.pos] = TYPE_LONGLONG; sql.len[sql.pos] = sizeof(uint64); sql.pos++;

   p_store_results(&sql);

   while(p_fetch_results(&sql) == OK){

      if(id > 0 && id < MAX_ATTACHMENTS){
         if(ptr > 0){
            ptr_arr[id].ptr = ptr;
            rc = query_attachment_pointers(sdata, ptr, &(ptr_arr[id].piler_id[0]), &(ptr_arr[id].attachment_id));
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

   p_free_results(&sql);

CLOSE:
   close_prepared_statement(&sql);

   return attachments;
}
