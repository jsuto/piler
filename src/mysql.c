/*
 * mysql.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <piler.h>


int open_database(struct session_data *sdata, struct __config *cfg){
   int rc=1;

   mysql_init(&(sdata->mysql));

   mysql_options(&(sdata->mysql), MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&cfg->mysql_connect_timeout);
   mysql_options(&(sdata->mysql), MYSQL_OPT_RECONNECT, (const char*)&rc);

   if(mysql_real_connect(&(sdata->mysql), cfg->mysqlhost, cfg->mysqluser, cfg->mysqlpwd, cfg->mysqldb, cfg->mysqlport, cfg->mysqlsocket, 0) == 0){
      printf("cant connect to mysql server\n");
      return ERR;
   }

   mysql_real_query(&(sdata->mysql), "SET NAMES utf8", strlen("SET NAMES utf8"));
   mysql_real_query(&(sdata->mysql), "SET CHARACTER SET utf8", strlen("SET CHARACTER SET utf8"));

   return OK;
}


void close_database(struct session_data *sdata){
   mysql_close(&(sdata->mysql));
}


int prepare_a_mysql_statement(struct session_data *sdata, MYSQL_STMT **stmt, char *s){

   *stmt = mysql_stmt_init(&(sdata->mysql));
   if(!*stmt){
      syslog(LOG_PRIORITY, "%s: mysql_stmt_init() error", sdata->ttmpfile);
      return ERR;
   }

   if(mysql_stmt_prepare(*stmt, s, strlen(s))){
      syslog(LOG_PRIORITY, "%s: mysql_stmt_prepare() error: %s => sql: %s", sdata->ttmpfile, mysql_stmt_error(*stmt), s);
      return ERR;
   }

   return OK; 
}


void insert_offset(struct session_data *sdata, int server_id){
   char s[SMALLBUFSIZE];
   uint64 id = server_id * 1000000000000ULL;

   snprintf(s, sizeof(s)-1, "INSERT INTO %s (`id`) VALUES (%llu)", SQL_METADATA_TABLE, id);

   mysql_real_query(&(sdata->mysql), s, strlen(s));
}


int create_prepared_statements(struct session_data *sdata, struct __data *data){
   int rc = OK;

   data->stmt_get_meta_id_by_message_id = NULL;
   data->stmt_insert_into_rcpt_table = NULL;
   data->stmt_update_metadata_reference = NULL;

   if(prepare_a_mysql_statement(sdata, &(data->stmt_get_meta_id_by_message_id), SQL_PREPARED_STMT_GET_META_ID_BY_MESSAGE_ID) == ERR) rc = ERR;
   if(prepare_a_mysql_statement(sdata, &(data->stmt_insert_into_rcpt_table), SQL_PREPARED_STMT_INSERT_INTO_RCPT_TABLE) == ERR) rc = ERR;
   if(prepare_a_mysql_statement(sdata, &(data->stmt_update_metadata_reference), SQL_PREPARED_STMT_UPDATE_METADATA_REFERENCE) == ERR) rc = ERR;

   return rc;
}


void close_prepared_statements(struct __data *data){
   if(data->stmt_get_meta_id_by_message_id) mysql_stmt_close(data->stmt_get_meta_id_by_message_id);
   if(data->stmt_insert_into_rcpt_table) mysql_stmt_close(data->stmt_insert_into_rcpt_table);
   if(data->stmt_update_metadata_reference) mysql_stmt_close(data->stmt_update_metadata_reference);
}

