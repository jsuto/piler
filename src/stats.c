/*
 * stats.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>
#include <syslog.h>
#include <getopt.h>
#include <piler.h>


extern char *optarg;
extern int optind;

struct stats {
   uint64 rcvd;
   uint64 size;
   uint64 ssize;

   uint64 sphx;
   uint64 ram_bytes;
   uint64 disk_bytes;
};


int query_counters(struct session_data *sdata, struct stats *stats){
   int rc=ERR;
   struct sql sql;

   if(prepare_sql_statement(sdata, &sql, "select rcvd, size, stored_size from counter") == ERR) return rc;

   p_bind_init(&sql);

   if(p_exec_stmt(sdata, &sql) == OK){
      p_bind_init(&sql);

      sql.sql[sql.pos] = (char *)&(stats->rcvd); sql.type[sql.pos] = TYPE_LONGLONG; sql.len[sql.pos] = sizeof(uint64); sql.pos++;
      sql.sql[sql.pos] = (char *)&(stats->size); sql.type[sql.pos] = TYPE_LONGLONG; sql.len[sql.pos] = sizeof(uint64); sql.pos++;
      sql.sql[sql.pos] = (char *)&(stats->ssize); sql.type[sql.pos] = TYPE_LONGLONG; sql.len[sql.pos] = sizeof(uint64); sql.pos++;

      p_store_results(&sql);

      if(p_fetch_results(&sql) == OK) rc = OK;

      p_free_results(&sql);
   }

   close_prepared_statement(&sql);

   return rc;
}


void sphinx_queries(struct session_data *sdata, struct stats *stats){
   MYSQL_RES *result;
   MYSQL_ROW row;

   p_query(sdata, "SHOW STATUS LIKE 'queries'");

   result = mysql_store_result(&(sdata->mysql));
   if(result){
      row = mysql_fetch_row(result);

      if(row){
         if(mysql_num_fields(result) == 2){
            stats->sphx = strtoull(row[1], NULL, 10);
         }
      }

      mysql_free_result(result);
   }

   p_query(sdata, "SHOW INDEX main1 STATUS");

   result = mysql_store_result(&(sdata->mysql));
   if(result){
      while((row = mysql_fetch_row(result))){
         if(strcmp((char*)row[0], "ram_bytes") == 0) stats->ram_bytes = strtoull(row[1], NULL, 10);
         if(strcmp((char*)row[0], "disk_bytes") == 0) stats->disk_bytes = strtoull(row[1], NULL, 10);
      }

      mysql_free_result(result);
   }
}


void print_json_results(struct stats *stats){
   printf("{\n");
   printf("\t\"rcvd\": %llu,\n", stats->rcvd);
   printf("\t\"size\": %llu,\n", stats->size);
   printf("\t\"ssize\": %llu,\n", stats->ssize);
   printf("\t\"sphx\": %llu,\n", stats->sphx);
   printf("\t\"ram_bytes\": %llu,\n", stats->ram_bytes);
   printf("\t\"disk_bytes\": %llu\n", stats->disk_bytes);
   printf("}\n");
}


int main(int argc, char **argv){
   struct session_data sdata;
   struct stats stats;
   struct config cfg;
   char *configfile=CONFIG_FILE;

   memset(&stats, 0, sizeof(stats));

   srand(getpid());

   (void) openlog("pilerstat", LOG_PID, LOG_MAIL);

   cfg = read_config(configfile);

   if(open_database(&sdata, &cfg) == ERR) return 0;

   query_counters(&sdata, &stats);

   close_database(&sdata);


   cfg.mysqlsocket[0] = '\0';
   snprintf(cfg.mysqlhost, MAXVAL-2, "127.0.0.1");
   cfg.mysqlport = 9306;

   if(open_database(&sdata, &cfg) == ERR) return 0;

   sphinx_queries(&sdata, &stats);
   
   close_database(&sdata);

   print_json_results(&stats);

   return 0;
}
