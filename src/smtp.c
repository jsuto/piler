/*
 * smtp.c, SJ
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
#include <smtp.h>


void process_command_ehlo_lhlo(struct session_data *sdata, struct __data *data, int *protocol_state, char *resp, int resplen, struct __config *cfg){
   char tmpbuf[MAXBUFSIZE];
   char extensions[SMALLBUFSIZE];

   memset(extensions, 0, sizeof(extensions));

   if(*protocol_state == SMTP_STATE_INIT) *protocol_state = SMTP_STATE_HELO;

   if(sdata->tls == 0) snprintf(extensions, sizeof(extensions)-1, "%s", data->starttls);
   if(cfg->enable_chunking == 1) strncat(extensions, SMTP_EXTENSION_CHUNKING, sizeof(extensions)-strlen(extensions)-2);

   snprintf(tmpbuf, sizeof(tmpbuf)-1, SMTP_RESP_250_EXTENSIONS, cfg->hostid, extensions);

   strncat(resp, tmpbuf, resplen-strlen(resp));
}


void process_command_starttls(struct session_data *sdata, struct __data *data, int *protocol_state, int *starttls, int new_sd, char *resp, int resplen, struct __config *cfg){

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: starttls request from client", sdata->ttmpfile);

   if(data->ctx){
      data->ssl = SSL_new(data->ctx);
      if(data->ssl){

         SSL_set_options(data->ssl, SSL_OP_NO_SSLv2|SSL_OP_NO_SSLv3);

         if(SSL_set_fd(data->ssl, new_sd) == 1){
            strncat(resp, SMTP_RESP_220_READY_TO_START_TLS, resplen);
            *starttls = 1;
            *protocol_state = SMTP_STATE_INIT;
            return;
         } syslog(LOG_PRIORITY, "%s: SSL_set_fd() failed", sdata->ttmpfile);
      } syslog(LOG_PRIORITY, "%s: SSL_new() failed", sdata->ttmpfile);
   } syslog(LOG_PRIORITY, "%s: SSL ctx is null!", sdata->ttmpfile);

   strncat(resp, SMTP_RESP_454_ERR_TLS_TEMP_ERROR, resplen);
}


void process_command_mail_from(struct session_data *sdata, int *protocol_state, char *buf, char *resp, int resplen, struct __config *cfg){

   if(*protocol_state != SMTP_STATE_HELO && *protocol_state != SMTP_STATE_PERIOD){
      strncat(resp, SMTP_RESP_503_ERR, resplen);
   }
   else {
      if(*protocol_state == SMTP_STATE_PERIOD){
         if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: initiated new transaction", sdata->ttmpfile);

         unlink(sdata->ttmpfile);
         unlink(sdata->tmpframe);

         init_session_data(sdata, cfg);
      }

      *protocol_state = SMTP_STATE_MAIL_FROM;

      snprintf(sdata->mailfrom, SMALLBUFSIZE-1, "%s\r\n", buf);

      memset(sdata->fromemail, 0, SMALLBUFSIZE);
      extractEmail(sdata->mailfrom, sdata->fromemail);

      strncat(resp, SMTP_RESP_250_OK, strlen(SMTP_RESP_250_OK));

   }

}


void process_command_rcpt_to(struct session_data *sdata, int *protocol_state, char *buf, char *resp, int resplen){

   if(*protocol_state == SMTP_STATE_MAIL_FROM || *protocol_state == SMTP_STATE_RCPT_TO){

      if(strlen(buf) > SMALLBUFSIZE/2){
         strncat(resp, SMTP_RESP_550_ERR_TOO_LONG_RCPT_TO, resplen);
         return;
      }

      if(sdata->num_of_rcpt_to < MAX_RCPT_TO-1){
         extractEmail(buf, sdata->rcptto[sdata->num_of_rcpt_to]);
      }

      *protocol_state = SMTP_STATE_RCPT_TO;

      if(sdata->num_of_rcpt_to < MAX_RCPT_TO-1) sdata->num_of_rcpt_to++;

      strncat(resp, SMTP_RESP_250_OK, resplen);
   }
   else {
      strncat(resp, SMTP_RESP_503_ERR, resplen);
   }
}


void process_command_data(struct session_data *sdata, int *protocol_state, char *resp, int resplen){

   if(*protocol_state != SMTP_STATE_RCPT_TO){
      strncat(resp, SMTP_RESP_503_ERR, resplen);
   }
   else {
      sdata->fd = open(sdata->filename, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP);
      if(sdata->fd == -1){
         syslog(LOG_PRIORITY, "%s: %s", ERR_OPEN_TMP_FILE, sdata->ttmpfile);
         strncat(resp, SMTP_RESP_451_ERR, resplen);
      }
      else {
         *protocol_state = SMTP_STATE_DATA;
         strncat(resp, SMTP_RESP_354_DATA_OK, resplen-1);
      }
   }

}


void process_command_quit(struct session_data *sdata, int *protocol_state, char *resp, int resplen, struct __config *cfg){
   char tmpbuf[MAXBUFSIZE];

   *protocol_state = SMTP_STATE_FINISHED;

   snprintf(tmpbuf, sizeof(tmpbuf)-1, SMTP_RESP_221_GOODBYE, cfg->hostid);
   strncat(resp, tmpbuf, resplen);

   unlink(sdata->ttmpfile);
   unlink(sdata->tmpframe);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: removed", sdata->ttmpfile);
}


void process_command_reset(struct session_data *sdata, int *protocol_state, char *resp, int resplen, struct __config *cfg){

   strncat(resp, SMTP_RESP_250_OK, resplen);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: removed", sdata->ttmpfile);

   unlink(sdata->ttmpfile);
   unlink(sdata->tmpframe);

   init_session_data(sdata, cfg);

   *protocol_state = SMTP_STATE_HELO;
}


void send_buffered_response(struct session_data *sdata, struct __data *data, int starttls, int new_sd, char *resp, struct __config *cfg){
   int rc;
   char ssl_error[SMALLBUFSIZE];

   write1(new_sd, resp, strlen(resp), sdata->tls, data->ssl);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: sent: %s", sdata->ttmpfile, resp);
   memset(resp, 0, MAXBUFSIZE);

   if(starttls == 1 && sdata->tls == 0){

      if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: waiting for ssl handshake", sdata->ttmpfile);

      rc = SSL_accept(data->ssl);

      if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: SSL_accept() finished", sdata->ttmpfile);

      if(rc == 1){
         sdata->tls = 1;
      }
      else {
         ERR_error_string_n(ERR_get_error(), ssl_error, SMALLBUFSIZE);
         syslog(LOG_PRIORITY, "%s: SSL_accept() failed, rc=%d, errorcode: %d, error text: %s\n", sdata->ttmpfile, rc, SSL_get_error(data->ssl, rc), ssl_error);
      }
   }
}


