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
   MYSQL_RES *res;
   MYSQL_ROW row;

   memset(data->mydomains, 0, size);

   snprintf(s, sizeof(s)-1, "SELECT `domain` FROM `%s`", SQL_DOMAIN_TABLE);

   if(mysql_real_query(&(sdata->mysql), s, strlen(s)) == 0){
      res = mysql_store_result(&(sdata->mysql));
      if(res != NULL){
         while((row = mysql_fetch_row(res))){
            snprintf(s, sizeof(s)-1, "%s,", (char*)row[0]);
            len = strlen(s);

            if(clen + len + 1 < size){
               memcpy(data->mydomains+clen, s, len);
               clen += len;
            }
            else break;
         }

         mysql_free_result(res);
      }

   }

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "mydomains: '%s'", data->mydomains);
}

