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


struct __counters load_counters(struct session_data *sdata, struct __data *data, struct __config *cfg){
   char buf[SMALLBUFSIZE];
   struct __counters counters;

   bzero(&counters, sizeof(counters));

   snprintf(buf, SMALLBUFSIZE-1, "SELECT `rcvd`, `virus`, `duplicate`, `ignore`, `size`, `stored_size` FROM `%s`", SQL_COUNTER_TABLE);


   if(prepare_sql_statement(sdata, &(data->stmt_generic), buf) == ERR) return counters;


   p_bind_init(data);

   if(p_exec_query(sdata, data->stmt_generic, data) == OK){

      p_bind_init(data);

      data->sql[data->pos] = (char *)&counters.c_rcvd; data->type[data->pos] = TYPE_LONGLONG; data->len[data->pos] = sizeof(uint64); data->pos++;
      data->sql[data->pos] = (char *)&counters.c_virus; data->type[data->pos] = TYPE_LONGLONG; data->len[data->pos] = sizeof(uint64); data->pos++;
      data->sql[data->pos] = (char *)&counters.c_duplicate; data->type[data->pos] = TYPE_LONGLONG; data->len[data->pos] = sizeof(uint64); data->pos++;
      data->sql[data->pos] = (char *)&counters.c_ignore; data->type[data->pos] = TYPE_LONGLONG; data->len[data->pos] = sizeof(uint64); data->pos++;
      data->sql[data->pos] = (char *)&counters.c_size; data->type[data->pos] = TYPE_LONGLONG; data->len[data->pos] = sizeof(uint64); data->pos++;
      data->sql[data->pos] = (char *)&counters.c_stored_size; data->type[data->pos] = TYPE_LONGLONG; data->len[data->pos] = sizeof(uint64); data->pos++;

      p_store_results(sdata, data->stmt_generic, data);
      p_fetch_results(data->stmt_generic);
      p_free_results(data->stmt_generic);
   }

   close_prepared_statement(data->stmt_generic);

   return counters;
}


