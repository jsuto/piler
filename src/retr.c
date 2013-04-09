/*
 * retr.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <time.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <piler.h>


int stat_file(struct session_data *sdata, char *f, char **buf, int buflen, struct __config *cfg){
   struct stat st;

   snprintf(*buf, buflen, "%s/%02x/%c%c%c/%c%c/%c%c/%s", cfg->queuedir, cfg->server_id, f[8], f[9], f[10], f[RND_STR_LEN-4], f[RND_STR_LEN-3], f[RND_STR_LEN-2], f[RND_STR_LEN-1], f);
   if(!stat(*buf, &st)) return st.st_size;

   snprintf(*buf, TINYBUFSIZE-1, "%s/%02x/%c%c/%c%c/%c%c/%s", cfg->queuedir, cfg->server_id, f[RND_STR_LEN-6], f[RND_STR_LEN-5], f[RND_STR_LEN-4], f[RND_STR_LEN-3], f[RND_STR_LEN-2], f[RND_STR_LEN-1], f);
   if(!stat(*buf, &st)) return st.st_size;

   return 0;
}


int stat_message(struct session_data *sdata, struct __data *data, char **buf, int buflen, struct __config *cfg){
   int i, attachments, len=0;
   struct ptr_array ptr_arr[MAX_ATTACHMENTS];
   char puf[TINYBUFSIZE];

   if(strlen(sdata->ttmpfile) != RND_STR_LEN){
      return ERR;
   }

   snprintf(*buf, buflen-2, "%s.m", sdata->ttmpfile);
   len = strlen(*buf);

   attachments = query_attachments(sdata, data, &ptr_arr[0], cfg);

   if(attachments == -1){
      return ERR;
   }

   for(i=1; i<=attachments; i++){
      if(len < buflen){
         snprintf(puf, sizeof(puf)-1, " %s.a%d", ptr_arr[i].piler_id, ptr_arr[i].attachment_id);
         strncat(*buf, puf, buflen);
         len += strlen(puf);
      }
   }

   strncat(*buf, "\r\n", buflen-2);

   return OK;
}


int handle_pilerget_request(int new_sd, struct __data *data, struct __config *cfg){
   int len, n, ssl_ok=0, n_files=0;
   char *q, buf[MAXBUFSIZE], puf[MAXBUFSIZE], muf[TINYBUFSIZE], resp[MAXBUFSIZE];
   char ssl_error[SMALLBUFSIZE];
   struct session_data sdata;
   int db_conn=0;
   int rc;
   struct __counters counters;

   struct timezone tz;
   struct timeval tv1, tv2;

#ifdef HAVE_LIBWRAP
   struct request_info req;

   request_init(&req, RQ_DAEMON, PILERGETD_PROGNAME, RQ_FILE, new_sd, 0);
   fromhost(&req);
   if(!hosts_access(&req)){
      send(new_sd, SMTP_RESP_550_ERR_YOU_ARE_BANNED_BY_LOCAL_POLICY, strlen(SMTP_RESP_550_ERR_YOU_ARE_BANNED_BY_LOCAL_POLICY), 0);
      syslog(LOG_PRIORITY, "denied connection from %s by tcp_wrappers", eval_client(&req));
      return 0;
   }
#endif


   init_session_data(&sdata, cfg);
   sdata.tls = 0;

   bzero(&counters, sizeof(counters));


   /* open database connection */

   db_conn = 0;

#ifdef NEED_MYSQL
   rc = 1;
   mysql_init(&(sdata.mysql));
   mysql_options(&(sdata.mysql), MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&cfg->mysql_connect_timeout);
   mysql_options(&(sdata.mysql), MYSQL_OPT_RECONNECT, (const char*)&rc);

   if(mysql_real_connect(&(sdata.mysql), cfg->mysqlhost, cfg->mysqluser, cfg->mysqlpwd, cfg->mysqldb, cfg->mysqlport, cfg->mysqlsocket, 0)){
      db_conn = 1;
      mysql_real_query(&(sdata.mysql), "SET NAMES utf8", strlen("SET NAMES utf8"));
      mysql_real_query(&(sdata.mysql), "SET CHARACTER SET utf8", strlen("SET CHARACTER SET utf8"));
   }
   else
      syslog(LOG_PRIORITY, "%s", ERR_MYSQL_CONNECT);
