/*
 * pilerpurge.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>
#include <syslog.h>
#include <piler.h>


extern char *optarg;
extern int optind;

int dryrun = 0;
unsigned long purged_size=0;


#define SQL_STMT_SELECT_PURGE_FROM_OPTION_TABLE "SELECT `value` FROM `" SQL_OPTION_TABLE "` WHERE `key`='enable_purge'"
#define SQL_STMT_DELETE_FROM_META_TABLE "UPDATE `" SQL_METADATA_TABLE "` SET `deleted`=1 WHERE `id` IN ("
#define SQL_STMT_DELETE_FROM_META_TABLE_BY_PILER_ID "UPDATE `" SQL_METADATA_TABLE "` SET `deleted`=1 WHERE `piler_id` IN ('"
#define SQL_STMT_SELECT_NON_REFERENCED_ATTACHMENTS "SELECT `piler_id`, `attachment_id`, `i` FROM `" SQL_ATTACHMENTS_VIEW "` WHERE `refcount`=0 AND `piler_id` IN ('"
#define SQL_STMT_DELETE_FROM_ATTACHMENT_TABLE "DELETE FROM `" SQL_ATTACHMENT_TABLE "` WHERE `id` IN ("


int is_purge_allowed(struct session_data *sdata, struct __data *data, struct __config *cfg){
   int rc=0;

   if(prepare_sql_statement(sdata, &(data->stmt_generic), SQL_STMT_SELECT_PURGE_FROM_OPTION_TABLE) == ERR) return rc;


   p_bind_init(data);

   if(p_exec_query(sdata, data->stmt_generic, data) == ERR) goto ENDE;



   p_bind_init(data);

   data->sql[data->pos] = (char *)&rc; data->type[data->pos] = TYPE_LONG; data->len[data->pos] = sizeof(int); data->pos++;

   p_store_results(sdata, data->stmt_generic, data);
   p_fetch_results(data->stmt_generic);
   p_free_results(data->stmt_generic);

ENDE:
   close_prepared_statement(data->stmt_generic);

   return rc;
}


int remove_message_frame_files(char *s, char *update_meta_sql, struct session_data *sdata, struct __config *cfg){
   char *p, puf[SMALLBUFSIZE], filename[SMALLBUFSIZE];
   int n=0;
   struct stat st;

   p = s;
   do {
      p = split(p, ' ', puf, sizeof(puf)-1);

      if(strlen(puf) == RND_STR_LEN){
         snprintf(filename, sizeof(filename)-1, "%s/%02x/%c%c%c/%c%c/%c%c/%s.m", cfg->queuedir, cfg->server_id, puf[8], puf[9], puf[10], puf[RND_STR_LEN-4], puf[RND_STR_LEN-3], puf[RND_STR_LEN-2], puf[RND_STR_LEN-1], puf);

         if(stat(filename, &st)){
            snprintf(filename, sizeof(filename)-1, "%s/%02x/%c%c/%c%c/%c%c/%s.m", cfg->queuedir, cfg->server_id, puf[RND_STR_LEN-6], puf[RND_STR_LEN-5], puf[RND_STR_LEN-4], puf[RND_STR_LEN-3], puf[RND_STR_LEN-2], puf[RND_STR_LEN-1], puf);
         }

         if(dryrun == 1){
            n++;
            printf("removing messagefile: %s\n", filename);
         }
         else {
            if(unlink(filename) == 0) n++;
         }

      }
   } while(p);


   update_meta_sql[strlen(update_meta_sql)-1] = ')';

   if(dryrun == 1){
      printf("running sql query: *%s*\n\n", update_meta_sql);
   } else {
      p_query(sdata, update_meta_sql);
   }


   return n;
}


int remove_attachments(char *in, struct session_data *sdata, struct __data *data, struct __config *cfg){
   char filename[SMALLBUFSIZE];
   char *a, buf[BIGBUFSIZE-300], update_meta_sql[BIGBUFSIZE], delete_attachment_stmt[BIGBUFSIZE];
   char piler_id[SMALLBUFSIZE], i[BUFLEN];
   int n=0, len, attachment_id;
   struct stat st;


   if(strlen(in) < 10) return 0;

   len = strlen(SQL_STMT_SELECT_NON_REFERENCED_ATTACHMENTS) + strlen(in) + 2;

   a = malloc(len);
   if(!a) return 0;

   memset(a, 0, len);

   in[strlen(in)-2] = ')';
   in[strlen(in)-1] = '\0';
   snprintf(a, len-1, "%s%s", SQL_STMT_SELECT_NON_REFERENCED_ATTACHMENTS, in);

   if(prepare_sql_statement(sdata, &(data->stmt_select_non_referenced_attachments), a) == ERR){ free(a); return n; }

   if(dryrun == 1) printf("running sql query: *%s*\n\n", a);


   memset(buf, 0, sizeof(buf));
   memset(update_meta_sql, 0, sizeof(update_meta_sql));
   memset(delete_attachment_stmt, 0, sizeof(delete_attachment_stmt));

   snprintf(update_meta_sql, sizeof(update_meta_sql)-1, "%s", SQL_STMT_DELETE_FROM_META_TABLE_BY_PILER_ID);
   snprintf(delete_attachment_stmt, sizeof(delete_attachment_stmt)-1, "%s", SQL_STMT_DELETE_FROM_ATTACHMENT_TABLE);


   p_bind_init(data);
   if(p_exec_query(sdata, data->stmt_select_non_referenced_attachments, data) == ERR) goto ENDE;


   p_bind_init(data);

   data->sql[data->pos] = &piler_id[0]; data->type[data->pos] = TYPE_STRING; data->len[data->pos] = sizeof(piler_id)-2; data->pos++;
   data->sql[data->pos] = (char *)&attachment_id; data->type[data->pos] = TYPE_LONG; data->len[data->pos] = sizeof(int); data->pos++;
   data->sql[data->pos] = &i[0]; data->type[data->pos] = TYPE_STRING; data->len[data->pos] = sizeof(i)-2; data->pos++;

   p_store_results(sdata, data->stmt_select_non_referenced_attachments, data);

   while(p_fetch_results(data->stmt_select_non_referenced_attachments) == OK){

      snprintf(filename, sizeof(filename)-1, "%s/%02x/%c%c%c/%c%c/%c%c/%s.a%d", cfg->queuedir, cfg->server_id, piler_id[8], piler_id[9], piler_id[10], piler_id[RND_STR_LEN-4], piler_id[RND_STR_LEN-3], piler_id[RND_STR_LEN-2], piler_id[RND_STR_LEN-1], piler_id, attachment_id);
      if(stat(filename, &st)){
         snprintf(filename, sizeof(filename)-1, "%s/%02x/%c%c/%c%c/%c%c/%s.a%d", cfg->queuedir, cfg->server_id, piler_id[RND_STR_LEN-6], piler_id[RND_STR_LEN-5], piler_id[RND_STR_LEN-4], piler_id[RND_STR_LEN-3], piler_id[RND_STR_LEN-2], piler_id[RND_STR_LEN-1], piler_id, attachment_id);
      }

      if(dryrun == 1){
         printf("removing attachment: *%s*\n", filename);
      } else {
         unlink(filename);
      }


      if(strlen(i) > 0){
         memcpy(&delete_attachment_stmt[strlen(delete_attachment_stmt)], i, strlen(i));
         memcpy(&delete_attachment_stmt[strlen(delete_attachment_stmt)], ",", 1);
      }

      if(attachment_id == 1){

         memcpy(&update_meta_sql[strlen(update_meta_sql)], piler_id, strlen(piler_id));
         memcpy(&update_meta_sql[strlen(update_meta_sql)], "','", 3);

         if(strlen(buf) >= sizeof(buf)-RND_STR_LEN-2-1){
            if(strlen(update_meta_sql) > 10){
               update_meta_sql[strlen(update_meta_sql)-2] = ')';
               update_meta_sql[strlen(update_meta_sql)-1] = '\0';
            }

            n += remove_message_frame_files(buf, update_meta_sql, sdata, cfg);

            if(strlen(delete_attachment_stmt) > strlen(SQL_STMT_DELETE_FROM_ATTACHMENT_TABLE)){
               delete_attachment_stmt[strlen(delete_attachment_stmt)-1] = ')';
               if(dryrun == 1){
                  printf("delete sql: *%s*\n", delete_attachment_stmt);
               } else {
                  p_query(sdata, delete_attachment_stmt);
               }
            }

            memset(buf, 0, sizeof(buf));
            memset(update_meta_sql, 0, sizeof(update_meta_sql));
            memset(delete_attachment_stmt, 0, sizeof(delete_attachment_stmt));

            snprintf(update_meta_sql, sizeof(update_meta_sql)-1, "%s", SQL_STMT_DELETE_FROM_META_TABLE_BY_PILER_ID);
            snprintf(delete_attachment_stmt, sizeof(delete_attachment_stmt)-1, "%s", SQL_STMT_DELETE_FROM_ATTACHMENT_TABLE);
         }

         memcpy(&buf[strlen(buf)], piler_id, strlen(piler_id));
         memcpy(&buf[strlen(buf)], " ", 1);
      }

   }

   p_free_results(data->stmt_select_non_referenced_attachments);


   if(strlen(buf) > 5 && strlen(update_meta_sql) > strlen(SQL_STMT_DELETE_FROM_META_TABLE_BY_PILER_ID)+10){
      update_meta_sql[strlen(update_meta_sql)-2] = ')';
      update_meta_sql[strlen(update_meta_sql)-1] = '\0';

      n += remove_message_frame_files(buf, update_meta_sql, sdata, cfg);
   }

   if(strlen(delete_attachment_stmt) > strlen(SQL_STMT_DELETE_FROM_ATTACHMENT_TABLE)){
      delete_attachment_stmt[strlen(delete_attachment_stmt)-1] = ')';
      if(dryrun == 1){
         printf("delete sql: *%s*\n", delete_attachment_stmt);
      } else {
         p_query(sdata, delete_attachment_stmt);
      }

   }

ENDE:
   free(a);

   close_prepared_statement(data->stmt_select_non_referenced_attachments);

   return n;
}


int purge_messages_without_attachment(struct session_data *sdata, struct __data *data, struct __config *cfg){
   int purged=0, size;
   char id[BUFLEN], s[SMALLBUFSIZE], buf[BIGBUFSIZE-300], update_meta_sql[BIGBUFSIZE];

   memset(buf, 0, sizeof(buf));
   memset(update_meta_sql, 0, sizeof(update_meta_sql));

   snprintf(update_meta_sql, sizeof(update_meta_sql)-1, "%s", SQL_STMT_DELETE_FROM_META_TABLE);

   snprintf(s, sizeof(s)-1, "SELECT `id`, `piler_id`, `size` FROM `%s` WHERE `deleted`=0 AND `retained` < %ld AND attachments=0", SQL_METADATA_TABLE, sdata->now);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "purge sql: *%s*", s);

   if(prepare_sql_statement(sdata, &(data->stmt_select_from_meta_table), s) == ERR) return purged;

   p_bind_init(data);

   if(p_exec_query(sdata, data->stmt_select_from_meta_table, data) == ERR) goto ENDE;


   p_bind_init(data);

   data->sql[data->pos] = &id[0]; data->type[data->pos] = TYPE_STRING; data->len[data->pos] = sizeof(id)-2; data->pos++;
   data->sql[data->pos] = &s[0]; data->type[data->pos] = TYPE_STRING; data->len[data->pos] = sizeof(s)-2; data->pos++;
   data->sql[data->pos] = (char *)&size; data->type[data->pos] = TYPE_LONG; data->len[data->pos] = sizeof(int); data->pos++;

   p_store_results(sdata, data->stmt_select_from_meta_table, data);

   while(p_fetch_results(data->stmt_select_from_meta_table) == OK){

      memcpy(&update_meta_sql[strlen(update_meta_sql)], id, strlen(id));
      memcpy(&update_meta_sql[strlen(update_meta_sql)], ",", 1);

      purged_size += size;

      if(strlen(buf) >= sizeof(buf)-RND_STR_LEN-2-1){

         purged += remove_message_frame_files(buf, update_meta_sql, sdata, cfg);

         memset(buf, 0, sizeof(buf));
         memset(update_meta_sql, 0, sizeof(update_meta_sql));

         snprintf(update_meta_sql, sizeof(update_meta_sql)-1, "%s", SQL_STMT_DELETE_FROM_META_TABLE);
      }

      memcpy(&buf[strlen(buf)], s, strlen(s));
      memcpy(&buf[strlen(buf)], " ", 1);

   }

   p_free_results(data->stmt_select_from_meta_table);

   if(strlen(buf) > 5 && strlen(update_meta_sql) > strlen(SQL_STMT_DELETE_FROM_META_TABLE)+5){
      purged += remove_message_frame_files(buf, update_meta_sql, sdata, cfg);
   }

ENDE:
   close_prepared_statement(data->stmt_select_from_meta_table);

   return purged;
}


int purge_messages_with_attachments(struct session_data *sdata, struct __data *data, struct __config *cfg){
   int purged=0, size;
   char s[SMALLBUFSIZE], idlist[BIGBUFSIZE];

   memset(idlist, 0, sizeof(idlist));

   snprintf(s, sizeof(s)-1, "SELECT `piler_id`, `size` FROM `%s` WHERE `deleted`=0 AND `retained` < %ld AND attachments > 0", SQL_METADATA_TABLE, sdata->now);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "purge sql: *%s*", s);

   if(prepare_sql_statement(sdata, &(data->stmt_select_from_meta_table), s) == ERR) return purged;

   p_bind_init(data);
   if(p_exec_query(sdata, data->stmt_select_from_meta_table, data) == ERR) goto ENDE;


   p_bind_init(data);

   data->sql[data->pos] = &s[0]; data->type[data->pos] = TYPE_STRING; data->len[data->pos] = sizeof(s)-2; data->pos++;
   data->sql[data->pos] = (char *)&size; data->type[data->pos] = TYPE_LONG; data->len[data->pos] = sizeof(int); data->pos++;

   p_store_results(sdata, data->stmt_select_from_meta_table, data);

   while(p_fetch_results(data->stmt_select_from_meta_table) == OK){
      memcpy(&idlist[strlen(idlist)], s, strlen(s));
      memcpy(&idlist[strlen(idlist)], "','", 3);

      purged_size += size;

      if(strlen(idlist) >= sizeof(idlist)-2*RND_STR_LEN){
         purged += remove_attachments(idlist, sdata, data, cfg);

         memset(idlist, 0, sizeof(idlist));
      }
   }

   p_free_results(data->stmt_select_from_meta_table);


   if(strlen(idlist) > 5){
      purged += remove_attachments(idlist, sdata, data, cfg);
   }

ENDE:
   close_prepared_statement(data->stmt_select_from_meta_table);

   return purged;
}


int main(int argc, char **argv){
   int i, purged=0;
   char *configfile=CONFIG_FILE, buf[SMALLBUFSIZE];
   struct session_data sdata;
   struct __data data;
   struct __config cfg;


   while((i = getopt(argc, argv, "c:dh?")) > 0){
       switch(i){

         case 'c' :
                    configfile = optarg;
                    break;

         case 'd' :
                    dryrun = 1;
                    break;

         case 'h' :
         case '?' :
         default  : 
                    break;
       }
   }



   (void) openlog("pilerpurge", LOG_PID, LOG_MAIL);

   cfg = read_config(configfile);


   if(open_database(&sdata, &cfg) == ERR) return 0;


   setlocale(LC_CTYPE, cfg.locale);

   init_session_data(&sdata, &cfg);

   i = is_purge_allowed(&sdata, &data, &cfg);
   if(i == 1){
      purged += purge_messages_without_attachment(&sdata, &data, &cfg);
      purged += purge_messages_with_attachments(&sdata, &data, &cfg);

      syslog(LOG_INFO, "purged %d messages, %ld bytes", purged, purged_size);
   }
   else printf("purge is not allowed by configuration, enable_purge=%d\n", i);

   
   if(purged_size > 100){
      snprintf(buf, sizeof(buf)-1, "UPDATE `%s` SET size = size - %ld", SQL_COUNTER_TABLE, purged_size);

      if(dry_run == 0) p_query(&sdata, buf);
   }


   close_database(&sdata);

   return 0;
}


