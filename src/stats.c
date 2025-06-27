/*
 * stats.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>
#include <syslog.h>
#include <getopt.h>
#include <piler.h>

#define SMTP_TIMEOUT 5

extern char *optarg;
extern int optind;

struct stats {
   uint64 rcvd;
   uint64 size;
   uint64 ssize;

   uint64 sphx;
   uint64 ram_bytes;
   uint64 disk_bytes;

   uint64 error_emails;
   long last_email;
   float smtp_response_time;
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


int get_last_email_archived_timestamp(struct session_data *sdata, struct stats *stats){
   int rc=ERR;
   unsigned long arrived=86400;
   struct sql sql;

   // By default we haven't archived a mail a day ago, and this value should represent an error
   time(&(sdata->now));
   arrived = sdata->now - 86400;

   if(prepare_sql_statement(sdata, &sql, "select arrived from metadata order by id desc limit 1") == ERR) return rc;

   p_bind_init(&sql);

   if(p_exec_stmt(sdata, &sql) == OK){
      p_bind_init(&sql);

      sql.sql[sql.pos] = (char *)&arrived; sql.type[sql.pos] = TYPE_LONG; sql.len[sql.pos] = sizeof(arrived); sql.pos++;

      p_store_results(&sql);

      if(p_fetch_results(&sql) == OK) rc = OK;

      p_free_results(&sql);
   }

   close_prepared_statement(&sql);

   stats->last_email = sdata->now - arrived;

   return rc;
}


void sphinx_queries(struct session_data *sdata, struct stats *stats, struct config *cfg){
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

   char s[SMALLBUFSIZE];
   snprintf(s, sizeof(s)-1, "SHOW INDEX main1 STATUS");
   if(cfg->rtindex){
      snprintf(s, sizeof(s)-1, "SHOW INDEX %s STATUS", cfg->sphxdb);
   }

   p_query(sdata, s);

   result = mysql_store_result(&(sdata->mysql));
   if(result){
      while((row = mysql_fetch_row(result))){
         if(strcmp((char*)row[0], "ram_bytes") == 0) stats->ram_bytes = strtoull(row[1], NULL, 10);
         if(strcmp((char*)row[0], "disk_bytes") == 0) stats->disk_bytes = strtoull(row[1], NULL, 10);
      }

      mysql_free_result(result);
   }
}


void count_error_emails(struct stats *stats){
   DIR *dir;
   struct stat st;

   dir = opendir(ERROR_DIR);
   if(dir){
      struct dirent *de;
      while((de = readdir(dir))){
         if(strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) continue;

         char buf[SMALLBUFSIZE];
         snprintf(buf, sizeof(buf)-1, "%s/%s", ERROR_DIR, de->d_name);

         if(stat(buf, &st) == 0 && S_ISREG(st.st_mode)){
            stats->error_emails++;
         }
      }

      closedir(dir);
   }
}


void check_smtp_status(struct stats *stats, struct config *cfg){
   int sd, rc;
   char port_string[8];
   char buf[SMALLBUFSIZE];
   struct addrinfo hints, *res;
   struct timezone tz;
   struct timeval tv1, tv2;

   // Set this to a very high number, 1 hour in ms
   stats->smtp_response_time = 3600000;

   memset(buf, 0, sizeof(buf));

   snprintf(port_string, sizeof(port_string)-1, "%d", cfg->listen_port);

   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;

   if((rc = getaddrinfo(cfg->listen_addr, port_string, &hints, &res)) != 0){
      fprintf(stderr, "getaddrinfo for '%s': %s\n", cfg->listen_addr, gai_strerror(rc));
      return;
   }

   if((sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1){
      fprintf(stderr, "cannot create socket\n");
      goto ENDE_CHECK_SMTP_STATUS;
   }

   gettimeofday(&tv1, &tz);

   if(connect(sd, res->ai_addr, res->ai_addrlen) == -1){
      fprintf(stderr, "connect()\n");
      goto ENDE_CHECK_SMTP_STATUS;
   }

   recvtimeout(sd, buf, sizeof(buf)-1, SMTP_TIMEOUT);

   close(sd);

   gettimeofday(&tv2, &tz);

   if(strncmp(buf, "220 ", 4) == 0){
      stats->smtp_response_time = tvdiff(tv2, tv1) / 1000.0; // response time in ms
   }

ENDE_CHECK_SMTP_STATUS:
   freeaddrinfo(res);
}


void print_json_results(struct stats *stats){
   printf("{\n");
   printf("\t\"rcvd\": %llu,\n", stats->rcvd);
   printf("\t\"size\": %llu,\n", stats->size);
   printf("\t\"ssize\": %llu,\n", stats->ssize);
   printf("\t\"sphx\": %llu,\n", stats->sphx);
   printf("\t\"ram_bytes\": %llu,\n", stats->ram_bytes);
   printf("\t\"disk_bytes\": %llu,\n", stats->disk_bytes);
   printf("\t\"error_emails\": %llu,\n", stats->error_emails);
   printf("\t\"last_email\": %ld,\n", stats->last_email);
   printf("\t\"smtp_response\": %.2f\n", stats->smtp_response_time);

   printf("}\n");
}


int main(){
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
   get_last_email_archived_timestamp(&sdata, &stats);

   close_database(&sdata);

   if(open_sphx(&sdata, &cfg) == ERR) return 0;

   sphinx_queries(&sdata, &stats, &cfg);

   close_sphx(&sdata);

   count_error_emails(&stats);

   check_smtp_status(&stats, &cfg);

   print_json_results(&stats);

   return 0;
}
