/*
 * counters.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <syslog.h>
#include <piler.h>


struct counters load_counters(struct session_data *sdata){
   char buf[SMALLBUFSIZE];
   struct counters counters;
   struct sql sql;

   bzero(&counters, sizeof(counters));

   snprintf(buf, SMALLBUFSIZE-1, "SELECT `rcvd`, `virus`, `duplicate`, `ignore`, `size`, `stored_size` FROM `%s`", SQL_COUNTER_TABLE);


   if(prepare_sql_statement(sdata, &sql, buf) == ERR) return counters;


   p_bind_init(&sql);

   if(p_exec_stmt(sdata, &sql) == OK){

      p_bind_init(&sql);

      sql.sql[sql.pos] = (char *)&counters.c_rcvd; sql.type[sql.pos] = TYPE_LONGLONG; sql.len[sql.pos] = sizeof(uint64); sql.pos++;
      sql.sql[sql.pos] = (char *)&counters.c_virus; sql.type[sql.pos] = TYPE_LONGLONG; sql.len[sql.pos] = sizeof(uint64); sql.pos++;
      sql.sql[sql.pos] = (char *)&counters.c_duplicate; sql.type[sql.pos] = TYPE_LONGLONG; sql.len[sql.pos] = sizeof(uint64); sql.pos++;
      sql.sql[sql.pos] = (char *)&counters.c_ignore; sql.type[sql.pos] = TYPE_LONGLONG; sql.len[sql.pos] = sizeof(uint64); sql.pos++;
      sql.sql[sql.pos] = (char *)&counters.c_size; sql.type[sql.pos] = TYPE_LONGLONG; sql.len[sql.pos] = sizeof(uint64); sql.pos++;
      sql.sql[sql.pos] = (char *)&counters.c_stored_size; sql.type[sql.pos] = TYPE_LONGLONG; sql.len[sql.pos] = sizeof(uint64); sql.pos++;

      p_store_results(&sql);
      p_fetch_results(&sql);
      p_free_results(&sql);
   }

   close_prepared_statement(&sql);

   return counters;
}


void update_counters(struct session_data *sdata, struct data *data, struct counters *counters, struct config *cfg){
   char buf[MAXBUFSIZE];
#ifdef HAVE_MEMCACHED
   unsigned long long mc;
   struct counters c;
   unsigned int flags=0;
#endif

   if(counters->c_virus + counters->c_duplicate + counters->c_ignore + counters->c_size + counters->c_stored_size == 0) return;

#ifdef HAVE_MEMCACHED
   if(cfg->update_counters_to_memcached == 1){

      /* increment counters to memcached */

      if(memcached_increment(&(data->memc), MEMCACHED_MSGS_RCVD, counters->c_rcvd, &mc) == MEMCACHED_SUCCESS){
         unsigned long long rcvd = mc;

         if(counters->c_virus > 0) memcached_increment(&(data->memc), MEMCACHED_MSGS_VIRUS, counters->c_virus, &mc);
         if(counters->c_duplicate > 0) memcached_increment(&(data->memc), MEMCACHED_MSGS_DUPLICATE, counters->c_duplicate, &mc);
         if(counters->c_ignore > 0) memcached_increment(&(data->memc), MEMCACHED_MSGS_IGNORE, counters->c_ignore, &mc);
         if(counters->c_size > 0) memcached_increment(&(data->memc), MEMCACHED_MSGS_SIZE, counters->c_size, &mc);
         if(counters->c_stored_size > 0) memcached_increment(&(data->memc), MEMCACHED_MSGS_STORED_SIZE, counters->c_stored_size, &mc);


         bzero(&c, sizeof(c));

         snprintf(buf, MAXBUFSIZE-1, "%s %s %s %s %s %s %s", MEMCACHED_MSGS_RCVD, MEMCACHED_MSGS_VIRUS, MEMCACHED_MSGS_DUPLICATE, MEMCACHED_MSGS_IGNORE, MEMCACHED_MSGS_SIZE, MEMCACHED_MSGS_STORED_SIZE, MEMCACHED_COUNTERS_LAST_UPDATE);

         if(memcached_mget(&(data->memc), buf) == MEMCACHED_SUCCESS){
            char key[MAX_MEMCACHED_KEY_LEN];

            while((memcached_fetch_result(&(data->memc), &key[0], &buf[0], &flags))){
               if(!strcmp(key, MEMCACHED_MSGS_RCVD)) c.c_rcvd = strtoull(buf, NULL, 10);
               else if(!strcmp(key, MEMCACHED_MSGS_VIRUS)) c.c_virus = strtoull(buf, NULL, 10);
               else if(!strcmp(key, MEMCACHED_MSGS_DUPLICATE)) c.c_duplicate = strtoull(buf, NULL, 10);
               else if(!strcmp(key, MEMCACHED_MSGS_IGNORE)) c.c_ignore = strtoull(buf, NULL, 10);
               else if(!strcmp(key, MEMCACHED_MSGS_SIZE)) c.c_size = strtoull(buf, NULL, 10);
               else if(!strcmp(key, MEMCACHED_MSGS_STORED_SIZE)) c.c_stored_size = strtoull(buf, NULL, 10);
               else if(!strcmp(key, MEMCACHED_COUNTERS_LAST_UPDATE)) mc = strtoull(buf, NULL, 10);
            }


            if(sdata->now - mc > (unsigned long long)cfg->memcached_to_db_interval && c.c_rcvd > 0 && c.c_rcvd >= rcvd){
               snprintf(buf, SMALLBUFSIZE-1, "%ld", sdata->now); memcached_add(&(data->memc), "set", MEMCACHED_COUNTERS_LAST_UPDATE, buf, strlen(buf), 0, 0);

               snprintf(buf, SMALLBUFSIZE-1, "UPDATE `%s` SET `rcvd`=%llu, `virus`=%llu, `duplicate`=%llu, `ignore`=%llu, `size`=%llu, `stored_size`=%llu", SQL_COUNTER_TABLE, c.c_rcvd, c.c_virus, c.c_duplicate, c.c_ignore, c.c_size, c.c_stored_size);

               //if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: update counters: %s", sdata->ttmpfile, buf);

               p_query(sdata, buf);
            }
         }

      }
      else {

         c = load_counters(sdata);

         snprintf(buf, SMALLBUFSIZE-1, "%ld", sdata->now); memcached_add(&(data->memc), "add", MEMCACHED_COUNTERS_LAST_UPDATE, buf, strlen(buf), 0, 0);

         snprintf(buf, SMALLBUFSIZE-1, "%llu", c.c_virus + counters->c_virus); memcached_add(&(data->memc), "add", MEMCACHED_MSGS_VIRUS, buf, strlen(buf), 0, 0);
         snprintf(buf, SMALLBUFSIZE-1, "%llu", c.c_rcvd + counters->c_rcvd); memcached_add(&(data->memc), "add", MEMCACHED_MSGS_RCVD, buf, strlen(buf), 0, 0);
         snprintf(buf, SMALLBUFSIZE-1, "%llu", c.c_duplicate + counters->c_duplicate); memcached_add(&(data->memc), "add", MEMCACHED_MSGS_DUPLICATE, buf, strlen(buf), 0, 0);
         snprintf(buf, SMALLBUFSIZE-1, "%llu", c.c_ignore + counters->c_ignore); memcached_add(&(data->memc), "add", MEMCACHED_MSGS_IGNORE, buf, strlen(buf), 0, 0);
         snprintf(buf, SMALLBUFSIZE-1, "%llu", c.c_size + counters->c_size); memcached_add(&(data->memc), "add", MEMCACHED_MSGS_SIZE, buf, strlen(buf), 0, 0);
         snprintf(buf, SMALLBUFSIZE-1, "%llu", c.c_stored_size + counters->c_stored_size); memcached_add(&(data->memc), "add", MEMCACHED_MSGS_STORED_SIZE, buf, strlen(buf), 0, 0);
      }

   }
   else {
#endif
      snprintf(buf, SMALLBUFSIZE-1, "UPDATE `%s` SET `rcvd`=`rcvd`+%llu, `virus`=`virus`+%llu, `duplicate`=`duplicate`+%llu, `ignore`=`ignore`+%llu, `size`=`size`+%llu, `stored_size`=`stored_size`+%llu", SQL_COUNTER_TABLE, counters->c_rcvd, counters->c_virus, counters->c_duplicate, counters->c_ignore, counters->c_size, counters->c_stored_size);
      p_query(sdata, buf);

#ifdef HAVE_MEMCACHED
   }
#endif

}
