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


int store_attachments(struct session_data *sdata, struct parser_state *state, struct __data *data, struct __config *cfg){
   uint64 id=0;
   int i, rc=1, found, affected_rows;


   if(prepare_sql_statement(sdata, &(data->stmt_insert_into_attachment_table), SQL_PREPARED_STMT_INSERT_INTO_ATTACHMENT_TABLE, cfg) == ERR) return rc;
   if(prepare_sql_statement(sdata, &(data->stmt_get_attachment_id_by_signature), SQL_PREPARED_STMT_GET_ATTACHMENT_ID_BY_SIGNATURE, cfg) == ERR) return rc;


   for(i=1; i<=state->n_attachments; i++){
      found = 0;
      id = 0;

      if(state->attachments[i].size > 0){

         p_bind_init(data);

         data->sql[data->pos] = state->attachments[i].digest; data->type[data->pos] = TYPE_STRING; data->pos++;
         data->sql[data->pos] = (char *)&(state->attachments[i].size); data->type[data->pos] = TYPE_LONG; data->pos++;

         if(p_exec_query(sdata, data->stmt_get_attachment_id_by_signature, data) == ERR) goto NOT_FOUND;


         p_bind_init(data);

         data->sql[data->pos] = (char *)&id; data->type[data->pos] = TYPE_LONGLONG; data->len[data->pos] = sizeof(uint64); data->pos++;

         p_store_results(sdata, data->stmt_get_attachment_id_by_signature, data);
         if(p_fetch_results(data->stmt_get_attachment_id_by_signature) == OK) found = 1;
         p_free_results(data->stmt_get_attachment_id_by_signature);

NOT_FOUND:

         if(found == 0){
            if(store_file(sdata, state->attachments[i].internalname, 0, 0, cfg) == 0){
               syslog(LOG_PRIORITY, "%s: error storing attachment: %s", sdata->ttmpfile, state->attachments[i].internalname);
               goto CLOSE;
            }
         }


         p_bind_init(data);

         data->sql[data->pos] = sdata->ttmpfile; data->type[data->pos] = TYPE_STRING; data->pos++;
         data->sql[data->pos] = (char *)&i; data->type[data->pos] = TYPE_LONG; data->pos++;
         data->sql[data->pos] = state->attachments[i].digest; data->type[data->pos] = TYPE_STRING; data->pos++;
         data->sql[data->pos] = state->attachments[i].filename; data->type[data->pos] = TYPE_STRING; data->pos++;
         data->sql[data->pos] = state->attachments[i].type; data->type[data->pos] = TYPE_STRING; data->pos++;
         data->sql[data->pos] = (char *)&(state->attachments[i].size); data->type[data->pos] = TYPE_LONG; data->pos++;
         data->sql[data->pos] = (char *)&id; data->type[data->pos] = TYPE_LONGLONG; data->pos++;

         if(p_exec_query(sdata, data->stmt_insert_into_attachment_table, data) == ERR) goto CLOSE;


         affected_rows = p_get_affected_rows(data->stmt_insert_into_attachment_table);
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
   close_prepared_statement(data->stmt_insert_into_attachment_table);
   close_prepared_statement(data->stmt_get_attachment_id_by_signature);

   return rc;
}


int query_attachment_pointers(struct session_data *sdata, struct __data *data, uint64 ptr, char *piler_id, int *id, struct __config *cfg){
   int rc=0;

   if(prepare_sql_statement(sdata, &(data->stmt_get_attachment_pointer), SQL_PREPARED_STMT_GET_ATTACHMENT_POINTER, cfg) == ERR) return rc;


   p_bind_init(data);

   data->sql[data->pos] = (char *)&ptr; data->type[data->pos] = TYPE_LONGLONG; data->pos++;

   if(p_exec_query(sdata, data->stmt_get_attachment_pointer, data) == ERR) goto ENDE;



   p_bind_init(data);

   data->sql[data->pos] = piler_id; data->type[data->pos] = TYPE_STRING; data->len[data->pos] = RND_STR_LEN; data->pos++;
   data->sql[data->pos] = (char *)id; data->type[data->pos] = TYPE_LONG; data->len[data->pos] = sizeof(int); data->pos++;

   p_store_results(sdata, data->stmt_get_attachment_pointer, data);

   if(p_fetch_results(data->stmt_get_attachment_pointer) == OK) rc = 1;
   p_free_results(data->stmt_get_attachment_pointer);

ENDE:
   close_prepared_statement(data->stmt_get_attachment_pointer);

   return rc;
}


int query_attachments(struct session_data *sdata, struct __data *data, struct ptr_array *ptr_arr, struct __config *cfg){
   int i, rc, id, attachments=0;
   uint64 ptr;

   for(i=0; i<MAX_ATTACHMENTS; i++) memset((char*)&ptr_arr[i], 0, sizeof(struct ptr_array));

   if(prepare_sql_statement(sdata, &(data->stmt_query_attachment), SQL_PREPARED_STMT_QUERY_ATTACHMENT, cfg) == ERR) goto ENDE;

   p_bind_init(data);

   data->sql[data->pos] = sdata->ttmpfile; data->type[data->pos] = TYPE_STRING; data->pos++;

   if(p_exec_query(sdata, data->stmt_query_attachment, data) == ERR) goto CLOSE;

   p_bind_init(data);

   data->sql[data->pos] = (char *)&id; data->type[data->pos] = TYPE_LONG; data->len[data->pos] = sizeof(int); data->pos++;
   data->sql[data->pos] = (char *)&ptr; data->type[data->pos] = TYPE_LONGLONG; data->len[data->pos] = sizeof(uint64); data->pos++;

   p_store_results(sdata, data->stmt_query_attachment, data);

   while(p_fetch_results(data->stmt_query_attachment) == OK){

      if(id > 0 && id < MAX_ATTACHMENTS){
         if(ptr > 0){
            ptr_arr[id].ptr = ptr;
            rc = query_attachment_pointers(sdata, data, ptr, &(ptr_arr[id].piler_id[0]), &(ptr_arr[id].attachment_id), cfg);
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

   p_free_results(data->stmt_query_attachment);

CLOSE:
   close_prepared_statement(data->stmt_query_attachment);

ENDE:

   return attachments;
}


