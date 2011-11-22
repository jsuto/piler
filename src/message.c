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


int is_existing_message_id(struct session_data *sdata, struct _state *state, struct __config *cfg){
   int rc=0;
   char s[SMALLBUFSIZE];
   MYSQL_STMT *stmt;
   MYSQL_BIND bind[1];
   my_bool is_null[1];
   unsigned long len=0;


   stmt = mysql_stmt_init(&(sdata->mysql));
   if(!stmt){
      if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: %s.mysql_stmt_init() error", sdata->ttmpfile, SQL_METADATA_TABLE);
      goto ENDE;
   }

   snprintf(s, SMALLBUFSIZE-1, "SELECT message_id FROM %s WHERE message_id=?", SQL_METADATA_TABLE);

   if(mysql_stmt_prepare(stmt, s, strlen(s))){
      syslog(LOG_PRIORITY, "%s: %s.mysql_stmt_prepare() error: %s", sdata->ttmpfile, SQL_METADATA_TABLE, mysql_stmt_error(stmt));
      goto ENDE;
   }

   memset(bind, 0, sizeof(bind));

   bind[0].buffer_type = MYSQL_TYPE_STRING;
   bind[0].buffer = state->message_id;
   bind[0].is_null = 0;
   len = strlen(state->message_id); bind[0].length = &len;

   if(mysql_stmt_bind_param(stmt, bind)){
      syslog(LOG_PRIORITY, "%s: %s.mysql_stmt_bind_param() error: %s", sdata->ttmpfile, SQL_METADATA_TABLE, mysql_stmt_error(stmt));
      goto ENDE;
   }


   if(mysql_stmt_execute(stmt)){
      syslog(LOG_PRIORITY, "%s: %s.mysql_stmt_execute() error: %s", sdata->ttmpfile, SQL_METADATA_TABLE, mysql_stmt_error(stmt));
      goto ENDE;
   }


   memset(bind, 0, sizeof(bind));

   bind[0].buffer_type = MYSQL_TYPE_STRING;
   bind[0].buffer = &s[0];
   bind[0].buffer_length = sizeof(s)-1;
   bind[0].is_null = &is_null[0];
   bind[0].length = &len;


   if(mysql_stmt_bind_result(stmt, bind)){
      syslog(LOG_PRIORITY, "%s: %s.mysql_stmt_bind_result() error: %s", sdata->ttmpfile, SQL_METADATA_TABLE, mysql_stmt_error(stmt));
      goto ENDE;
   }


   if(mysql_stmt_store_result(stmt)){
      syslog(LOG_PRIORITY, "%s: %s.mysql_stmt_store_result() error: %s", sdata->ttmpfile, SQL_METADATA_TABLE, mysql_stmt_error(stmt));
      goto ENDE;
   }

   if(!mysql_stmt_fetch(stmt)){
      syslog(LOG_PRIORITY, "%s: found message_id:*%s*(%ld) null=%d", sdata->ttmpfile, s, len, is_null[0]);
      if(is_null[0] == 0) rc = 1;
   }

   mysql_stmt_close(stmt);

ENDE:

   return rc;
}


int is_body_digest_already_stored(struct session_data *sdata, struct _state *state, struct __config *cfg){
   int rc=0;
   char s[SMALLBUFSIZE];
   MYSQL_RES *res;
   MYSQL_ROW row;

   snprintf(s, SMALLBUFSIZE-1, "SELECT `bodydigest` FROM `%s` WHERE `bodydigest`='%s'", SQL_METADATA_TABLE, sdata->bodydigest);

   //if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: check for body digest sql: *%s*", sdata->ttmpfile, s);

   if(mysql_real_query(&(sdata->mysql), s, strlen(s)) == 0){
      res = mysql_store_result(&(sdata->mysql));
      if(res != NULL){
         row = mysql_fetch_row(res);
         if(row) rc = 1;
         mysql_free_result(res);
      }
   }

   return rc;
}


int hand_to_sphinx(struct session_data *sdata, struct _state *state, struct __config *cfg){
   int rc;
   char s[BIGBUFSIZE+2*MAXBUFSIZE];

   snprintf(s, sizeof(s)-1, "INSERT INTO %s (`from`, `to`, `subject`, `body`, `arrived`, `sent`, `size`, `piler_id`) values('%s','%s','%s','%s',%ld,%ld,%d,'%s')", SQL_SPHINX_TABLE, state->b_from, state->b_to, state->b_subject, state->b_body, sdata->now, sdata->sent, sdata->tot_len, sdata->ttmpfile);

   rc = mysql_real_query(&(sdata->mysql), s, strlen(s));

   if(rc == 0) return OK;

   syslog(LOG_PRIORITY, "%s: sphinx sql error: *%s*", sdata->ttmpfile, mysql_error(&(sdata->mysql)));

   return ERR;
}


