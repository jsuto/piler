/*
 * session.c, SJ
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
#include "smtp.h"


int handle_smtp_session(struct session_ctx *sctx){
   int i, ret, pos, readpos=0, result, n, protocol_state, prevlen=0;
   char *p, buf[MAXBUFSIZE], puf[MAXBUFSIZE], resp[MAXBUFSIZE], prevbuf[MAXBUFSIZE], last2buf[2*MAXBUFSIZE+1];
   struct session_data sdata;
   int rc;
   struct counters counters;

   struct timezone tz;
   struct timeval tv1, tv2;

   int starttls = 0;

   bzero(&counters, sizeof(counters));

   sctx->inj = ERR;
   sctx->db_conn = 0;
   sctx->status = NULL;
   sctx->counters = &counters;
   sctx->parser_state = NULL;

#ifdef HAVE_LIBWRAP
   if(is_blocked_by_tcp_wrappers(sctx->new_sd) == 1) return 0;
#endif

   srand(getpid());

   protocol_state = SMTP_STATE_INIT;

   init_session_data(&sdata, sctx->cfg);

   sctx->sdata = &sdata;

   sctx->sdata->tls = 0;


   /* open database connection */

#ifdef NEED_MYSQL
   if(open_database(sctx->sdata, sctx->cfg) == OK){
      sctx->db_conn = 1;
   }
   else
      syslog(LOG_PRIORITY, "%s", ERR_MYSQL_CONNECT);
#endif

   if(sctx->db_conn == 0){
      snprintf(buf, MAXBUFSIZE-1, SMTP_RESP_421_ERR_TMP, sctx->cfg->hostid);
      send(sctx->new_sd, buf, strlen(buf), 0);
      return 0;
   }


   gettimeofday(&tv1, &tz);

#ifdef HAVE_LMTP
   snprintf(buf, MAXBUFSIZE-1, LMTP_RESP_220_BANNER, sctx->cfg->hostid);
#else
   snprintf(buf, MAXBUFSIZE-1, SMTP_RESP_220_BANNER, sctx->cfg->hostid);
