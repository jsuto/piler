/*
 * import.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>
#include <syslog.h>
#include <piler.h>


int import_message(char *filename, struct session_data *sdata, struct __data *data, struct __config *cfg){
   int rc=ERR, fd;
   char *rule;
   struct stat st;
   struct _state state;
   struct __counters counters;


   init_session_data(sdata);


   if(strcmp(filename, "-") == 0){

      if(read_from_stdin(sdata) == ERR){
         printf("error reading from stdin\n");
         return rc;
      }

      snprintf(sdata->filename, SMALLBUFSIZE-1, "%s", sdata->ttmpfile);

   }
   else {

      if(stat(filename, &st) != 0){
         printf("cannot stat() %s\n", filename);
         return rc;
      }

      if(S_ISREG(st.st_mode) == 0){
         printf("%s is not a file\n", filename);
         return rc;
      }

      fd = open(filename, O_RDONLY);
      if(fd == -1){
         printf("cannot open %s\n", filename);
         return rc;
      }
      close(fd);

      snprintf(sdata->filename, SMALLBUFSIZE-1, "%s", filename);

      sdata->tot_len = st.st_size;
   }


   
   sdata->sent = 0;

   state = parse_message(sdata, 1, cfg);
   post_parse(sdata, &state, cfg);

   if(sdata->sent > sdata->now) sdata->sent = sdata->now;
   if(sdata->sent == -1) sdata->sent = 0;

   /* fat chances that you won't import emails before 1990.01.01 */

   if(sdata->sent > 631148400) sdata->retained = sdata->sent;

   rule = check_againt_ruleset(data->archiving_rules, &state, sdata->tot_len, sdata->spam_message);

   if(rule){
      printf("discarding %s by archiving policy: %s\n", filename, rule);
      rc = OK;
      goto ENDE;
   }

   make_digests(sdata, cfg);

   rc = process_message(sdata, &state, data, cfg);

ENDE:
   unlink(sdata->tmpframe);

   if(strcmp(filename, "-") == 0) unlink(sdata->ttmpfile);


   switch(rc) {
      case OK:
                        //printf("imported: %s\n", filename);

                        bzero(&counters, sizeof(counters));
                        counters.c_size += sdata->tot_len;
                        update_counters(sdata, data, &counters, cfg);

                        break;

      case ERR_EXISTS:
                        //printf("discarding duplicate message: %s\n", filename);
                        rc = OK;
                        break;

      default:
                        printf("failed to import: %s (id: %s)\n", filename, sdata->ttmpfile);
                        break;
   } 

   return rc;
}


unsigned long get_folder_id(struct session_data *sdata, char *foldername, int parent_id){
   unsigned long id=0;
   char s[SMALLBUFSIZE];
   MYSQL_STMT *stmt;
   MYSQL_BIND bind[2];
   unsigned long len[2];


   snprintf(s, SMALLBUFSIZE-1, "SELECT `id` FROM %s WHERE `name`=? AND `parent_id`=?", SQL_FOLDER_TABLE);

   if(prepare_a_mysql_statement(sdata, &stmt, s) == ERR) goto ENDE;

   memset(bind, 0, sizeof(bind));

   bind[0].buffer_type = MYSQL_TYPE_STRING;
   bind[0].buffer = foldername;
   bind[0].is_null = 0;
   len[0] = strlen(foldername); bind[0].length = &len[0];

   bind[1].buffer_type = MYSQL_TYPE_LONG;
   bind[1].buffer = (char *)&parent_id;
   bind[1].is_null = 0;
   bind[1].length = 0;

   if(mysql_stmt_bind_param(stmt, bind)){
      goto CLOSE;
   }


   if(mysql_stmt_execute(stmt)){
      goto CLOSE;
   }

   memset(bind, 0, sizeof(bind));

   bind[0].buffer_type = MYSQL_TYPE_LONG;
   bind[0].buffer = (char *)&id;
   bind[0].is_null = 0;
   bind[0].length = 0;

   if(mysql_stmt_bind_result(stmt, bind)){
      goto CLOSE;
   }


   if(mysql_stmt_store_result(stmt)){
      goto CLOSE;
   }

   mysql_stmt_fetch(stmt);

CLOSE:
   mysql_stmt_close(stmt);

ENDE:

   return id;
}


unsigned long add_new_folder(struct session_data *sdata, char *foldername, int parent_id){
   unsigned long id=0;
   char s[SMALLBUFSIZE];
   MYSQL_STMT *stmt;
   MYSQL_BIND bind[2];
   unsigned long len[2];


   snprintf(s, sizeof(s)-1, "INSERT INTO %s (`name`, `parent_id`) VALUES(?,?)", SQL_FOLDER_TABLE);

   if(prepare_a_mysql_statement(sdata, &stmt, s) == ERR) goto ENDE;

   memset(bind, 0, sizeof(bind));

   bind[0].buffer_type = MYSQL_TYPE_STRING;
   bind[0].buffer = foldername;
   bind[0].is_null = 0;
   len[0] = strlen(foldername); bind[0].length = &len[0];

   bind[1].buffer_type = MYSQL_TYPE_LONG;
   bind[1].buffer = (char *)&parent_id;
   bind[1].is_null = 0;
   bind[1].length = 0;

   if(mysql_stmt_bind_param(stmt, bind)){
      syslog(LOG_PRIORITY, "%s: %s.mysql_stmt_bind_param() error: %s", sdata->ttmpfile, SQL_FOLDER_TABLE, mysql_stmt_error(stmt));
      goto CLOSE;
   }

   if(mysql_stmt_execute(stmt)){
      syslog(LOG_PRIORITY, "%s: %s.mysql_stmt_execute error: *%s*", sdata->ttmpfile, SQL_RECIPIENT_TABLE, mysql_error(&(sdata->mysql)));
      goto CLOSE;
   }


   id = mysql_stmt_insert_id(stmt);

CLOSE:
   mysql_stmt_close(stmt);

ENDE:

   return id;
}