int store_meta_data(struct session_data *sdata, struct _state *state, struct __config *cfg){
   int i=0, rc, ret=ERR;
   char *p, s[MAXBUFSIZE], s2[SMALLBUFSIZE];
   struct list *list = NULL;

   MYSQL_STMT *stmt;
   MYSQL_BIND bind[4];
   unsigned long len[4];

   stmt = mysql_stmt_init(&(sdata->mysql));
   if(!stmt){
      if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: %s.mysql_stmt_init() error", sdata->ttmpfile, SQL_METADATA_TABLE);
      goto ENDE_META;
   }

   snprintf(s, MAXBUFSIZE-1, "INSERT INTO %s (`from`,`to`,`subject`,`arrived`,`sent`,`size`,`hlen`,`attachments`,`piler_id`,`message_id`,`digest`,`bodydigest`) VALUES(?,?,?,%ld,%ld,%d,%d,%d,'%s',?,'%s','%s')", SQL_METADATA_TABLE, sdata->now, sdata->sent, sdata->tot_len, sdata->hdr_len, state->n_attachments, sdata->ttmpfile, sdata->digest, sdata->bodydigest);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: meta sql: *%s*", sdata->ttmpfile, s);

   if(mysql_stmt_prepare(stmt, s, strlen(s))){
      syslog(LOG_PRIORITY, "%s: %s.mysql_stmt_prepare() error: %s", sdata->ttmpfile, SQL_METADATA_TABLE, mysql_stmt_error(stmt));
      goto ENDE_META;
   }



   if(strlen(state->b_to) < 5){
      snprintf(s2, sizeof(s2)-1, "undisclosed-recipients");
      p = NULL;
      goto LABEL1;
   }
   else p = state->b_to;

   do {
      p = split_str(p, " ", s2, sizeof(s2)-1);
      if(strlen(s2) > 5){
LABEL1:

         if(is_string_on_list(list, s2) == 1) continue;

         append_list(&list, s2);
         i++;


         memset(bind, 0, sizeof(bind));

         bind[0].buffer_type = MYSQL_TYPE_STRING;
         bind[0].buffer = state->b_from;
         bind[0].is_null = 0;
         len[0] = strlen(state->b_from); bind[0].length = &len[0];

         bind[1].buffer_type = MYSQL_TYPE_STRING;
         bind[1].buffer = s2;
         bind[1].is_null = 0;
         len[1] = strlen(s2); bind[1].length = &len[1];

         bind[2].buffer_type = MYSQL_TYPE_STRING;
         bind[2].buffer = state->b_subject;
         bind[2].is_null = 0;
         len[2] = strlen(state->b_subject); bind[2].length = &len[2];

         bind[3].buffer_type = MYSQL_TYPE_STRING;
         bind[3].buffer = state->message_id;
         bind[3].is_null = 0;
         len[3] = strlen(state->message_id); bind[3].length = &len[3];

         if(mysql_stmt_bind_param(stmt, bind)){
            syslog(LOG_PRIORITY, "%s: %s.mysql_stmt_bind_param() error: %s", sdata->ttmpfile, SQL_METADATA_TABLE, mysql_stmt_error(stmt));
            goto ENDE_META;
         }


         rc = mysql_stmt_execute(stmt);

         if(rc){
            syslog(LOG_PRIORITY, "%s: meta sql error: *%s*", sdata->ttmpfile, mysql_error(&(sdata->mysql)));

            ret = ERR_EXISTS;
            goto ENDE_META;
         }

      }

   } while(p);


   if(i == 0) ret = ERR_EXISTS;
   else ret = OK;


ENDE_META:
   free_list(list);

   return ret;
}


int processMessage(struct session_data *sdata, struct _state *state, struct __config *cfg){
   int i, rc;

   /* discard if existing message_id */

   if(is_existing_message_id(sdata, state, cfg) == 1){
      return ERR_EXISTS;
   }


   /* check for existing body digest */

   rc = is_body_digest_already_stored(sdata, state, cfg);

   /*
    * TODO: check if the bodydigest were stored, then we should
    *       only store the header and append a 'bodypointer'
    */



   rc = store_attachments(sdata, state, cfg);

   for(i=1; i<=state->n_attachments; i++){
      unlink(state->attachments[i].internalname);
   }

   if(rc) return ERR;

   rc = store_file(sdata, sdata->tmpframe, 0, 0, cfg);
   if(rc == 0){
      syslog(LOG_PRIORITY, "%s: error storing message: %s", sdata->ttmpfile, sdata->tmpframe);
      return ERR;
   }


   rc = store_meta_data(sdata, state, cfg);
   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: stored metadata, rc=%d",  sdata->ttmpfile, rc);
   if(rc == ERR_EXISTS) return ERR_EXISTS;

   rc = hand_to_sphinx(sdata, state, cfg);
   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: stored indexdata, rc=%d", sdata->ttmpfile, rc);

   return OK;
}

