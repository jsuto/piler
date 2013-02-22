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


#define SQL_STMT_SELECT_PURGE_FROM_OPTION_TABLE "SELECT `value` FROM `" SQL_OPTION_TABLE "` WHERE `key`='enable_purge'"
#define SQL_STMT_DELETE_FROM_META_TABLE "UPDATE `" SQL_METADATA_TABLE "` SET `deleted`=1 WHERE `id` IN ("
#define SQL_STMT_DELETE_FROM_META_TABLE_BY_PILER_ID "UPDATE `" SQL_METADATA_TABLE "` SET `deleted`=1 WHERE `piler_id` IN ('"
#define SQL_STMT_SELECT_NON_REFERENCED_ATTACHMENTS "SELECT `piler_id`, `attachment_id`, `i` FROM `" SQL_ATTACHMENTS_VIEW "` WHERE `refcount`=0 AND `piler_id` IN ('"
#define SQL_STMT_DELETE_FROM_ATTACHMENT_TABLE "DELETE FROM `" SQL_ATTACHMENT_TABLE "` WHERE `id` IN ("


int is_purge_allowed(struct session_data *sdata, struct __config *cfg){
   int rc=0;
   MYSQL_RES *res;
   MYSQL_ROW row;

   if(mysql_real_query(&(sdata->mysql), SQL_STMT_SELECT_PURGE_FROM_OPTION_TABLE, strlen(SQL_STMT_SELECT_PURGE_FROM_OPTION_TABLE)) == 0){
      res = mysql_store_result(&(sdata->mysql));
      if(res){
         row = mysql_fetch_row(res);
         if(row[0]){
            rc = atoi(row[0]);
         }

         mysql_free_result(res);
      }
   }

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
      mysql_real_query(&(sdata->mysql), update_meta_sql, strlen(update_meta_sql));
   }


   return n;
}


int remove_attachments(char *in, struct session_data *sdata, struct __config *cfg){
   char filename[SMALLBUFSIZE];
   char *a, buf[BIGBUFSIZE-300], update_meta_sql[BIGBUFSIZE], delete_attachment_stmt[BIGBUFSIZE];
   int n=0, len;
   MYSQL_RES *res;
   MYSQL_ROW row;
   struct stat st;


   if(strlen(in) < 10) return 0;

   len = strlen(SQL_STMT_SELECT_NON_REFERENCED_ATTACHMENTS) + strlen(in) + 2;

   a = malloc(len);
   if(!a) return 0;

   memset(a, 0, len);

   in[strlen(in)-2] = ')';
   in[strlen(in)-1] = '\0';
   snprintf(a, len-1, "%s%s", SQL_STMT_SELECT_NON_REFERENCED_ATTACHMENTS, in);

   if(dryrun == 1) printf("running sql query: *%s*\n\n", a);


   memset(buf, 0, sizeof(buf));
   memset(update_meta_sql, 0, sizeof(update_meta_sql));
   memset(delete_attachment_stmt, 0, sizeof(delete_attachment_stmt));

   snprintf(update_meta_sql, sizeof(update_meta_sql)-1, "%s", SQL_STMT_DELETE_FROM_META_TABLE_BY_PILER_ID);
   snprintf(delete_attachment_stmt, sizeof(delete_attachment_stmt)-1, "%s", SQL_STMT_DELETE_FROM_ATTACHMENT_TABLE);

   if(mysql_real_query(&(sdata->mysql), a, strlen(a)) == 0){
      res = mysql_store_result(&(sdata->mysql));
      if(res){
         while((row = mysql_fetch_row(res))){
            if(!row[0]) continue;

            snprintf(filename, sizeof(filename)-1, "%s/%02x/%c%c%c/%c%c/%c%c/%s.a%d", cfg->queuedir, cfg->server_id, row[0][8], row[0][9], row[0][10], row[0][RND_STR_LEN-4], row[0][RND_STR_LEN-3], row[0][RND_STR_LEN-2], row[0][RND_STR_LEN-1], row[0], atoi(row[1]));
            if(stat(filename, &st)){
               snprintf(filename, sizeof(filename)-1, "%s/%02x/%c%c/%c%c/%c%c/%s.a%d", cfg->queuedir, cfg->server_id, row[0][RND_STR_LEN-6], row[0][RND_STR_LEN-5], row[0][RND_STR_LEN-4], row[0][RND_STR_LEN-3], row[0][RND_STR_LEN-2], row[0][RND_STR_LEN-1], row[0], atoi(row[1]));
            }

            if(dryrun == 1){
               printf("removing attachment: *%s*\n", filename);
            } else {
               unlink(filename);
            }


            if(row[2]){
               memcpy(&delete_attachment_stmt[strlen(delete_attachment_stmt)], row[2], strlen(row[2]));
               memcpy(&delete_attachment_stmt[strlen(delete_attachment_stmt)], ",", 1);
            }

            if(atoi(row[1]) == 1){

               memcpy(&update_meta_sql[strlen(update_meta_sql)], row[0], strlen(row[0]));
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
                        mysql_real_query(&(sdata->mysql), delete_attachment_stmt, strlen(delete_attachment_stmt));
                     }
                  }

                  memset(buf, 0, sizeof(buf));
                  memset(update_meta_sql, 0, sizeof(update_meta_sql));
                  memset(delete_attachment_stmt, 0, sizeof(delete_attachment_stmt));

                  snprintf(update_meta_sql, sizeof(update_meta_sql)-1, "%s", SQL_STMT_DELETE_FROM_META_TABLE_BY_PILER_ID);
                  snprintf(delete_attachment_stmt, sizeof(delete_attachment_stmt)-1, "%s", SQL_STMT_DELETE_FROM_ATTACHMENT_TABLE);
               }

               memcpy(&buf[strlen(buf)], row[0], strlen(row[0]));
               memcpy(&buf[strlen(buf)], " ", 1);

            }
         }

         mysql_free_result(res);
      }
   }

   free(a);


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
         mysql_real_query(&(sdata->mysql), delete_attachment_stmt, strlen(delete_attachment_stmt));
      }

   }


   return n;
}


