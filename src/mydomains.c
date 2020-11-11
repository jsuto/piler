/*
 * mydomains.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <piler.h>


void load_mydomains(struct session_data *sdata, struct data *data, struct config *cfg){
   char s[SMALLBUFSIZE];
   struct sql sql;

   memset(s, 0, sizeof(s));


   if(prepare_sql_statement(sdata, &sql, SQL_PREPARED_STMT_GET_DOMAINS) == ERR) return;


   p_bind_init(&sql);

   if(p_exec_stmt(sdata, &sql) == OK){

      p_bind_init(&sql);

      sql.sql[sql.pos] = &s[0]; sql.type[sql.pos] = TYPE_STRING; sql.len[sql.pos] = sizeof(s)-2; sql.pos++;

      p_store_results(&sql);

      while(p_fetch_results(&sql) == OK){
         if(addnode(data->mydomains, s) == 0) syslog(LOG_PRIORITY, "failed to append mydomain: '%s'", s);
         else if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "added mydomain: '%s'", s);

         memset(s, 0, sizeof(s));
      }

      p_free_results(&sql);
   }

   close_prepared_statement(&sql);
}


int is_email_address_on_my_domains(char *email, struct data *data){
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
