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
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <piler.h>
#include <zlib.h>


int store_index_data(struct session_data *sdata, struct parser_state *state, struct data *data, uint64 id, struct config *cfg){
   int rc=ERR;
   char *subj, *sender=state->b_from, *sender_domain=state->b_from_domain;
   struct sql sql;

   if(data->folder == 0){
      data->folder = get_folder_id_by_rule(data, state, sdata->tot_len, sdata->spam_message, cfg);
   }

   subj = state->b_subject;
   if(*subj == ' ') subj++;


   if(prepare_sql_statement(sdata, &sql, SQL_PREPARED_STMT_INSERT_INTO_SPHINX_TABLE) == ERR) return rc;


   fix_email_address_for_sphinx(state->b_from);
   fix_email_address_for_sphinx(state->b_sender);
   fix_email_address_for_sphinx(state->b_to);
   fix_email_address_for_sphinx(state->b_from_domain);
   fix_email_address_for_sphinx(state->b_sender_domain);
   fix_email_address_for_sphinx(state->b_to_domain);

   if(state->b_sender_domain[0]){
      sender = state->b_sender;
      sender_domain = state->b_sender_domain;
   }

   p_bind_init(&sql);

   sql.sql[sql.pos] = (char *)&id; sql.type[sql.pos] = TYPE_LONGLONG; sql.pos++;
   sql.sql[sql.pos] = sender; sql.type[sql.pos] = TYPE_STRING; sql.pos++;
   sql.sql[sql.pos] = state->b_to; sql.type[sql.pos] = TYPE_STRING; sql.pos++;
   sql.sql[sql.pos] = sender_domain; sql.type[sql.pos] = TYPE_STRING; sql.pos++;
   sql.sql[sql.pos] = state->b_to_domain; sql.type[sql.pos] = TYPE_STRING; sql.pos++;
   sql.sql[sql.pos] = subj; sql.type[sql.pos] = TYPE_STRING; sql.pos++;
   sql.sql[sql.pos] = state->b_body; sql.type[sql.pos] = TYPE_STRING; sql.pos++;
   sql.sql[sql.pos] = (char *)&sdata->now; sql.type[sql.pos] = TYPE_LONG; sql.pos++;
   sql.sql[sql.pos] = (char *)&sdata->sent; sql.type[sql.pos] = TYPE_LONG; sql.pos++;
   sql.sql[sql.pos] = (char *)&sdata->tot_len; sql.type[sql.pos] = TYPE_LONG; sql.pos++;
   sql.sql[sql.pos] = (char *)&sdata->direction; sql.type[sql.pos] = TYPE_LONG; sql.pos++;
   sql.sql[sql.pos] = (char *)&data->folder; sql.type[sql.pos] = TYPE_LONG; sql.pos++;
   sql.sql[sql.pos] = (char *)&state->n_attachments; sql.type[sql.pos] = TYPE_LONG; sql.pos++;
   sql.sql[sql.pos] = sdata->attachments; sql.type[sql.pos] = TYPE_STRING; sql.pos++;

   if(p_exec_stmt(sdata, &sql) == OK) rc = OK;
   else syslog(LOG_PRIORITY, "ERROR: %s failed to store index data for id=%llu, sql_errno=%d", sdata->ttmpfile, id, sdata->sql_errno);

   close_prepared_statement(&sql);

   return rc;
}


uint64 get_metaid_by_messageid(struct session_data *sdata, char *message_id, char *piler_id){
   uint64 id=0;
   struct sql sql;

   if(prepare_sql_statement(sdata, &sql, SQL_PREPARED_STMT_GET_META_ID_BY_MESSAGE_ID) == ERR) return id;

   p_bind_init(&sql);
   sql.sql[sql.pos] = message_id; sql.type[sql.pos] = TYPE_STRING; sql.pos++;

   if(p_exec_stmt(sdata, &sql) == OK){

      p_bind_init(&sql);

      sql.sql[sql.pos] = (char *)&id; sql.type[sql.pos] = TYPE_LONGLONG; sql.len[sql.pos] = sizeof(uint64); sql.pos++;
      sql.sql[sql.pos] = piler_id; sql.type[sql.pos] = TYPE_STRING; sql.len[sql.pos] = RND_STR_LEN; sql.pos++;

      p_store_results(&sql);

      p_fetch_results(&sql);

      p_free_results(&sql);
   }

   close_prepared_statement(&sql);

   return id;
}