void update_counters(struct session_data *sdata, struct __data *data, struct __counters *counters, struct __config *cfg){
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

         if(counters->c_virus > 0) memcached_increment(&(data->memc), MEMCACHED_MSGS_VIRUS, strlen(MEMCACHED_MSGS_VIRUS), counters->c_virus, &mc);
         if(counters->c_duplicate > 0) memcached_increment(&(data->memc), MEMCACHED_MSGS_DUPLICATE, strlen(MEMCACHED_MSGS_DUPLICATE), counters->c_duplicate, &mc);
         if(counters->c_ignore > 0) memcached_increment(&(data->memc), MEMCACHED_MSGS_IGNORE, strlen(MEMCACHED_MSGS_IGNORE), counters->c_ignore, &mc);
         if(counters->c_size > 0) memcached_increment(&(data->memc), MEMCACHED_MSGS_SIZE, strlen(MEMCACHED_MSGS_SIZE), counters->c_size, &mc);
         if(counters->c_stored_size > 0) memcached_increment(&(data->memc), MEMCACHED_MSGS_STORED_SIZE, strlen(MEMCACHED_MSGS_STORED_SIZE), counters->c_stored_size, &mc);


         bzero(&c, sizeof(c)); 

         snprintf(buf, MAXBUFSIZE-1, "%s %s %s %s %s %s %s", MEMCACHED_MSGS_RCVD, MEMCACHED_MSGS_VIRUS, MEMCACHED_MSGS_DUPLICATE, MEMCACHED_MSGS_IGNORE, MEMCACHED_MSGS_SIZE, MEMCACHED_MSGS_STORED_SIZE, MEMCACHED_COUNTERS_LAST_UPDATE);

         if(memcached_mget(&(data->memc), buf) == MEMCACHED_SUCCESS){
            while((memcached_fetch_result(&(data->memc), &key[0], &buf[0], &flags))){
               if(!strcmp(key, MEMCACHED_MSGS_RCVD)) c.c_rcvd = strtoull(buf, NULL, 10);
               else if(!strcmp(key, MEMCACHED_MSGS_VIRUS)) c.c_virus = strtoull(buf, NULL, 10);
               else if(!strcmp(key, MEMCACHED_MSGS_DUPLICATE)) c.c_duplicate = strtoull(buf, NULL, 10);
               else if(!strcmp(key, MEMCACHED_MSGS_IGNORE)) c.c_ignore = strtoull(buf, NULL, 10);
               else if(!strcmp(key, MEMCACHED_MSGS_SIZE)) c.c_size = strtoull(buf, NULL, 10);
               else if(!strcmp(key, MEMCACHED_MSGS_STORED_SIZE)) c.c_stored_size = strtoull(buf, NULL, 10);
               else if(!strcmp(key, MEMCACHED_COUNTERS_LAST_UPDATE)) mc = strtoull(buf, NULL, 10);
            }


            if(sdata->now - mc > cfg->memcached_to_db_interval && c.c_rcvd > 0 && c.c_rcvd >= rcvd){
               snprintf(buf, SMALLBUFSIZE-1, "%ld", sdata->now); memcached_set(&(data->memc), MEMCACHED_COUNTERS_LAST_UPDATE, strlen(MEMCACHED_COUNTERS_LAST_UPDATE), buf, strlen(buf), 0, 0);

               snprintf(buf, SMALLBUFSIZE-1, "UPDATE `%s` SET `rcvd`=%llu, `virus`=%llu, `duplicate`=%llu, `ignore`=%llu, `size`=%llu, `stored_size`=%llu", SQL_COUNTER_TABLE, c.c_rcvd, c.c_virus, c.c_duplicate, c.c_ignore, c.c_size, c.c_stored_size);
 
               //if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: update counters: %s", sdata->ttmpfile, buf);

               p_query(sdata, buf);
            }
         }

      }
      else {

         c = load_counters(sdata, data, cfg);

         snprintf(buf, SMALLBUFSIZE-1, "%ld", sdata->now); memcached_add(&(data->memc), MEMCACHED_COUNTERS_LAST_UPDATE, strlen(MEMCACHED_COUNTERS_LAST_UPDATE), buf, strlen(buf), 0, 0);

         snprintf(buf, SMALLBUFSIZE-1, "%llu", c.c_virus + counters->c_virus); memcached_add(&(data->memc), MEMCACHED_MSGS_VIRUS, strlen(MEMCACHED_MSGS_VIRUS), buf, strlen(buf), 0, 0);
         snprintf(buf, SMALLBUFSIZE-1, "%llu", c.c_rcvd + counters->c_rcvd); memcached_add(&(data->memc), MEMCACHED_MSGS_RCVD, strlen(MEMCACHED_MSGS_RCVD), buf, strlen(buf), 0, 0);
         snprintf(buf, SMALLBUFSIZE-1, "%llu", c.c_duplicate + counters->c_duplicate); memcached_add(&(data->memc), MEMCACHED_MSGS_DUPLICATE, strlen(MEMCACHED_MSGS_DUPLICATE), buf, strlen(buf), 0, 0);
         snprintf(buf, SMALLBUFSIZE-1, "%llu", c.c_ignore + counters->c_ignore); memcached_add(&(data->memc), MEMCACHED_MSGS_IGNORE, strlen(MEMCACHED_MSGS_IGNORE), buf, strlen(buf), 0, 0);
         snprintf(buf, SMALLBUFSIZE-1, "%llu", c.c_size + counters->c_size); memcached_add(&(data->memc), MEMCACHED_MSGS_SIZE, strlen(MEMCACHED_MSGS_SIZE), buf, strlen(buf), 0, 0);
         snprintf(buf, SMALLBUFSIZE-1, "%llu", c.c_stored_size + counters->c_stored_size); memcached_add(&(data->memc), MEMCACHED_MSGS_STORED_SIZE, strlen(MEMCACHED_MSGS_STORED_SIZE), buf, strlen(buf), 0, 0);
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