int purge_messages_without_attachment(struct session_data *sdata, struct __config *cfg){
   int purged=0;
   char s[SMALLBUFSIZE], buf[BIGBUFSIZE-300], update_meta_sql[BIGBUFSIZE];
   MYSQL_RES *res;
   MYSQL_ROW row;

   memset(buf, 0, sizeof(buf));
   memset(update_meta_sql, 0, sizeof(update_meta_sql));

   snprintf(update_meta_sql, sizeof(update_meta_sql)-1, "%s", SQL_STMT_DELETE_FROM_META_TABLE);

   snprintf(s, sizeof(s)-1, "SELECT `id`, `piler_id` FROM `%s` WHERE `deleted`=0 AND `retained` < %ld AND attachments=0", SQL_METADATA_TABLE, sdata->now);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "purge sql: *%s*", s);

   if(mysql_real_query(&(sdata->mysql), s, strlen(s)) == 0){
      res = mysql_store_result(&(sdata->mysql));
      if(res){
         while((row = mysql_fetch_row(res))){

            if((char *)row[0] && (char *)row[1]){

               memcpy(&update_meta_sql[strlen(update_meta_sql)], row[0], strlen(row[0]));
               memcpy(&update_meta_sql[strlen(update_meta_sql)], ",", 1);

               if(strlen(buf) >= sizeof(buf)-RND_STR_LEN-2-1){

                  purged += remove_message_frame_files(buf, update_meta_sql, sdata, cfg);

                  memset(buf, 0, sizeof(buf));
                  memset(update_meta_sql, 0, sizeof(update_meta_sql));

                  snprintf(update_meta_sql, sizeof(update_meta_sql)-1, "%s", SQL_STMT_DELETE_FROM_META_TABLE);
               }

               memcpy(&buf[strlen(buf)], row[1], strlen(row[1]));
               memcpy(&buf[strlen(buf)], " ", 1);

            }

         }

         mysql_free_result(res);
      }
   }

   if(strlen(buf) > 5 && strlen(update_meta_sql) > strlen(SQL_STMT_DELETE_FROM_META_TABLE)+5){
      purged += remove_message_frame_files(buf, update_meta_sql, sdata, cfg);
   }

   return purged;
}


int purge_messages_with_attachments(struct session_data *sdata, struct __config *cfg){
   int purged=0;
   char s[SMALLBUFSIZE], idlist[BIGBUFSIZE];
   MYSQL_RES *res;
   MYSQL_ROW row;

   memset(idlist, 0, sizeof(idlist));

   snprintf(s, sizeof(s)-1, "SELECT `piler_id` FROM `%s` WHERE `deleted`=0 AND `retained` < %ld AND attachments > 0", SQL_METADATA_TABLE, sdata->now);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "purge sql: *%s*", s);

   if(mysql_real_query(&(sdata->mysql), s, strlen(s)) == 0){
      res = mysql_store_result(&(sdata->mysql));
      if(res){
         while((row = mysql_fetch_row(res))){
            if((char *)row[0]){
               memcpy(&idlist[strlen(idlist)], row[0], strlen(row[0]));
               memcpy(&idlist[strlen(idlist)], "','", 3);
            }

            if(strlen(idlist) >= sizeof(idlist)-2*RND_STR_LEN){
               purged += remove_attachments(idlist, sdata, cfg);
               memset(idlist, 0, sizeof(idlist));
            }
         }

         mysql_free_result(res);
      }
   }

   if(strlen(idlist) > 5){
      purged += remove_attachments(idlist, sdata, cfg);
   }

   return purged;
}


int main(int argc, char **argv){
   int i, purged=0;
   char *configfile=CONFIG_FILE;
   struct session_data sdata;
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


   mysql_init(&(sdata.mysql));
   mysql_options(&(sdata.mysql), MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&cfg.mysql_connect_timeout);
   if(mysql_real_connect(&(sdata.mysql), cfg.mysqlhost, cfg.mysqluser, cfg.mysqlpwd, cfg.mysqldb, cfg.mysqlport, cfg.mysqlsocket, 0) == 0){
      printf("cannot connect to mysql server\n");
      return 0;
   }

   mysql_real_query(&(sdata.mysql), "SET NAMES utf8", strlen("SET NAMES utf8"));
   mysql_real_query(&(sdata.mysql), "SET CHARACTER SET utf8", strlen("SET CHARACTER SET utf8"));

   setlocale(LC_CTYPE, cfg.locale);

   init_session_data(&sdata, &cfg);

   i = is_purge_allowed(&sdata, &cfg);
   if(i == 1){
      purged += purge_messages_without_attachment(&sdata, &cfg);
      purged += purge_messages_with_attachments(&sdata, &cfg);

      printf("purged: %d\n", purged);
   }
   else printf("purge is not allowed by configuration, enable_purge=%d\n", i);

   mysql_close(&(sdata.mysql));

   return 0;
}