int store_recipients(struct session_data *sdata, char *to, uint64 id, struct config *cfg){
   int rc=OK, n=0;
   char *p, *q, puf[SMALLBUFSIZE];
   struct sql sql;

   if(prepare_sql_statement(sdata, &sql, SQL_PREPARED_STMT_INSERT_INTO_RCPT_TABLE) == ERR) return ERR;

   p = to;
   do {
      p = split_str(p, " ", puf, sizeof(puf)-1);

      q = strchr(puf, '@');

      if(q && strlen(q) > 3 && does_it_seem_like_an_email_address(puf) == 1){
         q++;

         p_bind_init(&sql);

         sql.sql[sql.pos] = (char *)&id; sql.type[sql.pos] = TYPE_LONGLONG; sql.pos++;
         sql.sql[sql.pos] = &puf[0]; sql.type[sql.pos] = TYPE_STRING; sql.pos++;
         sql.sql[sql.pos] = q; sql.type[sql.pos] = TYPE_STRING; sql.pos++;

         if(p_exec_stmt(sdata, &sql) == ERR){
            if(sdata->sql_errno != ER_DUP_ENTRY){
               syslog(LOG_PRIORITY, "ERROR: %s: failed to add '%s' for id=%llu to rcpt table, sql_errno=%d", sdata->ttmpfile, puf, id, sdata->sql_errno);
               rc = ERR;
            }
         }
         else n++;
      }

   } while(p);

   close_prepared_statement(&sql);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: stored %d recipients, rc=%d", sdata->ttmpfile, n, rc);

   return rc;
}


int store_folder_id(struct session_data *sdata, struct data *data, uint64 id, struct config *cfg){
   int rc=ERR;
   struct sql sql;

   if(data->folder == ERR_FOLDER) return rc;

   if(prepare_sql_statement(sdata, &sql, SQL_PREPARED_STMT_INSERT_FOLDER_MESSAGE) == ERR) return rc;

   p_bind_init(&sql);

   sql.sql[sql.pos] = (char *)&data->folder; sql.type[sql.pos] = TYPE_LONGLONG; sql.pos++;
   sql.sql[sql.pos] = (char *)&id; sql.type[sql.pos] = TYPE_LONGLONG; sql.pos++;

   if(p_exec_stmt(sdata, &sql) == OK) rc = OK;
   close_prepared_statement(&sql);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: stored folderdata, rc=%d", sdata->ttmpfile, rc);

   return rc;
}


int update_metadata_reference(struct session_data *sdata, struct parser_state *state, char *ref, struct config *cfg){
   int ret = ERR;
   struct sql sql;

   if(prepare_sql_statement(sdata, &sql, SQL_PREPARED_STMT_UPDATE_METADATA_REFERENCE) == ERR) return ret;

   p_bind_init(&sql);

   sql.sql[sql.pos] = ref; sql.type[sql.pos] = TYPE_STRING; sql.pos++;
   sql.sql[sql.pos] = state->reference; sql.type[sql.pos] = TYPE_STRING; sql.pos++;

   if(p_exec_stmt(sdata, &sql) == OK) ret = OK;

   close_prepared_statement(&sql);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: updated meta reference for '%s', rc=%d", sdata->ttmpfile, state->reference, ret);

   return ret;
}


