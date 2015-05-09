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

   subj = state->b_subject;
   if(*subj == ' ') subj++;


   if(prepare_sql_statement(sdata, &(data->stmt_insert_into_sphinx_table), SQL_PREPARED_STMT_INSERT_INTO_SPHINX_TABLE) == ERR) return rc;


   fix_email_address_for_sphinx(state->b_from);
   fix_email_address_for_sphinx(state->b_to);
   fix_email_address_for_sphinx(state->b_from_domain);
   fix_email_address_for_sphinx(state->b_to_domain);


   p_bind_init(data);

   data->sql[data->pos] = (char *)&id; data->type[data->pos] = TYPE_LONGLONG; data->pos++;
   data->sql[data->pos] = state->b_from; data->type[data->pos] = TYPE_STRING; data->pos++;
   data->sql[data->pos] = state->b_to; data->type[data->pos] = TYPE_STRING; data->pos++;
   data->sql[data->pos] = state->b_from_domain; data->type[data->pos] = TYPE_STRING; data->pos++;
   data->sql[data->pos] = state->b_to_domain; data->type[data->pos] = TYPE_STRING; data->pos++;
   data->sql[data->pos] = subj; data->type[data->pos] = TYPE_STRING; data->pos++;
   data->sql[data->pos] = state->b_body; data->type[data->pos] = TYPE_STRING; data->pos++;
   data->sql[data->pos] = (char *)&sdata->now; data->type[data->pos] = TYPE_LONG; data->pos++;
   data->sql[data->pos] = (char *)&sdata->sent; data->type[data->pos] = TYPE_LONG; data->pos++;
   data->sql[data->pos] = (char *)&sdata->tot_len; data->type[data->pos] = TYPE_LONG; data->pos++;
   data->sql[data->pos] = (char *)&sdata->direction; data->type[data->pos] = TYPE_LONG; data->pos++;
   data->sql[data->pos] = (char *)&data->folder; data->type[data->pos] = TYPE_LONG; data->pos++;
   data->sql[data->pos] = (char *)&state->n_attachments; data->type[data->pos] = TYPE_LONG; data->pos++;
   data->sql[data->pos] = sdata->attachments; data->type[data->pos] = TYPE_STRING; data->pos++;

   if(p_exec_query(sdata, data->stmt_insert_into_sphinx_table, data) == OK) rc = OK;

   close_prepared_statement(data->stmt_insert_into_sphinx_table);


   return rc;
}


uint64 get_metaid_by_messageid(struct session_data *sdata, struct __data *data, char *message_id, struct __config *cfg){
   uint64 id=0;

   if(prepare_sql_statement(sdata, &(data->stmt_get_meta_id_by_message_id), SQL_PREPARED_STMT_GET_META_ID_BY_MESSAGE_ID) == ERR) return id;

   p_bind_init(data);
   data->sql[data->pos] = message_id; data->type[data->pos] = TYPE_STRING; data->pos++;

   if(p_exec_query(sdata, data->stmt_get_meta_id_by_message_id, data) == ERR) goto CLOSE;


   p_bind_init(data);
   data->sql[data->pos] = (char *)&id; data->type[data->pos] = TYPE_LONGLONG; data->len[data->pos] = sizeof(uint64); data->pos++;

   p_store_results(sdata, data->stmt_get_meta_id_by_message_id, data);

   p_fetch_results(data->stmt_get_meta_id_by_message_id);

   p_free_results(data->stmt_get_meta_id_by_message_id);

CLOSE:
   mysql_stmt_close(data->stmt_get_meta_id_by_message_id);

   return id;
}