#endif

   send(sctx->new_sd, buf, strlen(buf), 0);
   if(sctx->cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: sent: %s", sctx->sdata->ttmpfile, buf);

   while((n = recvtimeoutssl(sctx->new_sd, &puf[readpos], sizeof(puf)-readpos, TIMEOUT, sctx->sdata->tls, sctx->data->ssl)) > 0){
         pos = 0;

         /* accept mail data */

         if(protocol_state == SMTP_STATE_DATA){

            /* join the last 2 buffer */

            memset(last2buf, 0, 2*MAXBUFSIZE+1);
            memcpy(last2buf, prevbuf, MAXBUFSIZE);
            memcpy(last2buf+prevlen, puf, MAXBUFSIZE);


            pos = searchStringInBuffer(last2buf, 2*MAXBUFSIZE+1, SMTP_CMD_PERIOD, 5);
            if(pos > 0){

	       /* fix position */
               pos = pos - prevlen;

               if(sctx->cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: period found", sctx->sdata->ttmpfile);


               /* write data only to (and including) the trailing period (.) */
               ret = write(sctx->sdata->fd, puf, pos);
               sctx->sdata->tot_len += ret;

               /* fix posistion! */
               pos += strlen(SMTP_CMD_PERIOD);

               if(sctx->cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: got: (.)", sctx->sdata->ttmpfile);


               protocol_state = SMTP_STATE_PERIOD;


               /* make sure we had a successful read */

               rc = fsync(sctx->sdata->fd);
               close(sctx->sdata->fd);


               gettimeofday(&tv2, &tz);
               sctx->sdata->__acquire = tvdiff(tv2, tv1);


               if(rc){
                  syslog(LOG_PRIORITY, "failed writing data: %s", sctx->sdata->ttmpfile);

               #ifdef HAVE_LMTP
                  for(i=0; i<sctx->sdata->num_of_rcpt_to; i++){
               #endif

                     write1(sctx->new_sd, SMTP_RESP_421_ERR_WRITE_FAILED, strlen(SMTP_RESP_421_ERR_WRITE_FAILED), sctx->sdata->tls, sctx->data->ssl);

               #ifdef HAVE_LMTP
                  }
               #endif

                  memset(puf, 0, MAXBUFSIZE);
                  goto AFTER_PERIOD;
               }

               process_written_file(sctx);



               unlink(sctx->sdata->ttmpfile);
               unlink(sctx->sdata->tmpframe);


               /* if we have nothing after the trailing (.), we can read
                  the next command from the network */

               if(puf[n-3] == '.' && puf[n-2] == '\r' && puf[n-1] == '\n') continue;


               /* if we left something in the puffer, we are ready to proceed
                  to handle the additional commands, such as QUIT */

               /* if we miss the trailing \r\n, ie. we need another read */

               if(puf[n-2] != '\r' && puf[n-1] != '\n'){
                  memmove(puf, puf+pos, n-pos);
                  memset(puf+n-pos, 0, MAXBUFSIZE-n+pos);
                  recvtimeout(sctx->new_sd, buf, MAXBUFSIZE, TIMEOUT);
                  strncat(puf, buf, MAXBUFSIZE-1-n+pos);
                  if(sctx->cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: partial read: %s", sctx->sdata->ttmpfile, puf);
                  pos = 0;
               }

            } /* pos > 0, PERIOD found */
            else {
               ret = write(sctx->sdata->fd, puf, n);
               sctx->sdata->tot_len += ret;

               memcpy(prevbuf, puf, n);
               prevlen = n;

               continue;
            }

         } /* if protocol_state == SMTP_STATE_DATA */

AFTER_PERIOD:

      /* handle smtp commands */

      memset(resp, 0, sizeof(resp));

      p = &puf[pos];
      readpos = 0;

      if(sctx->cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: command=*%s*", sctx->sdata->ttmpfile, p);

      do {
         p = split(p, '\n', buf, sizeof(buf)-1, &result);

         if(result == 0){
            if(strlen(buf) > 0){
               if(sctx->cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: partial read: *%s*", sctx->sdata->ttmpfile, buf);

               snprintf(puf, sizeof(puf)-5, "%s", buf);
               readpos = strlen(puf);
            }

            break;
         }

         if(sctx->cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: got: %s", sctx->sdata->ttmpfile, buf);

         if(strncasecmp(buf, SMTP_CMD_EHLO, strlen(SMTP_CMD_EHLO)) == 0 || strncasecmp(buf, LMTP_CMD_LHLO, strlen(LMTP_CMD_LHLO)) == 0){
            process_command_ehlo_lhlo(sctx, &protocol_state, &resp[0], sizeof(resp)-1);
            continue;

            /* FIXME: implement the ENHANCEDSTATUSCODE extensions */
         }


         if(strncasecmp(buf, SMTP_CMD_HELO, strlen(SMTP_CMD_HELO)) == 0){
            if(protocol_state == SMTP_STATE_INIT) protocol_state = SMTP_STATE_HELO;
            strncat(resp, SMTP_RESP_250_OK, sizeof(resp)-strlen(resp)-1);
            continue;
         }


         if(sctx->cfg->tls_enable > 0 && strncasecmp(buf, SMTP_CMD_STARTTLS, strlen(SMTP_CMD_STARTTLS)) == 0 && strlen(sctx->data->starttls) > 4 && sctx->sdata->tls == 0){
            process_command_starttls(sctx, &protocol_state, &starttls, &resp[0], sizeof(resp)-1);
            continue;
         }


         if(strncasecmp(buf, SMTP_CMD_MAIL_FROM, strlen(SMTP_CMD_MAIL_FROM)) == 0){
            process_command_mail_from(sctx, &protocol_state, buf, &resp[0], sizeof(resp)-1);
            continue;
         }


         if(strncasecmp(buf, SMTP_CMD_RCPT_TO, strlen(SMTP_CMD_RCPT_TO)) == 0){
            process_command_rcpt_to(sctx, &protocol_state, buf, &resp[0], sizeof(resp)-1);
            continue;
         }


         if(strncasecmp(buf, SMTP_CMD_DATA, strlen(SMTP_CMD_DATA)) == 0){
            memset(last2buf, 0, 2*MAXBUFSIZE+1);
            memset(prevbuf, 0, MAXBUFSIZE);
            sctx->inj = ERR;
            prevlen = 0;

            process_command_data(sctx, &protocol_state, &resp[0], sizeof(resp)-1);
            continue; 
         }


         if(sctx->cfg->enable_chunking == 1 && strncasecmp(buf, SMTP_CMD_BDAT, strlen(SMTP_CMD_BDAT)) == 0){

            process_command_bdat(sctx, &protocol_state, buf, &resp[0], sizeof(resp)-1);

            if(protocol_state == SMTP_STATE_BDAT){

               for(i=0; i<sctx->bdat_rounds-1; i++){
                  if(sctx->cfg->verbosity >= _LOG_DEBUG) syslog(LOG_INFO, "%s: sending bdat response (%d)", sctx->sdata->ttmpfile, i);
                  write1(sctx->new_sd, SMTP_RESP_250_BDAT, strlen(SMTP_RESP_250_BDAT), sctx->sdata->tls, sctx->data->ssl);
               }

               process_written_file(sctx);

               unlink(sctx->sdata->ttmpfile);
               unlink(sctx->sdata->tmpframe);
            }

            continue;
         }


         if(strncasecmp(buf, SMTP_CMD_QUIT, strlen(SMTP_CMD_QUIT)) == 0){
            process_command_quit(sctx, &protocol_state, &resp[0], sizeof(resp)-1);
            continue;
         }


         if(strncasecmp(buf, SMTP_CMD_NOOP, strlen(SMTP_CMD_NOOP)) == 0){
            strncat(resp, SMTP_RESP_250_OK, sizeof(resp)-strlen(resp)-1);
            continue;
         }


         if(strncasecmp(buf, SMTP_CMD_RESET, strlen(SMTP_CMD_RESET)) == 0){
            process_command_reset(sctx, &protocol_state, &resp[0], sizeof(resp)-1);
            continue;
         }


         /* by default send 502 command not implemented message */

         syslog(LOG_PRIORITY, "%s: invalid command: *%s*", sctx->sdata->ttmpfile, buf);
         strncat(resp, SMTP_RESP_502_ERR, sizeof(resp)-strlen(resp)-1);
      } while(p);


      if(strlen(resp) > 0){
         send_buffered_response(sctx, starttls, &resp[0]);
         memset(resp, 0, sizeof(resp));
      }


      if(protocol_state == SMTP_STATE_FINISHED){
         goto QUITTING;
      }

   } /* while */

   /*
    * if we are not in SMTP_STATE_QUIT and the message was not injected,
    * ie. we have timed out than send back 421 error message
    */

   if(protocol_state < SMTP_STATE_QUIT && sctx->inj == ERR){
      snprintf(buf, MAXBUFSIZE-1, SMTP_RESP_421_ERR, sctx->cfg->hostid);
      write1(sctx->new_sd, buf, strlen(buf), sctx->sdata->tls, sctx->data->ssl);

      if(sctx->cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: sent: %s", sctx->sdata->ttmpfile, buf);

      if(sctx->sdata->fd != -1){

         syslog(LOG_PRIORITY, "%s: removing stale files: %s, %s", sctx->sdata->ttmpfile, sctx->sdata->ttmpfile, sctx->sdata->tmpframe);

         close(sctx->sdata->fd);
         unlink(sctx->sdata->ttmpfile);
         unlink(sctx->sdata->tmpframe);
      }

      goto QUITTING;
   }


QUITTING:

   update_counters(sctx->sdata, sctx->data, sctx->counters, sctx->cfg);

#ifdef NEED_MYSQL
   close_database(sctx->sdata);
#endif

   if(sctx->sdata->tls == 1){
      SSL_shutdown(sctx->data->ssl);
      SSL_free(sctx->data->ssl);
   }

   if(sctx->cfg->verbosity >= _LOG_INFO) syslog(LOG_PRIORITY, "processed %llu messages", sctx->counters->c_rcvd);

   return (int)sctx->counters->c_rcvd;
}



#ifdef HAVE_LIBWRAP
int is_blocked_by_tcp_wrappers(int sd){
   struct request_info req;

   request_init(&req, RQ_DAEMON, PROGNAME, RQ_FILE, sd, 0);

   fromhost(&req);

   if(!hosts_access(&req)){
      send(sd, SMTP_RESP_550_ERR_YOU_ARE_BANNED_BY_LOCAL_POLICY, strlen(SMTP_RESP_550_ERR_YOU_ARE_BANNED_BY_LOCAL_POLICY), 0);
      syslog(LOG_PRIORITY, "denied connection from %s by tcp_wrappers", eval_client(&req));
      return 1;
   }

   return 0;
}
#endif


void process_written_file(struct session_ctx *sctx){
   int i;
   char *rcpt;
   char delay[SMALLBUFSIZE], tmpbuf[SMALLBUFSIZE];
   struct parser_state parser_state;
   struct timezone tz;
   struct timeval tv1, tv2;

   gettimeofday(&tv1, &tz);

   sctx->data->folder = 0;

   parser_state = parse_message(sctx->sdata, 1, sctx->data, sctx->cfg);
   post_parse(sctx->sdata, &parser_state, sctx->cfg);

   sctx->parser_state = &parser_state;

   gettimeofday(&tv2, &tz);
   sctx->sdata->__parsed = tvdiff(tv2, tv1);

   if(sctx->cfg->syslog_recipients == 1){
      rcpt = sctx->parser_state->b_to;
      do {
         rcpt = split_str(rcpt, " ", tmpbuf, sizeof(tmpbuf)-1);

         if(does_it_seem_like_an_email_address(tmpbuf) == 1){
            syslog(LOG_PRIORITY, "%s: rcpt=%s", sctx->sdata->ttmpfile, tmpbuf);
         }
      } while(rcpt);
   }

   if(sctx->cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: parsed message", sctx->sdata->ttmpfile);

   if(sctx->cfg->archive_only_mydomains == 1 && sctx->sdata->internal_sender == 0 && sctx->sdata->internal_recipient == 0){
      remove_stripped_attachments(sctx->parser_state);
      sctx->inj = ERR_MYDOMAINS;

      snprintf(sctx->sdata->acceptbuf, SMALLBUFSIZE-1, "250 Ok %s\r\n", sctx->sdata->ttmpfile);
      write1(sctx->new_sd, sctx->sdata->acceptbuf, strlen(sctx->sdata->acceptbuf), sctx->sdata->tls, sctx->data->ssl);

      syslog(LOG_PRIORITY, "%s: discarding: not on mydomains, from=%s, message-id=%s", sctx->sdata->ttmpfile, sctx->sdata->fromemail, sctx->parser_state->message_id);

      return;
   }

   make_digests(sctx->sdata, sctx->cfg);

#ifdef HAVE_ANTIVIRUS
   if(sctx->cfg->use_antivirus == 1){
      sctx->sdata->rav = do_av_check(sctx->sdata, &virusinfo[0], sctx->data, sctx->cfg);
   }
#endif


#ifdef HAVE_LMTP
   for(i=0; i<sctx->sdata->num_of_rcpt_to; i++){
#else
   i = 0;
#endif
      if(sctx->cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: round %d in injection", sctx->sdata->ttmpfile, i);

      process_data(sctx);

      send_response_to_data(sctx, sctx->sdata->rcptto[i]);


      snprintf(delay, SMALLBUFSIZE-1, "delay=%.2f, delays=%.2f/%.2f/%.2f/%.2f/%.2f/%.2f",
                                     (sctx->sdata->__acquire+sctx->sdata->__parsed+sctx->sdata->__av+sctx->sdata->__compress+sctx->sdata->__encrypt+sctx->sdata->__store)/1000000.0,
                                     sctx->sdata->__acquire/1000000.0, sctx->sdata->__parsed/1000000.0,
                                     sctx->sdata->__av/1000000.0, sctx->sdata->__compress/1000000.0,
                                     sctx->sdata->__encrypt/1000000.0, sctx->sdata->__store/1000000.0);

      syslog(LOG_PRIORITY, "%s: from=%s, size=%d/%d, attachments=%d, reference=%s, message-id=%s, retention=%d, folder=%d, %s, status=%s",
                                                                                         sctx->sdata->ttmpfile, sctx->sdata->fromemail, sctx->sdata->tot_len,
                                                                                         sctx->sdata->stored_len, sctx->parser_state->n_attachments,
                                                                                         sctx->parser_state->reference, sctx->parser_state->message_id,
                                                                                         sctx->parser_state->retention, sctx->data->folder, delay, sctx->status);

#ifdef HAVE_LMTP
   } /* for */
#endif


}


void process_data(struct session_ctx *sctx){
   char *arule = NULL;
   char virusinfo[SMALLBUFSIZE];

   sctx->inj = ERR;
   sctx->status = S_STATUS_UNDEF;

   if(sctx->db_conn == 1){

      if(sctx->sdata->restored_copy == 1){
         syslog(LOG_PRIORITY, "%s: discarding: restored copy", sctx->sdata->ttmpfile);
         sctx->inj = OK;
      }
      else if(sctx->sdata->tot_len < sctx->cfg->min_message_size){
         syslog(LOG_PRIORITY, "%s: discarding: too short message (%d bytes)", sctx->sdata->ttmpfile, sctx->sdata->tot_len);
         sctx->inj = OK;
      }
      else if(AVIR_VIRUS == sctx->sdata->rav){
         syslog(LOG_PRIORITY, "%s: found virus: %s", sctx->sdata->ttmpfile, virusinfo);
         sctx->counters->c_virus++;
         sctx->inj = OK;
      } else if(strlen(sctx->sdata->bodydigest) < 10) {
         syslog(LOG_PRIORITY, "%s: invalid digest", sctx->sdata->ttmpfile);
         sctx->inj = ERR;
      } else {
         if(sctx->cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: processing message", sctx->sdata->ttmpfile);

         /* check message against archiving rules */

         arule = check_againt_ruleset(sctx->data->archiving_rules, sctx->parser_state, sctx->sdata->tot_len, sctx->sdata->spam_message);

         if(arule){
            syslog(LOG_PRIORITY, "%s: discarding: archiving policy: *%s*", sctx->sdata->ttmpfile, arule);
            sctx->inj = OK;
            sctx->counters->c_ignore++;

            remove_stripped_attachments(sctx->parser_state);

            sctx->status = S_STATUS_DISCARDED;
         }
         else {
            sctx->inj = process_message(sctx->sdata, sctx->parser_state, sctx->data, sctx->cfg);
            unlink(sctx->parser_state->message_id_hash);
            sctx->counters->c_size += sctx->sdata->tot_len;
            sctx->counters->c_stored_size = sctx->sdata->stored_len;

            sctx->status = S_STATUS_STORED;
         }

      }

   }
}


void send_response_to_data(struct session_ctx *sctx, char *rcptto){

   /* set the accept buffer */

   snprintf(sctx->sdata->acceptbuf, SMALLBUFSIZE-1, "250 Ok %s <%s>\r\n", sctx->sdata->ttmpfile, rcptto);

   if(sctx->inj == ERR){
      snprintf(sctx->sdata->acceptbuf, SMALLBUFSIZE-1, "451 %s <%s>\r\n", sctx->sdata->ttmpfile, rcptto);
      sctx->status = S_STATUS_ERROR;
   }

   write1(sctx->new_sd, sctx->sdata->acceptbuf, strlen(sctx->sdata->acceptbuf), sctx->sdata->tls, sctx->data->ssl);

   if(sctx->cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: sent: %s", sctx->sdata->ttmpfile, sctx->sdata->acceptbuf);

   sctx->counters->c_rcvd++;

   if(sctx->inj == ERR_EXISTS){
      syslog(LOG_PRIORITY, "%s: discarding: duplicate message, id: %llu, message-id: %s", sctx->sdata->ttmpfile, sctx->sdata->duplicate_id, sctx->parser_state->message_id);
      sctx->counters->c_duplicate++;
      sctx->status = S_STATUS_DUPLICATE;
   }
}
