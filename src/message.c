/*
 * message.c, SJ
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
#include <zlib.h>


int store_index_data(struct session_data *sdata, struct _state *state, struct __data *data, uint64 id, struct __config *cfg){
   int rc=ERR;
   char *subj;

   MYSQL_BIND bind[14];
   unsigned long len[14];

   subj = state->b_subject;
   if(*subj == ' ') subj++;


   if(prepare_a_mysql_statement(sdata, &(data->stmt_insert_into_sphinx_table), SQL_PREPARED_STMT_INSERT_INTO_SPHINX_TABLE) == ERR) return rc;


   fix_email_address_for_sphinx(state->b_from);
   fix_email_address_for_sphinx(state->b_to);
   fix_email_address_for_sphinx(state->b_from_domain);
   fix_email_address_for_sphinx(state->b_to_domain);


   memset(bind, 0, sizeof(bind));

   bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
   bind[0].buffer = (char *)&id;
   bind[0].is_null = 0;
   bind[0].length = 0;

   bind[1].buffer_type = MYSQL_TYPE_STRING;
   bind[1].buffer = state->b_from;
   bind[1].is_null = 0;
   len[1] = strlen(state->b_from); bind[1].length = &len[1];

   bind[2].buffer_type = MYSQL_TYPE_STRING;
   bind[2].buffer = state->b_to;
   bind[2].is_null = 0;
   len[2] = strlen(state->b_to); bind[2].length = &len[2];

   bind[3].buffer_type = MYSQL_TYPE_STRING;
   bind[3].buffer = state->b_from_domain;
   bind[3].is_null = 0;
   len[3] = strlen(state->b_from_domain); bind[3].length = &len[3];

   bind[4].buffer_type = MYSQL_TYPE_STRING;
   bind[4].buffer = state->b_to_domain;
   bind[4].is_null = 0;
   len[4] = strlen(state->b_to_domain); bind[4].length = &len[4];

   bind[5].buffer_type = MYSQL_TYPE_STRING;
   bind[5].buffer = subj;
   bind[5].is_null = 0;
   len[5] = strlen(subj); bind[5].length = &len[5];

   bind[6].buffer_type = MYSQL_TYPE_STRING;
   bind[6].buffer = state->b_body;
   bind[6].is_null = 0;
   len[6] = strlen(state->b_body); bind[6].length = &len[6];

   bind[7].buffer_type = MYSQL_TYPE_LONG;
   bind[7].buffer = (char *)&sdata->now;
   bind[7].is_null = 0;
   bind[7].length = 0;

   bind[8].buffer_type = MYSQL_TYPE_LONG;
   bind[8].buffer = (char *)&sdata->sent;
   bind[8].is_null = 0;
   bind[8].length = 0;

   bind[9].buffer_type = MYSQL_TYPE_LONG;
   bind[9].buffer = (char *)&sdata->tot_len;
   bind[9].is_null = 0;
   bind[9].length = 0;

   bind[10].buffer_type = MYSQL_TYPE_LONG;
   bind[10].buffer = (char *)&sdata->direction;
   bind[10].is_null = 0;
   bind[10].length = 0;

   bind[11].buffer_type = MYSQL_TYPE_LONG;
   bind[11].buffer = (char *)&data->folder;
   bind[11].is_null = 0;
   bind[11].length = 0;

   bind[12].buffer_type = MYSQL_TYPE_LONG;
   bind[12].buffer = (char *)&state->n_attachments;
   bind[12].is_null = 0;
   bind[12].length = 0;

   bind[13].buffer_type = MYSQL_TYPE_STRING;
   bind[13].buffer = sdata->attachments;
   bind[13].is_null = 0;
   len[13] = strlen(sdata->attachments); bind[13].length = &len[13];


   if(mysql_stmt_bind_param(data->stmt_insert_into_sphinx_table, bind)){
      syslog(LOG_PRIORITY, "%s: %s.mysql_stmt_bind_param() error: %s", sdata->ttmpfile, SQL_SPHINX_TABLE, mysql_stmt_error(data->stmt_insert_into_sphinx_table));
      goto CLOSE;
   }


   if(mysql_stmt_execute(data->stmt_insert_into_sphinx_table)){
      syslog(LOG_PRIORITY, "%s: %s.mysql_stmt_execute error: *%s*", sdata->ttmpfile, SQL_SPHINX_TABLE, mysql_error(&(sdata->mysql)));
      goto CLOSE;
   }

   rc = OK;

CLOSE:
   mysql_stmt_close(data->stmt_insert_into_sphinx_table);


   return rc;
}


uint64 get_metaid_by_messageid(struct session_data *sdata, struct __data *data, char *message_id, struct __config *cfg){
   unsigned long len=0;
   uint64 id=0;
   MYSQL_BIND bind[1];

   if(prepare_a_mysql_statement(sdata, &(data->stmt_get_meta_id_by_message_id), SQL_PREPARED_STMT_GET_META_ID_BY_MESSAGE_ID) == ERR) return id;

   memset(bind, 0, sizeof(bind));

   bind[0].buffer_type = MYSQL_TYPE_STRING;
   bind[0].buffer = message_id;
   bind[0].is_null = 0;
   len = strlen(message_id); bind[0].length = &len;

   if(mysql_stmt_bind_param(data->stmt_get_meta_id_by_message_id, bind)){
      goto CLOSE;
   }

   if(mysql_stmt_execute(data->stmt_get_meta_id_by_message_id)){
      goto CLOSE;
   }

   memset(bind, 0, sizeof(bind));

   bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
   bind[0].buffer = (char *)&id;
   bind[0].is_null = 0;
   bind[0].length = 0;


   if(mysql_stmt_bind_result(data->stmt_get_meta_id_by_message_id, bind)){
      goto CLOSE;
   }


   if(mysql_stmt_store_result(data->stmt_get_meta_id_by_message_id)){
      goto CLOSE;
   }

   mysql_stmt_fetch(data->stmt_get_meta_id_by_message_id);

CLOSE:
   mysql_stmt_close(data->stmt_get_meta_id_by_message_id);

   return id;
}


int store_recipients(struct session_data *sdata, struct __data *data, char *to, uint64 id, int log_errors, struct __config *cfg){
   int ret=OK, n=0;
   char *p, *q, puf[SMALLBUFSIZE];

   MYSQL_BIND bind[3];
   unsigned long len[3];


   if(prepare_a_mysql_statement(sdata, &(data->stmt_insert_into_rcpt_table), SQL_PREPARED_STMT_INSERT_INTO_RCPT_TABLE) == ERR) return ERR;


   p = to;
   do {
      p = split_str(p, " ", puf, sizeof(puf)-1);

      q = strchr(puf, '@');

      if(q && strlen(q) > 3 && does_it_seem_like_an_email_address(puf) == 1){
         q++;

         memset(bind, 0, sizeof(bind));

         bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
         bind[0].buffer = (char *)&id;
         bind[0].is_null = 0;
         bind[0].length = 0;

         bind[1].buffer_type = MYSQL_TYPE_STRING;
         bind[1].buffer = &puf[0];
         bind[1].is_null = 0;
         len[1] = strlen(puf); bind[1].length = &len[1];

         bind[2].buffer_type = MYSQL_TYPE_STRING;
         bind[2].buffer = q;
         bind[2].is_null = 0;
         len[2] = strlen(q); bind[2].length = &len[2];


         if(mysql_stmt_bind_param(data->stmt_insert_into_rcpt_table, bind)){
            syslog(LOG_PRIORITY, "%s: %s.mysql_stmt_bind_param() error: %s", sdata->ttmpfile, SQL_RECIPIENT_TABLE, mysql_stmt_error(data->stmt_insert_into_rcpt_table));
            ret = ERR;
            goto CLOSE;
         }


         if(mysql_stmt_execute(data->stmt_insert_into_rcpt_table) && log_errors == 1){
            syslog(LOG_PRIORITY, "%s: %s.mysql_stmt_execute error: *%s*", sdata->ttmpfile, SQL_RECIPIENT_TABLE, mysql_error(&(sdata->mysql)));
            ret = ERR;
         }
         else n++;
      }

   } while(p);


CLOSE:
   mysql_stmt_close(data->stmt_insert_into_rcpt_table);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: added %d recipients", sdata->ttmpfile, n);

   return ret;
}


int store_meta_data(struct session_data *sdata, struct _state *state, struct __data *data, struct __config *cfg){
   int rc, ret=ERR;
   char *subj, *p, s[MAXBUFSIZE], s2[SMALLBUFSIZE], vcode[2*DIGEST_LENGTH+1], ref[2*DIGEST_LENGTH+1];

   MYSQL_BIND bind[17];
   unsigned long len[17];

   my_ulonglong id=0;


   subj = state->b_subject;
   if(*subj == ' ') subj++;

   snprintf(s, sizeof(s)-1, "%llu+%s%s%s%ld%ld%ld%d%d%d%d%s%s%s", id, subj, state->b_from, state->message_id, sdata->now, sdata->sent, sdata->retained, sdata->tot_len, sdata->hdr_len, sdata->direction, state->n_attachments, sdata->ttmpfile, sdata->digest, sdata->bodydigest);

   digest_string(s, &vcode[0]);

   memset(ref, 0, sizeof(ref));
   if(strlen(state->reference) > 10) digest_string(state->reference, &ref[0]);


   if(prepare_a_mysql_statement(sdata, &(data->stmt_insert_into_meta_table), SQL_PREPARED_STMT_INSERT_INTO_META_TABLE) == ERR) return ERR;

   memset(s2, 0, sizeof(s2));

   p = state->b_from;
   do {
      memset(s2, 0, sizeof(s2));
      p = split(p, ' ', s2, sizeof(s2)-1);

      if(s2[0] == '\0') continue;

      if(does_it_seem_like_an_email_address(s2) == 1){ break; }
   } while(p);


   if(strlen(state->b_to) < 5){
      snprintf(state->b_to, SMALLBUFSIZE-1, "undisclosed-recipients@no.domain");
   }

   memset(bind, 0, sizeof(bind));

   bind[0].buffer_type = MYSQL_TYPE_STRING;
   bind[0].buffer = &s2[0];
   bind[0].is_null = 0;
   len[0] = strlen(s2); bind[0].length = &len[0];

   bind[1].buffer_type = MYSQL_TYPE_STRING;
   bind[1].buffer = state->b_from_domain;
   bind[1].is_null = 0;
   len[1] = strlen(state->b_from_domain); bind[1].length = &len[1];

   bind[2].buffer_type = MYSQL_TYPE_STRING;
   bind[2].buffer = subj;
   bind[2].is_null = 0;
   len[2] = strlen(subj); bind[2].length = &len[2];

   bind[3].buffer_type = MYSQL_TYPE_LONG;
   bind[3].buffer = (char *)&sdata->spam_message;
   bind[3].is_null = 0;
   bind[3].length = 0;

   bind[4].buffer_type = MYSQL_TYPE_LONG;
   bind[4].buffer = (char *)&sdata->now;
   bind[4].is_null = 0;
   bind[4].length = 0;

   bind[5].buffer_type = MYSQL_TYPE_LONG;
   bind[5].buffer = (char *)&sdata->sent;
   bind[5].is_null = 0;
   bind[5].length = 0;

   bind[6].buffer_type = MYSQL_TYPE_LONG;
   bind[6].buffer = (char *)&sdata->retained;
   bind[6].is_null = 0;
   bind[6].length = 0;

   bind[7].buffer_type = MYSQL_TYPE_LONG;
   bind[7].buffer = (char *)&sdata->tot_len;
   bind[7].is_null = 0;
   bind[7].length = 0;

   bind[8].buffer_type = MYSQL_TYPE_LONG;
   bind[8].buffer = (char *)&sdata->hdr_len;
   bind[8].is_null = 0;
   bind[8].length = 0;

   bind[9].buffer_type = MYSQL_TYPE_LONG;
   bind[9].buffer = (char *)&sdata->direction;
   bind[9].is_null = 0;
   bind[9].length = 0;

   bind[10].buffer_type = MYSQL_TYPE_LONG;
   bind[10].buffer = (char *)&state->n_attachments;
   bind[10].is_null = 0;
   bind[10].length = 0;

   bind[11].buffer_type = MYSQL_TYPE_STRING;
   bind[11].buffer = sdata->ttmpfile;
   bind[11].is_null = 0;
   len[11] = strlen(sdata->ttmpfile); bind[11].length = &len[11];

   bind[12].buffer_type = MYSQL_TYPE_STRING;
   bind[12].buffer = state->message_id;
   bind[12].is_null = 0;
   len[12] = strlen(state->message_id); bind[12].length = &len[12];

   bind[13].buffer_type = MYSQL_TYPE_STRING;
   bind[13].buffer = &ref[0];
   bind[13].is_null = 0;
   len[13] = strlen(ref); bind[13].length = &len[13];

   bind[14].buffer_type = MYSQL_TYPE_STRING;
   bind[14].buffer = sdata->digest;
   bind[14].is_null = 0;
   len[14] = strlen(sdata->digest); bind[14].length = &len[14];

   bind[15].buffer_type = MYSQL_TYPE_STRING;
   bind[15].buffer = sdata->bodydigest;
   bind[15].is_null = 0;
   len[15] = strlen(sdata->bodydigest); bind[15].length = &len[15];

   bind[16].buffer_type = MYSQL_TYPE_STRING;
   bind[16].buffer = &vcode[0];
   bind[16].is_null = 0;
   len[16] = strlen(vcode); bind[16].length = &len[16];



   if(mysql_stmt_bind_param(data->stmt_insert_into_meta_table, bind)){
      syslog(LOG_PRIORITY, "%s: %s.mysql_stmt_bind_param() error: %s", sdata->ttmpfile, SQL_METADATA_TABLE, mysql_stmt_error(data->stmt_insert_into_meta_table));
      goto CLOSE;
   }


   rc = mysql_stmt_execute(data->stmt_insert_into_meta_table);

   if(rc){
      syslog(LOG_PRIORITY, "%s: %s.mysql_stmt_execute() error: *%s*", sdata->ttmpfile, SQL_METADATA_TABLE, mysql_error(&(sdata->mysql)));
      ret = ERR_EXISTS;
   }
   else {
      id = mysql_stmt_insert_id(data->stmt_insert_into_meta_table);

      rc = store_recipients(sdata, data, state->b_to, id, 1, cfg);

      if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: stored recipients, rc=%d", sdata->ttmpfile, rc);

      if(rc == OK){

         rc = store_index_data(sdata, state, data, id, cfg);

         if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: stored indexdata, rc=%d", sdata->ttmpfile, rc);

         if(rc == OK)
            ret = OK;
      }
   }

CLOSE:
   mysql_stmt_close(data->stmt_insert_into_meta_table);

   return ret;
}


void remove_stripped_attachments(struct _state *state){
   int i;

   for(i=1; i<=state->n_attachments; i++) unlink(state->attachments[i].internalname);
}


int process_message(struct session_data *sdata, struct _state *state, struct __data *data, struct __config *cfg){
   int i, rc;
   uint64 id=0;

   /* discard if existing message_id */

   id = get_metaid_by_messageid(sdata, data, state->message_id, cfg);

   if(id > 0){
      remove_stripped_attachments(state);

      if(strlen(state->b_journal_to) > 0){
         if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: trying to add journal rcpt (%s) to id=%llu for message-id: '%s'", sdata->ttmpfile, state->b_journal_to, id, state->message_id);
         store_recipients(sdata, data, state->b_journal_to, id, 0, cfg);
      }

      return ERR_EXISTS;
   }


   /* store base64 encoded file attachments */

   if(state->n_attachments > 0){
      rc = store_attachments(sdata, state, data, cfg);

      for(i=1; i<=state->n_attachments; i++){
         unlink(state->attachments[i].internalname);
      }

      if(rc) return ERR;
   }


   rc = store_file(sdata, sdata->tmpframe, 0, 0, cfg);
   if(rc == 0){
      syslog(LOG_PRIORITY, "%s: error storing message: %s", sdata->ttmpfile, sdata->tmpframe);
      return ERR;
   }


   sdata->retained += query_retain_period(data->retention_rules, state, sdata->tot_len, sdata->spam_message, cfg);

   rc = store_meta_data(sdata, state, data, cfg);
   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: stored metadata, rc=%d",  sdata->ttmpfile, rc);
   if(rc == ERR_EXISTS){

      remove_stored_message_files(sdata, state, cfg);

      return ERR_EXISTS;
   }

   return OK;
}