int store_recipients(struct session_data *sdata, struct __data *data, char *to, uint64 id, struct __config *cfg){
   int ret=OK, n=0;
   char *p, *q, puf[SMALLBUFSIZE];

   if(prepare_sql_statement(sdata, &(data->stmt_insert_into_rcpt_table), SQL_PREPARED_STMT_INSERT_INTO_RCPT_TABLE) == ERR) return ret;

   p = to;
   do {
      p = split_str(p, " ", puf, sizeof(puf)-1);

      q = strchr(puf, '@');

      if(q && strlen(q) > 3 && does_it_seem_like_an_email_address(puf) == 1){
         q++;

         p_bind_init(data);

         data->sql[data->pos] = (char *)&id; data->type[data->pos] = TYPE_LONGLONG; data->pos++;
         data->sql[data->pos] = &puf[0]; data->type[data->pos] = TYPE_STRING; data->pos++;
         data->sql[data->pos] = q; data->type[data->pos] = TYPE_STRING; data->pos++;


         if(p_exec_query(sdata, data->stmt_insert_into_rcpt_table, data) == ERR){
            if(sdata->sql_errno != ER_DUP_ENTRY) ret = ERR;
         }
         else n++;
      }

   } while(p);

   mysql_stmt_close(data->stmt_insert_into_rcpt_table);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: added %d recipients", sdata->ttmpfile, n);

   return ret;
}


int update_metadata_reference(struct session_data *sdata, struct _state *state, struct __data *data, char *ref, struct __config *cfg){
   int ret = ERR;

   if(prepare_sql_statement(sdata, &(data->stmt_update_metadata_reference), SQL_PREPARED_STMT_UPDATE_METADATA_REFERENCE) == ERR) return ret;

   p_bind_init(data);

   data->sql[data->pos] = ref; data->type[data->pos] = TYPE_STRING; data->pos++;
   data->sql[data->pos] = state->reference; data->type[data->pos] = TYPE_STRING; data->pos++;

   if(p_exec_query(sdata, data->stmt_update_metadata_reference, data) == OK) ret = OK;

   mysql_stmt_close(data->stmt_update_metadata_reference);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: updated meta reference for '%s', rc=%d", sdata->ttmpfile, state->reference, ret);

   return ret;
}


int store_meta_data(struct session_data *sdata, struct _state *state, struct __data *data, struct __config *cfg){
   int rc, ret=ERR, result;
   char *subj, *p, s[MAXBUFSIZE], s2[SMALLBUFSIZE], vcode[2*DIGEST_LENGTH+1], ref[2*DIGEST_LENGTH+1];
   uint64 id=0;


   subj = state->b_subject;
   if(*subj == ' ') subj++;

   snprintf(s, sizeof(s)-1, "%llu+%s%s%s%ld%ld%ld%d%d%d%d%s%s%s", id, subj, state->b_from, state->message_id, sdata->now, sdata->sent, sdata->retained, sdata->tot_len, sdata->hdr_len, sdata->direction, state->n_attachments, sdata->ttmpfile, sdata->digest, sdata->bodydigest);

   digest_string(s, &vcode[0]);

   memset(ref, 0, sizeof(ref));
   if(strlen(state->reference) > 10){
      digest_string(state->reference, &ref[0]);
      update_metadata_reference(sdata, state, data, &ref[0], cfg);
   }


   if(prepare_sql_statement(sdata, &(data->stmt_insert_into_meta_table), SQL_PREPARED_STMT_INSERT_INTO_META_TABLE) == ERR) return ERR;

   memset(s2, 0, sizeof(s2));

   p = state->b_from;
   do {
      memset(s2, 0, sizeof(s2));
      p = split(p, ' ', s2, sizeof(s2)-1, &result);

      if(s2[0] == '\0') continue;

      if(does_it_seem_like_an_email_address(s2) == 1){ break; }
   } while(p);


   if(strlen(state->b_to) < 5){
      snprintf(state->b_to, SMALLBUFSIZE-1, "undisclosed-recipients@no.domain");
   }


   p_bind_init(data);

   data->sql[data->pos] = &s2[0]; data->type[data->pos] = TYPE_STRING; data->pos++;
   data->sql[data->pos] = state->b_from_domain; data->type[data->pos] = TYPE_STRING; data->pos++;
   data->sql[data->pos] = subj; data->type[data->pos] = TYPE_STRING; data->pos++;
   data->sql[data->pos] = (char *)&sdata->spam_message; data->type[data->pos] = TYPE_LONG; data->pos++;
   data->sql[data->pos] = (char *)&sdata->now; data->type[data->pos] = TYPE_LONG; data->pos++;
   data->sql[data->pos] = (char *)&sdata->sent; data->type[data->pos] = TYPE_LONG; data->pos++;
   data->sql[data->pos] = (char *)&sdata->retained; data->type[data->pos] = TYPE_LONG; data->pos++;
   data->sql[data->pos] = (char *)&sdata->tot_len; data->type[data->pos] = TYPE_LONG; data->pos++;
   data->sql[data->pos] = (char *)&sdata->hdr_len; data->type[data->pos] = TYPE_LONG; data->pos++;
   data->sql[data->pos] = (char *)&sdata->direction; data->type[data->pos] = TYPE_LONG; data->pos++;
   data->sql[data->pos] = (char *)&state->n_attachments; data->type[data->pos] = TYPE_LONG; data->pos++;
   data->sql[data->pos] = sdata->ttmpfile; data->type[data->pos] = TYPE_STRING; data->pos++;
   data->sql[data->pos] = state->message_id; data->type[data->pos] = TYPE_STRING; data->pos++;
   data->sql[data->pos] = &ref[0]; data->type[data->pos] = TYPE_STRING; data->pos++;
   data->sql[data->pos] = sdata->digest; data->type[data->pos] = TYPE_STRING; data->pos++;
   data->sql[data->pos] = sdata->bodydigest; data->type[data->pos] = TYPE_STRING; data->pos++;
   data->sql[data->pos] = &vcode[0]; data->type[data->pos] = TYPE_STRING; data->pos++;

   if(p_exec_query(sdata, data->stmt_insert_into_meta_table, data) == ERR){
      ret = ERR_EXISTS;
   }
   else {
      id = p_get_insert_id(data->stmt_insert_into_meta_table);

      rc = store_recipients(sdata, data, state->b_to, id, cfg);

      if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: stored recipients, rc=%d", sdata->ttmpfile, rc);

      rc = store_index_data(sdata, state, data, id, cfg);

      if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: stored indexdata, rc=%d", sdata->ttmpfile, rc);

      ret = OK;
   }

   close_prepared_statement(data->stmt_insert_into_meta_table);

   return ret;
}