int store_meta_data(struct session_data *sdata, struct parser_state *state, struct data *data, struct config *cfg){
   int rc=ERR;
   char *subj, *sender, *sender_domain, s[MAXBUFSIZE], s2[SMALLBUFSIZE], vcode[2*DIGEST_LENGTH+1], ref[2*DIGEST_LENGTH+1];
   uint64 id=0;
   struct sql sql;

   subj = state->b_subject;
   if(*subj == ' ') subj++;

   if(state->b_sender_domain[0]){
      sender = state->b_sender;
      sender_domain = state->b_sender_domain;
      get_first_email_address_from_string(state->b_sender, s2, sizeof(s2));
   } else {
      sender = state->b_from;
      sender_domain = state->b_from_domain;
      get_first_email_address_from_string(state->b_from, s2, sizeof(s2));
   }

   snprintf(s, sizeof(s)-1, "%llu+%s%s%s%ld%ld%ld%d%d%d%d%s%s%s", id, subj, sender, state->message_id, sdata->now, sdata->sent, sdata->retained, sdata->tot_len, sdata->hdr_len, sdata->direction, state->n_attachments, sdata->ttmpfile, sdata->digest, sdata->bodydigest);

   digest_string(s, &vcode[0]);

   memset(ref, 0, sizeof(ref));
   if(strlen(state->reference) > 10){
      digest_string(state->reference, &ref[0]);
      update_metadata_reference(sdata, state, &ref[0], cfg);
   }
   else if(state->reference[0] == 0){
      // during import, the order of messages is often random
      // check if this is a message which is already referenced
      uint64 count=0;

      digest_string(state->message_id, &ref[0]);
      if(prepare_sql_statement(sdata, &sql, SQL_PREPARED_STMT_GET_METADATA_REFERENCE) != ERR){
         p_bind_init(&sql);

         sql.sql[sql.pos] = &ref[0]; sql.type[sql.pos] = TYPE_STRING; sql.pos++;

         if(p_exec_stmt(sdata, &sql) == OK){
	    p_bind_init(&sql);

	    sql.sql[sql.pos] = (char *)&count; sql.type[sql.pos] = TYPE_LONGLONG; sql.len[sql.pos] = sizeof(uint64); sql.pos++;
	    p_store_results(&sql);
	    p_fetch_results(&sql);
	    p_free_results(&sql);
	 }
      }

      close_prepared_statement(&sql);

      // no reference yet
      if(count <= 0)
         ref[0] = 0;
   }


   if(prepare_sql_statement(sdata, &sql, SQL_PREPARED_STMT_INSERT_INTO_META_TABLE) == ERR) return ERR;

   if(strlen(state->b_to) < 5){
      snprintf(state->b_to, SMALLBUFSIZE-1, "undisclosed-recipients@no.domain");
   }


   p_bind_init(&sql);

   sql.sql[sql.pos] = &s2[0]; sql.type[sql.pos] = TYPE_STRING; sql.pos++;
   sql.sql[sql.pos] = sender_domain; sql.type[sql.pos] = TYPE_STRING; sql.pos++;
   sql.sql[sql.pos] = subj; sql.type[sql.pos] = TYPE_STRING; sql.pos++;
   sql.sql[sql.pos] = (char *)&sdata->spam_message; sql.type[sql.pos] = TYPE_LONG; sql.pos++;
   sql.sql[sql.pos] = (char *)&sdata->now; sql.type[sql.pos] = TYPE_LONG; sql.pos++;
   sql.sql[sql.pos] = (char *)&sdata->sent; sql.type[sql.pos] = TYPE_LONG; sql.pos++;
   sql.sql[sql.pos] = (char *)&sdata->retained; sql.type[sql.pos] = TYPE_LONGLONG; sql.pos++;
   sql.sql[sql.pos] = (char *)&sdata->tot_len; sql.type[sql.pos] = TYPE_LONG; sql.pos++;
   sql.sql[sql.pos] = (char *)&sdata->hdr_len; sql.type[sql.pos] = TYPE_LONG; sql.pos++;
   sql.sql[sql.pos] = (char *)&sdata->direction; sql.type[sql.pos] = TYPE_LONG; sql.pos++;
   sql.sql[sql.pos] = (char *)&state->n_attachments; sql.type[sql.pos] = TYPE_LONG; sql.pos++;
   sql.sql[sql.pos] = sdata->ttmpfile; sql.type[sql.pos] = TYPE_STRING; sql.pos++;
   sql.sql[sql.pos] = state->message_id; sql.type[sql.pos] = TYPE_STRING; sql.pos++;
   sql.sql[sql.pos] = &ref[0]; sql.type[sql.pos] = TYPE_STRING; sql.pos++;
   sql.sql[sql.pos] = sdata->digest; sql.type[sql.pos] = TYPE_STRING; sql.pos++;
   sql.sql[sql.pos] = sdata->bodydigest; sql.type[sql.pos] = TYPE_STRING; sql.pos++;
   sql.sql[sql.pos] = &vcode[0]; sql.type[sql.pos] = TYPE_STRING; sql.pos++;

   if(p_exec_stmt(sdata, &sql) == OK){
      id = p_get_insert_id(&sql);

      if(store_recipients(sdata, state->b_to, id, cfg) == OK){

         if(store_index_data(sdata, state, data, id, cfg) == OK) rc = OK;

         if(cfg->enable_folders == 1){
            rc = store_folder_id(sdata, data, id, cfg);
         }
      }

      // There were some sql errors, so we should rollback everything
      if(rc == ERR){
         rollback(sdata, state, id, cfg);
      }

   }
   else {
      syslog(LOG_PRIORITY, "ERROR: %s storing metadata, sql_errno=%d", sdata->ttmpfile, sdata->sql_errno);
   }

   close_prepared_statement(&sql);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: stored metadata, rc=%d",  sdata->ttmpfile, rc);

   return rc;
}


