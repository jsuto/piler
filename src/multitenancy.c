/*
 * multitenancy.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <piler.h>


void load_customers(struct session_data *sdata, struct __data *data, struct __config *cfg){
   int rc;
   char s[SMALLBUFSIZE];
   MYSQL_RES *res;
   MYSQL_ROW row;

   data->customers = NULL;

   snprintf(s, sizeof(s)-1, "SELECT `id`, `guid` FROM `%s`", SQL_CUSTOMER_TABLE);

   if(mysql_real_query(&(sdata->mysql), s, strlen(s)) == 0){
      res = mysql_store_result(&(sdata->mysql));
      if(res != NULL){
         while((row = mysql_fetch_row(res))){
            snprintf(s, sizeof(s)-1, "%s@%d", (char*)row[1], atoi(row[0]));
            rc = append_list(&(data->customers), s);
            if(cfg->verbosity >= _LOG_DEBUG && rc == 1) syslog(LOG_PRIORITY, "adding customer: id: %d, guid: '%s'", atoi(row[0]), row[1]);
            if(rc == -1) syslog(LOG_PRIORITY, "error: failed to add customer: id: %d, guid: '%s'", atoi(row[0]), row[1]);
         }

         mysql_free_result(res);
      }

   }

}


unsigned char get_customer_id_by_rcpt_to_email(char *rcpttoemail, struct __data *data){
   int len;
   char *q;
   struct list *p;

   q = strchr(rcpttoemail, '@');
   if(!q || strlen(q) < 3) return 0;

   len = strlen(rcpttoemail) - strlen(q) + 1;
   if(len < 5) return 0;

   p = data->customers;

   while(p != NULL){
      if(strncasecmp(p->s, rcpttoemail, len) == 0){
         q = strchr(p->s, '@');

         if(!q || strlen(q) < 2) return 0;

         return atoi(q+1);
      }

      p = p->r;
   }

   return 0;
}