void remove_stripped_attachments(struct _state *state){
   int i;

   for(i=1; i<=state->n_attachments; i++) unlink(state->attachments[i].internalname);
}


int process_message(struct session_data *sdata, struct _state *state, struct __data *data, struct __config *cfg){
   int rc, fd;

   /* discard if existing message_id */

   sdata->duplicate_id = get_metaid_by_messageid(sdata, data, state->message_id, cfg);

   if(sdata->duplicate_id > 0){
      remove_stripped_attachments(state);

      if(strlen(state->b_journal_to) > 0){
         if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: trying to add journal rcpt (%s) to id=%llu for message-id: '%s'", sdata->ttmpfile, state->b_journal_to, sdata->duplicate_id, state->message_id);
         store_recipients(sdata, data, state->b_journal_to, sdata->duplicate_id, cfg);
      }

      return ERR_EXISTS;
   }

   fd = open(state->message_id_hash, O_CREAT|O_EXCL, S_IRUSR|S_IWUSR);
   if(fd == -1){
      remove_stripped_attachments(state);
      if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: touch %s FAILED (%s)", sdata->ttmpfile, state->message_id_hash, state->message_id);
      return ERR_EXISTS;
   }
   close(fd);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: touch %s OK (%s)", sdata->ttmpfile, state->message_id_hash, state->message_id);

   /* store base64 encoded file attachments */

   if(state->n_attachments > 0){
      rc = store_attachments(sdata, state, data, cfg);

      remove_stripped_attachments(state);

      if(rc) return ERR;
   }


   rc = store_file(sdata, sdata->tmpframe, 0, 0, cfg);
   if(rc == 0){
      syslog(LOG_PRIORITY, "%s: error storing message: %s", sdata->ttmpfile, sdata->tmpframe);
      return ERR;
   }


   sdata->retained += query_retain_period(data, state, sdata->tot_len, sdata->spam_message, cfg);

   rc = store_meta_data(sdata, state, data, cfg);
   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: stored metadata, rc=%d",  sdata->ttmpfile, rc);
   if(rc == ERR_EXISTS){

      remove_stored_message_files(sdata, state, cfg);

      return ERR_EXISTS;
   }

   return OK;
}