void rollback(struct session_data *sdata, struct parser_state *state, uint64 id, struct config *cfg){
   char buf[SMALLBUFSIZE];

   snprintf(buf, sizeof(buf)-2, "DELETE FROM %s WHERE id=%llu", SQL_SPHINX_TABLE, id);
   p_query(sdata, buf);
   syslog(LOG_PRIORITY, "ERROR: %s: rollback sql stmt=%s", sdata->ttmpfile, buf);

   snprintf(buf, sizeof(buf)-2, "DELETE FROM %s WHERE id=%llu", SQL_RECIPIENT_TABLE, id);
   p_query(sdata, buf);
   syslog(LOG_PRIORITY, "ERROR: %s: rollback sql stmt=%s", sdata->ttmpfile, buf);

   snprintf(buf, sizeof(buf)-2, "DELETE FROM %s WHERE id=%llu", SQL_METADATA_TABLE, id);
   p_query(sdata, buf);
   syslog(LOG_PRIORITY, "ERROR: %s: rollback sql stmt=%s", sdata->ttmpfile, buf);

   snprintf(buf, sizeof(buf)-2, "DELETE FROM %s WHERE piler_id='%s'", SQL_ATTACHMENT_TABLE, sdata->ttmpfile);
   p_query(sdata, buf);
   syslog(LOG_PRIORITY, "ERROR: %s: rollback sql stmt=%s", sdata->ttmpfile, buf);

   if(cfg->enable_folders == 1){
      snprintf(buf, sizeof(buf)-1, "DELETE FROM " SQL_FOLDER_MESSAGE_TABLE " WHERE id=%llu", id);
      p_query(sdata, buf);
      syslog(LOG_PRIORITY, "ERROR: %s: rollback sql stmt=%s", sdata->ttmpfile, buf);
   }

   remove_stored_message_files(sdata, state, cfg);
}


void remove_stripped_attachments(struct parser_state *state){
   int i;

   for(i=1; i<=state->n_attachments; i++) unlink(state->attachments[i].internalname);
}


int is_duplicated_message(struct session_data *sdata, struct parser_state *state, struct data *data, struct config *cfg){
   int fd;
   char piler_id[SMALLBUFSIZE];

   /* discard if existing message_id */

   sdata->duplicate_id = get_metaid_by_messageid(sdata, state->message_id, piler_id);

   if(sdata->duplicate_id > 0){
      remove_stripped_attachments(state);

      if(strlen(state->b_journal_to) > 0){
         if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: trying to add journal rcpt (%s) to id=%llu for message-id: '%s'", sdata->ttmpfile, state->b_journal_to, sdata->duplicate_id, state->message_id);
         store_recipients(sdata, state->b_journal_to, sdata->duplicate_id, cfg);
      }

      return ERR_EXISTS;
   }

   fd = open(state->message_id_hash, O_CREAT|O_EXCL, S_IRUSR|S_IWUSR);
   if(fd == -1){
      remove_stripped_attachments(state);
      syslog(LOG_PRIORITY, "%s: touch %s FAILED (%s), error: %s", sdata->ttmpfile, state->message_id_hash, state->message_id, strerror(errno));
      return ERR_EXISTS;
   }
   close(fd);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: touch %s OK (%s)", sdata->ttmpfile, state->message_id_hash, state->message_id);


   if(cfg->mmap_dedup_test == 1 && data->dedup != MAP_FAILED && data->child_serial >= 0 && data->child_serial < MAXCHILDREN){

      if(strstr(data->dedup, state->message_id_hash)){
         if(cfg->verbosity >= _LOG_DEBUG){
            syslog(LOG_INFO, "%s: dedup string: %s", sdata->ttmpfile, data->dedup);
            syslog(LOG_INFO, "%s: message-id-hash=%s, serial=%d", sdata->ttmpfile, state->message_id_hash, data->child_serial);
         }

         remove_stripped_attachments(state);
         return ERR_EXISTS;
      }

      memcpy(data->dedup + data->child_serial*DIGEST_LENGTH*2, state->message_id_hash, DIGEST_LENGTH*2);
   }

   return OK;
}


int process_message(struct session_data *sdata, struct parser_state *state, struct data *data, struct config *cfg){

   if(is_duplicated_message(sdata, state, data, cfg) == ERR_EXISTS)
      return ERR_EXISTS;

   sdata->retained += query_retain_period(data, state, sdata->tot_len, sdata->spam_message, cfg);


   if(state->n_attachments > 0 && store_attachments(sdata, state, cfg) == ERR) return ERR;


   if(store_file(sdata, sdata->tmpframe, 0, cfg) == 0){
      syslog(LOG_PRIORITY, "ERROR: %s: failed to store message: %s", sdata->ttmpfile, sdata->tmpframe);
      return ERR;
   }


   return store_meta_data(sdata, state, data, cfg);
}
