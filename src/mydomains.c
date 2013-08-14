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
   int rc;
   char s[SMALLBUFSIZE];

   memset(s, 0, sizeof(s));


   if(prepare_sql_statement(sdata, &(data->stmt_generic), SQL_PREPARED_STMT_GET_DOMAINS) == ERR) return;


   p_bind_init(data);

   if(p_exec_query(sdata, data->stmt_generic, data) == ERR) goto ENDE;



   p_bind_init(data);

   data->sql[data->pos] = &s[0]; data->type[data->pos] = TYPE_STRING; data->len[data->pos] = sizeof(s)-2; data->pos++;

   p_store_results(sdata, data->stmt_generic, data);

   while(p_fetch_results(data->stmt_generic) == OK){
      rc = addnode(data->mydomains, s);

      if(rc == 0) syslog(LOG_PRIORITY, "failed to append mydomain: '%s'", s);
      else if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "added mydomain: '%s'", s);

      memset(s, 0, sizeof(s));
   }

   p_free_results(data->stmt_generic);

ENDE:
   close_prepared_statement(data->stmt_generic);
}


int is_email_address_on_my_domains(char *email, struct __data *data){
   int rc=0;
   char *q, *s;

   if(email == NULL) return rc;

   q = strchr(email, '@');
   if(!q || strlen(q) < 3) return rc;

   s = strrchr(q+1, ' ');

   if(s) *s = '\0';

   if(findnode(data->mydomains, q+1)) rc = 1;

   if(s) *s = ' ';

   return rc;
}


