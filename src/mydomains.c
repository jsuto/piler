/*
 * mydomains.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <piler.h>


void load_mydomains(struct session_data *sdata, struct __data *data, struct __config *cfg){
   int clen=0, len=0, size=sizeof(data->mydomains);
   char s[SMALLBUFSIZE];

   memset(data->mydomains, 0, size);
   memset(s, 0, sizeof(s));


   if(prepare_sql_statement(sdata, &(data->stmt_generic), SQL_PREPARED_STMT_GET_DOMAINS) == ERR) return;


   p_bind_init(data);

   if(p_exec_query(sdata, data->stmt_generic, data) == ERR) goto ENDE;



   p_bind_init(data);

   data->sql[data->pos] = &s[0]; data->type[data->pos] = TYPE_STRING; data->len[data->pos] = sizeof(s)-2; data->pos++;

   p_store_results(sdata, data->stmt_generic, data);

   while(p_fetch_results(data->stmt_generic) == OK){
      len = strlen(s);

      if(clen + len + 1 < size){
         memcpy(data->mydomains+clen, s, len);
         clen += len;
      }
      else break;

      memset(s, 0, sizeof(s));
   }

   p_free_results(data->stmt_generic);

ENDE:
   close_prepared_statement(data->stmt_generic);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "mydomains: '%s'", data->mydomains);
}