#endif

   if(db_conn == 1 && create_prepared_statements(&sdata, data) == ERR){
      close_prepared_statements(data);
      mysql_close(&(sdata.mysql));
      db_conn = 0;
   }


   if(db_conn == 0){
      send(new_sd, SMTP_RESP_421_ERR_TMP, strlen(SMTP_RESP_421_ERR_TMP), 0);
      syslog(LOG_PRIORITY, "cannot make prepared statement");
      return 0;
   }


   gettimeofday(&tv1, &tz);


   if(data->ctx){
      data->ssl = SSL_new(data->ctx);
      if(data->ssl){
         if(SSL_set_fd(data->ssl, new_sd) == 1){
            ssl_ok = 1;
         } else syslog(LOG_PRIORITY, "SSL_set_fd() failed");
      } else syslog(LOG_PRIORITY, "SSL_new() failed");
   } else syslog(LOG_PRIORITY, "SSL ctx is null!");


   if(ssl_ok == 0){
      send(new_sd, SMTP_RESP_421_ERR_TMP, strlen(SMTP_RESP_421_ERR_TMP), 0);
      return 0;
   }


   rc = SSL_accept(data->ssl);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "SSL_accept() finished");

   if(rc == 1){
      sdata.tls = 1;
   }
   else {
      ERR_error_string_n(ERR_get_error(), ssl_error, SMALLBUFSIZE);
      syslog(LOG_PRIORITY, "SSL_accept() failed, rc=%d, errorcode: %d, error text: %s\n", rc, SSL_get_error(data->ssl, rc), ssl_error);
      goto QUITTING;
   }


   snprintf(buf, MAXBUFSIZE-1, SMTP_RESP_220_BANNER, cfg->hostid);

   write1(new_sd, buf, strlen(buf), sdata.tls, data->ssl);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "sent: %s", buf);

   while((n = recvtimeoutssl(new_sd, puf, MAXBUFSIZE, TIMEOUT, sdata.tls, data->ssl)) > 0){

      if(strncasecmp(puf, "MESSAGE ", strlen("MESSAGE ")) == 0){
         trimBuffer(puf);
         q = &resp[0];

         memset(resp, 0, sizeof(resp));

         snprintf(sdata.ttmpfile, sizeof(sdata.ttmpfile)-1, "%s", &puf[8]);
         stat_message(&sdata, data, &q, sizeof(resp)-2, cfg);

         write1(new_sd, resp, strlen(resp), sdata.tls, data->ssl);

         continue;
      }


      if(strncasecmp(puf, "STAT ", strlen("STAT ")) == 0){
         trimBuffer(puf);
         q = &muf[0];

         len = stat_file(&sdata, &puf[5], &q, sizeof(muf)-2, cfg);

         snprintf(resp, sizeof(resp)-1, "SIZE %s %d\r\n", &puf[5], len);

         write1(new_sd, resp, strlen(resp), sdata.tls, data->ssl);

         continue;
      }

 
      if(strncasecmp(puf, "RETR ", strlen("RETR ")) == 0){

         trimBuffer(puf);
         q = &muf[0];

         if(strlen(&puf[5]) >= RND_STR_LEN){
            len = stat_file(&sdata, &puf[5], &q, sizeof(muf)-2, cfg);
            file_from_archive_to_network(muf, new_sd, data, cfg);
            n_files++;
         }
         else {
            snprintf(resp, sizeof(resp)-1, "ERR\r\n");
            write1(new_sd, resp, strlen(resp), sdata.tls, data->ssl);
         }

         continue;
      }


      if(strncasecmp(puf, SMTP_CMD_QUIT, strlen(SMTP_CMD_QUIT)) == 0){
         snprintf(resp, sizeof(resp)-1, SMTP_RESP_221_GOODBYE, cfg->hostid);
         write1(new_sd, resp, strlen(resp), sdata.tls, data->ssl);

         gettimeofday(&tv2, &tz);
 
         break;
      }

      snprintf(resp, sizeof(resp)-1, "ERR\r\n");
      write1(new_sd, resp, strlen(resp), sdata.tls, data->ssl);

   }



QUITTING:

#ifdef NEED_MYSQL
   close_prepared_statements(data);
   mysql_close(&(sdata.mysql));
#endif

   SSL_shutdown(data->ssl);
   SSL_free(data->ssl);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "served %d files", n_files);

   return 1;
}


