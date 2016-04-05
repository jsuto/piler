/*
 * import_gui.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>
#include <getopt.h>
#include <syslog.h>
#include <piler.h>


int read_gui_import_data(struct session_data *sdata, struct __data *data, char *folder_imap, char *skiplist, int dryrun, struct __config *cfg){
   int rc=ERR;
   char s_type[SMALLBUFSIZE], s_username[SMALLBUFSIZE], s_password[SMALLBUFSIZE], s_server[SMALLBUFSIZE];

   memset(s_type, 0, sizeof(s_type));
   memset(s_username, 0, sizeof(s_username));
   memset(s_password, 0, sizeof(s_password));
   memset(s_server, 0, sizeof(s_server));

   if(prepare_sql_statement(sdata, &(data->stmt_generic), SQL_PREPARED_STMT_GET_GUI_IMPORT_JOBS) == ERR) return ERR;

   p_bind_init(data);

   if(p_exec_query(sdata, data->stmt_generic, data) == OK){

      p_bind_init(data);

      data->sql[data->pos] = (char *)&(data->import->import_job_id); data->type[data->pos] = TYPE_LONG; data->len[data->pos] = sizeof(int); data->pos++;
      data->sql[data->pos] = &s_type[0]; data->type[data->pos] = TYPE_STRING; data->len[data->pos] = sizeof(s_type)-2; data->pos++;
      data->sql[data->pos] = &s_username[0]; data->type[data->pos] = TYPE_STRING; data->len[data->pos] = sizeof(s_username)-2; data->pos++;
      data->sql[data->pos] = &s_password[0]; data->type[data->pos] = TYPE_STRING; data->len[data->pos] = sizeof(s_password)-2; data->pos++;
      data->sql[data->pos] = &s_server[0]; data->type[data->pos] = TYPE_STRING; data->len[data->pos] = sizeof(s_server)-2; data->pos++;

      p_store_results(data->stmt_generic, data);

      if(p_fetch_results(data->stmt_generic) == OK) rc = OK;

      p_free_results(data->stmt_generic);
   }

   close_prepared_statement(data->stmt_generic);

   data->import->processed_messages = 0;
   data->import->total_messages = 0;

   time(&(data->import->started));
   data->import->status = 1;
   update_import_job_stat(sdata, data);  

   if(strcmp(s_type, "pop3") == 0){
      rc = import_from_pop3_server(s_server, s_username, s_password, 110, sdata, data, dryrun, cfg);
   }

   if(strcmp(s_type, "imap") == 0){
      rc = import_from_imap_server(s_server, s_username, s_password, 143, sdata, data, folder_imap, skiplist, dryrun, cfg);
   }

   update_import_job_stat(sdata, data);

   // don't set error in case of a problem, because it
   // will scare users looking at the gui progressbar

   return rc;
}


