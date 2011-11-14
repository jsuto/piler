/*
 * counters.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <syslog.h>
#include <piler.h>


struct __counters loadCounters(struct session_data *sdata, struct __config *cfg){
   char buf[SMALLBUFSIZE];
   struct __counters counters;

   bzero(&counters, sizeof(counters));

   snprintf(buf, SMALLBUFSIZE-1, "SELECT rcvd, virus, duplicate FROM %s", SQL_COUNTER_TABLE);

#ifdef NEED_MYSQL
   MYSQL_RES *res;
   MYSQL_ROW row;

   if(mysql_real_query(&(sdata->mysql), buf, strlen(buf)) == 0){
      res = mysql_store_result(&(sdata->mysql));
      if(res != NULL){
         row = mysql_fetch_row(res);
         if(row){
            counters.c_rcvd = strtoull(row[0], NULL, 10);
            counters.c_virus = strtoull(row[1], NULL, 10);
            counters.c_duplicate = strtoull(row[2], NULL, 10);
         }
         mysql_free_result(res);
      }
   }

#endif

   return counters;
}


void updateCounters(struct session_data *sdata, struct __data *data, struct __counters *counters, struct __config *cfg){
   char buf[MAXBUFSIZE];
#ifdef HAVE_MEMCACHED
   unsigned long long mc, rcvd;
   struct __counters c;
   char key[MAX_MEMCACHED_KEY_LEN];
   unsigned int flags=0;

   if(cfg->update_counters_to_memcached == 1){

      /* increment counters to memcached */

      if(memcached_increment(&(data->memc), MEMCACHED_MSGS_RCVD, strlen(MEMCACHED_MSGS_RCVD), counters->c_rcvd, &mc) == MEMCACHED_SUCCESS){
         rcvd = mc;

         if(counters->c_ham > 0) memcached_increment(&(data->memc), MEMCACHED_MSGS_HAM, strlen(MEMCACHED_MSGS_HAM), counters->c_ham, &mc);
         if(counters->c_virus > 0) memcached_increment(&(data->memc), MEMCACHED_MSGS_VIRUS, strlen(MEMCACHED_MSGS_VIRUS), counters->c_virus, &mc);
         if(counters->c_duplicate > 0) memcached_increment(&(data->memc), MEMCACHED_MSGS_DUPLICATE, strlen(MEMCACHED_MSGS_DUPLICATE), counters->c_duplicate, &mc);


         bzero(&c, sizeof(c)); 

         snprintf(buf, MAXBUFSIZE-1, "%s %s %s %s", MEMCACHED_MSGS_RCVD, MEMCACHED_MSGS_VIRUS, MEMCACHED_MSGS_DUPLICATE, MEMCACHED_COUNTERS_LAST_UPDATE);

         if(memcached_mget(&(data->memc), buf) == MEMCACHED_SUCCESS){
            while((memcached_fetch_result(&(data->memc), &key[0], &buf[0], &flags))){
               if(!strcmp(key, MEMCACHED_MSGS_RCVD)) c.c_rcvd = strtoull(buf, NULL, 10);
               else if(!strcmp(key, MEMCACHED_MSGS_VIRUS)) c.c_virus = strtoull(buf, NULL, 10);
               else if(!strcmp(key, MEMCACHED_MSGS_DUPLICATE)) c.c_duplicate = strtoull(buf, NULL, 10);
               else if(!strcmp(key, MEMCACHED_COUNTERS_LAST_UPDATE)) mc = strtoull(buf, NULL, 10);
            }


            if(sdata->now - mc > cfg->memcached_to_db_interval && c.c_rcvd > 0 && c.c_rcvd >= rcvd){
               snprintf(buf, SMALLBUFSIZE-1, "%ld", sdata->now); memcached_set(&(data->memc), MEMCACHED_COUNTERS_LAST_UPDATE, strlen(MEMCACHED_COUNTERS_LAST_UPDATE), buf, strlen(buf), 0, 0);

               snprintf(buf, SMALLBUFSIZE-1, "UPDATE `%s` SET rcvd=%llu, virus=%llu, duplicate=%llu", c.c_rcvd, c.c_virus, c.c_duplicate);

               //if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: update counters: %s", sdata->ttmpfile, buf);

               goto EXEC_SQL;
            }
         }

      }
      else {

         c = loadCounters(sdata, cfg);

         snprintf(buf, SMALLBUFSIZE-1, "%ld", sdata->now); memcached_add(&(data->memc), MEMCACHED_COUNTERS_LAST_UPDATE, strlen(MEMCACHED_COUNTERS_LAST_UPDATE), buf, strlen(buf), 0, 0);

         snprintf(buf, SMALLBUFSIZE-1, "%llu", c.c_virus + counters->c_virus); memcached_add(&(data->memc), MEMCACHED_MSGS_VIRUS, strlen(MEMCACHED_MSGS_VIRUS), buf, strlen(buf), 0, 0);
         snprintf(buf, SMALLBUFSIZE-1, "%llu", c.c_rcvd + counters->c_rcvd); memcached_add(&(data->memc), MEMCACHED_MSGS_RCVD, strlen(MEMCACHED_MSGS_RCVD), buf, strlen(buf), 0, 0);
         snprintf(buf, SMALLBUFSIZE-1, "%llu", c.c_duplicate + counters->c_duplicate); memcached_add(&(data->memc), MEMCACHED_MSGS_DUPLICATE, strlen(MEMCACHED_MSGS_DUPLICATE), buf, strlen(buf), 0, 0);
      }

   }
   else {
#endif
      snprintf(buf, SMALLBUFSIZE-1, "UPDATE `%s` SET rcvd=rcvd+%llu, virus=virus+%llu, duplicate=duplicate+%llu", SQL_COUNTER_TABLE, counters->c_rcvd, counters->c_virus, counters->c_duplicate);

      //if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: update counters: %s", sdata->ttmpfile, buf);

#ifdef HAVE_MEMCACHED
EXEC_SQL:
#endif

   #ifdef NEED_MYSQL
      mysql_real_query(&(sdata->mysql), buf, strlen(buf));
   #endif

#ifdef HAVE_MEMCACHED
   }
#endif

}


