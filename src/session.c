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


int handle_smtp_session(int new_sd, struct __data *data, struct __config *cfg){
   int i, ret, pos, n, inj=ERR, state, prevlen=0;
   char *p, buf[MAXBUFSIZE], puf[MAXBUFSIZE], resp[MAXBUFSIZE], prevbuf[MAXBUFSIZE], last2buf[2*MAXBUFSIZE+1];
   char rcpttoemail[SMALLBUFSIZE], virusinfo[SMALLBUFSIZE], delay[SMALLBUFSIZE];
   char *arule = NULL;
   struct session_data sdata;
   struct _state sstate;
   int db_conn=0;
   int rc;
   struct __counters counters;

   struct timezone tz;
   struct timeval tv1, tv2;

#ifdef HAVE_STARTTLS
   int starttls = 0;
   char ssl_error[SMALLBUFSIZE];
#endif


#ifdef HAVE_LIBWRAP
   struct request_info req;

   request_init(&req, RQ_DAEMON, PROGNAME, RQ_FILE, new_sd, 0);
   fromhost(&req);
   if(!hosts_access(&req)){
      send(new_sd, SMTP_RESP_550_ERR_YOU_ARE_BANNED_BY_LOCAL_POLICY, strlen(SMTP_RESP_550_ERR_YOU_ARE_BANNED_BY_LOCAL_POLICY), 0);
      syslog(LOG_PRIORITY, "denied connection from %s by tcp_wrappers", eval_client(&req));
      return 0;
   }
#endif


   state = SMTP_STATE_INIT;

   init_session_data(&sdata, cfg);
   sdata.tls = 0;

   bzero(&counters, sizeof(counters));


   /* open database connection */

   db_conn = 0;

#ifdef NEED_MYSQL
   if(open_database(&sdata, cfg) == OK){
      db_conn = 1;
   }
   else
      syslog(LOG_PRIORITY, "%s", ERR_MYSQL_CONNECT);
#endif

   if(db_conn == 1 && create_prepared_statements(&sdata, data) == ERR){
      close_prepared_statements(data);
      close_database(&sdata);
      db_conn = 0;
   }


   if(db_conn == 0){
      send(new_sd, SMTP_RESP_421_ERR_TMP, strlen(SMTP_RESP_421_ERR_TMP), 0);
      syslog(LOG_PRIORITY, "cannot make prepared statement");
      return 0;
   }


   gettimeofday(&tv1, &tz);

#ifdef HAVE_LMTP
   snprintf(buf, MAXBUFSIZE-1, LMTP_RESP_220_BANNER, cfg->hostid);
#else
   snprintf(buf, MAXBUFSIZE-1, SMTP_RESP_220_BANNER, cfg->hostid);
#endif

   send(new_sd, buf, strlen(buf), 0);
   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: sent: %s", sdata.ttmpfile, buf);

   while((n = recvtimeoutssl(new_sd, puf, MAXBUFSIZE, TIMEOUT, sdata.tls, data->ssl)) > 0){
         pos = 0;

         /* accept mail data */

         if(state == SMTP_STATE_DATA){

            /* join the last 2 buffer */

            memset(last2buf, 0, 2*MAXBUFSIZE+1);
            memcpy(last2buf, prevbuf, MAXBUFSIZE);
            memcpy(last2buf+prevlen, puf, MAXBUFSIZE);


            pos = searchStringInBuffer(last2buf, 2*MAXBUFSIZE+1, SMTP_CMD_PERIOD, 5);
            if(pos > 0){

	       /* fix position */
               pos = pos - prevlen;

               if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: period found", sdata.ttmpfile);


               /* write data only to (and including) the trailing period (.) */
               ret = write(sdata.fd, puf, pos);
               sdata.tot_len += ret;

               /* fix posistion! */
               pos += strlen(SMTP_CMD_PERIOD);

               if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: got: (.)", sdata.ttmpfile);


               state = SMTP_STATE_PERIOD;

               /* make sure we had a successful read */

               rc = fsync(sdata.fd);
               close(sdata.fd);

               gettimeofday(&tv2, &tz);
               sdata.__acquire = tvdiff(tv2, tv1);

               if(rc){
                  syslog(LOG_PRIORITY, "failed writing data: %s", sdata.ttmpfile);

               #ifdef HAVE_LMTP
                  for(i=0; i<sdata.num_of_rcpt_to; i++){
               #endif

                     write1(new_sd, SMTP_RESP_421_ERR_WRITE_FAILED, strlen(SMTP_RESP_421_ERR_WRITE_FAILED), sdata.tls, data->ssl);

               #ifdef HAVE_LMTP
                  }
               #endif

                  memset(puf, 0, MAXBUFSIZE);
                  goto AFTER_PERIOD;
               }


               gettimeofday(&tv1, &tz);

               sstate = parse_message(&sdata, 1, data, cfg);
               post_parse(&sdata, &sstate, cfg);

               gettimeofday(&tv2, &tz);
               sdata.__parsed = tvdiff(tv2, tv1);

               if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: parsed message", sdata.ttmpfile);

               sdata.need_scan = 1;

               make_digests(&sdata, cfg);

            #ifdef HAVE_ANTIVIRUS
               if(cfg->use_antivirus == 1){
                  gettimeofday(&tv1, &tz);
                  sdata.rav = do_av_check(&sdata, rcpttoemail, &virusinfo[0], data, cfg);
                  gettimeofday(&tv2, &tz);
                  sdata.__av = tvdiff(tv2, tv1);
               }
            #endif



            #ifdef HAVE_LMTP
               for(i=0; i<sdata.num_of_rcpt_to; i++){
            #else
               i = 0;
            #endif
                  if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: round %d in injection", sdata.ttmpfile, i);

                  extractEmail(sdata.rcptto[i], rcpttoemail);

                  /* copy default config to enable policy support */
                  //memcpy(&my_cfg, cfg, sizeof(struct __config));

                  inj = ERR;


                  if(db_conn == 1){

                     if(sdata.restored_copy == 1){
                        syslog(LOG_PRIORITY, "%s: discarding restored copy", sdata.ttmpfile);
                        inj = OK;
                     }
                     else if(AVIR_VIRUS == sdata.rav){
                        syslog(LOG_PRIORITY, "%s: found virus: %s", sdata.ttmpfile, virusinfo);
                        counters.c_virus++;
                        inj = OK;
                     } else if(strlen(sdata.bodydigest) < 10) {
                        syslog(LOG_PRIORITY, "%s: invalid digest", sdata.ttmpfile);
                        inj = ERR;
                     } else {
                        if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: processing message", sdata.ttmpfile);

                        /* check message against archiving rules */

                        arule = check_againt_ruleset(data->archiving_rules, &sstate, sdata.tot_len, sdata.spam_message);

                        if(arule){
                           syslog(LOG_PRIORITY, "%s: discarding message by archiving policy: *%s*", sdata.ttmpfile, arule);
                           inj = OK;
                           counters.c_ignore++;

                           remove_stripped_attachments(&sstate);
                        }
                        else {
                           inj = process_message(&sdata, &sstate, data, cfg);
                           counters.c_size += sdata.tot_len;
                        }

                     }

                  }



                  /* set the accept buffer */

                  snprintf(sdata.acceptbuf, SMALLBUFSIZE-1, "250 Ok %s <%s>\r\n", sdata.ttmpfile, rcpttoemail);

                  if(inj == ERR) snprintf(sdata.acceptbuf, SMALLBUFSIZE-1, "451 %s <%s>\r\n", sdata.ttmpfile, rcpttoemail);

                  write1(new_sd, sdata.acceptbuf, strlen(sdata.acceptbuf), sdata.tls, data->ssl);

                  if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: sent: %s", sdata.ttmpfile, sdata.acceptbuf);

                  counters.c_rcvd++;

                  if(inj == ERR_EXISTS){
                     syslog(LOG_PRIORITY, "%s: discarding duplicate message", sdata.ttmpfile);
                     counters.c_duplicate++;
                  }

                  snprintf(delay, SMALLBUFSIZE-1, "delay=%.2f, delays=%.2f/%.2f/%.2f/%.2f/%.2f/%.2f", 
                               (sdata.__acquire+sdata.__parsed+sdata.__av+sdata.__compress+sdata.__encrypt+sdata.__store)/1000000.0,
                                   sdata.__acquire/1000000.0, sdata.__parsed/1000000.0, sdata.__av/1000000.0, sdata.__compress/1000000.0, sdata.__encrypt/1000000.0, sdata.__store/1000000.0);

                  syslog(LOG_PRIORITY, "%s: from=%s, size=%d, reference=%s, message-id=%s, %s", sdata.ttmpfile, sdata.fromemail, sdata.tot_len, sstate.reference, sstate.message_id, delay);



            #ifdef HAVE_LMTP
               } /* for */
            #endif

               unlink(sdata.ttmpfile);
               unlink(sdata.tmpframe);


               /* if we have nothing after the trailing (.), we can read
                  the next command from the network */

               if(puf[n-3] == '.' && puf[n-2] == '\r' && puf[n-1] == '\n') continue;


               /* if we left something in the puffer, we are ready to proceed
                  to handle the additional commands, such as QUIT */

               /* if we miss the trailing \r\n, ie. we need another read */

               if(puf[n-2] != '\r' && puf[n-1] != '\n'){
                  memmove(puf, puf+pos, n-pos);
                  memset(puf+n-pos, 0, MAXBUFSIZE-n+pos);
                  i = recvtimeout(new_sd, buf, MAXBUFSIZE, TIMEOUT);
                  strncat(puf, buf, MAXBUFSIZE-1-n+pos);
                  if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: partial read: %s", sdata.ttmpfile, puf);
                  pos = 0;
               }

            } /* PERIOD found */
            else {
               ret = write(sdata.fd, puf, n);
               sdata.tot_len += ret;

               memcpy(prevbuf, puf, n);
               prevlen = n;

               continue;
            }

         } /* SMTP DATA */

AFTER_PERIOD:

      /* handle smtp commands */

      memset(resp, 0, MAXBUFSIZE);

      p = &puf[pos];

      while((p = split_str(p, "\r\n", buf, MAXBUFSIZE-1))){
         if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: got: %s", sdata.ttmpfile, buf);


         if(strncasecmp(buf, SMTP_CMD_EHLO, strlen(SMTP_CMD_EHLO)) == 0 || strncasecmp(buf, LMTP_CMD_LHLO, strlen(LMTP_CMD_LHLO)) == 0){
            if(state == SMTP_STATE_INIT) state = SMTP_STATE_HELO;

            if(sdata.tls == 0) snprintf(buf, MAXBUFSIZE-1, SMTP_RESP_250_EXTENSIONS, cfg->hostid, data->starttls);
            else snprintf(buf, MAXBUFSIZE-1, SMTP_RESP_250_EXTENSIONS, cfg->hostid, "");

            strncat(resp, buf, MAXBUFSIZE-1);

            continue;

            /* FIXME: implement the ENHANCEDSTATUSCODE extensions */
         }


         if(strncasecmp(buf, SMTP_CMD_HELO, strlen(SMTP_CMD_HELO)) == 0){
            if(state == SMTP_STATE_INIT) state = SMTP_STATE_HELO;

            strncat(resp, SMTP_RESP_250_OK, MAXBUFSIZE-1);

            continue;
         }


      #ifdef HAVE_STARTTLS
         if(cfg->tls_enable > 0 && strncasecmp(buf, SMTP_CMD_STARTTLS, strlen(SMTP_CMD_STARTTLS)) == 0 && strlen(data->starttls) > 4 && sdata.tls == 0){
            if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: starttls request from client", sdata.ttmpfile);

            if(data->ctx){
               data->ssl = SSL_new(data->ctx);
               if(data->ssl){
                  if(SSL_set_fd(data->ssl, new_sd) == 1){
                     strncat(resp, SMTP_RESP_220_READY_TO_START_TLS, MAXBUFSIZE-1);
                     starttls = 1;
                     state = SMTP_STATE_INIT;

                     continue;
                  } syslog(LOG_PRIORITY, "%s: SSL_set_fd() failed", sdata.ttmpfile);
               } syslog(LOG_PRIORITY, "%s: SSL_new() failed", sdata.ttmpfile);
            } syslog(LOG_PRIORITY, "%s: SSL ctx is null!", sdata.ttmpfile);


            strncat(resp, SMTP_RESP_454_ERR_TLS_TEMP_ERROR, MAXBUFSIZE-1);
            continue;
         }
      #endif


         if(strncasecmp(buf, SMTP_CMD_MAIL_FROM, strlen(SMTP_CMD_MAIL_FROM)) == 0){

            if(state != SMTP_STATE_HELO && state != SMTP_STATE_PERIOD){
               strncat(resp, SMTP_RESP_503_ERR, MAXBUFSIZE-1);
            }
            else {

               if(state == SMTP_STATE_PERIOD){
                  if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: initiated new transaction", sdata.ttmpfile);

                  unlink(sdata.ttmpfile);
                  unlink(sdata.tmpframe);

                  init_session_data(&sdata, cfg);
               }

               state = SMTP_STATE_MAIL_FROM;

               snprintf(sdata.mailfrom, SMALLBUFSIZE-1, "%s\r\n", buf);

               memset(sdata.fromemail, 0, SMALLBUFSIZE);
               extractEmail(sdata.mailfrom, sdata.fromemail);

               strncat(resp, SMTP_RESP_250_OK, strlen(SMTP_RESP_250_OK));

            }

            continue;
         }


         if(strncasecmp(buf, SMTP_CMD_RCPT_TO, strlen(SMTP_CMD_RCPT_TO)) == 0){

            if(state == SMTP_STATE_MAIL_FROM || state == SMTP_STATE_RCPT_TO){
               if(strlen(buf) > SMALLBUFSIZE/2){
                  strncat(resp, SMTP_RESP_550_ERR_TOO_LONG_RCPT_TO, MAXBUFSIZE-1);
                  continue;
               }

               if(sdata.num_of_rcpt_to < MAX_RCPT_TO-1){
                  snprintf(sdata.rcptto[sdata.num_of_rcpt_to], SMALLBUFSIZE-1, "%s\r\n", buf);
               }

               state = SMTP_STATE_RCPT_TO;

               /* check against blackhole addresses */

               extractEmail(buf, rcpttoemail);

            #ifdef HAVE_MULTITENANCY
               if(sdata.customer_id == 0) sdata.customer_id = get_customer_id_by_rcpt_to_email(rcpttoemail, data);
            #endif


               if(sdata.num_of_rcpt_to < MAX_RCPT_TO-1) sdata.num_of_rcpt_to++;


               strncat(resp, SMTP_RESP_250_OK, MAXBUFSIZE-1);
            }
            else {
               strncat(resp, SMTP_RESP_503_ERR, MAXBUFSIZE-1);
            }

            continue;
         }


         if(strncasecmp(buf, SMTP_CMD_DATA, strlen(SMTP_CMD_DATA)) == 0){

            memset(last2buf, 0, 2*MAXBUFSIZE+1);
            memset(prevbuf, 0, MAXBUFSIZE);
            inj = ERR;
            prevlen = 0;

            if(state != SMTP_STATE_RCPT_TO){
               strncat(resp, SMTP_RESP_503_ERR, MAXBUFSIZE-1);
            }
            else {
               sdata.fd = open(sdata.filename, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP);
               if(sdata.fd == -1){
                  syslog(LOG_PRIORITY, "%s: %s", ERR_OPEN_TMP_FILE, sdata.ttmpfile);
                  strncat(resp, SMTP_RESP_451_ERR, MAXBUFSIZE-1);
               }
               else {
                  state = SMTP_STATE_DATA;
                  strncat(resp, SMTP_RESP_354_DATA_OK, MAXBUFSIZE-1);
               }

            }

            continue; 
         }


         if(strncasecmp(buf, SMTP_CMD_QUIT, strlen(SMTP_CMD_QUIT)) == 0){

            state = SMTP_STATE_FINISHED;

            snprintf(buf, MAXBUFSIZE-1, SMTP_RESP_221_GOODBYE, cfg->hostid);
            strncat(resp, buf, MAXBUFSIZE-1);

            unlink(sdata.ttmpfile);
            unlink(sdata.tmpframe);
            if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: removed", sdata.ttmpfile);

            continue;
         }


         if(strncasecmp(buf, SMTP_CMD_NOOP, strlen(SMTP_CMD_NOOP)) == 0){
            strncat(resp, SMTP_RESP_250_OK, MAXBUFSIZE-1);
            continue;
         }


         if(strncasecmp(buf, SMTP_CMD_RESET, strlen(SMTP_CMD_RESET)) == 0){

            strncat(resp, SMTP_RESP_250_OK, MAXBUFSIZE-1);

            if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: removed", sdata.ttmpfile);
            unlink(sdata.ttmpfile);
            unlink(sdata.tmpframe);

            init_session_data(&sdata, cfg);

            state = SMTP_STATE_HELO;

            continue;
         }

         /* by default send 502 command not implemented message */

         syslog(LOG_PRIORITY, "%s: invalid command: *%s*", sdata.ttmpfile, buf);
         strncat(resp, SMTP_RESP_502_ERR, MAXBUFSIZE-1);
      }


      /* now we can send our buffered response */

      if(strlen(resp) > 0){
         write1(new_sd, resp, strlen(resp), sdata.tls, data->ssl);

         if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: sent: %s", sdata.ttmpfile, resp);
         memset(resp, 0, MAXBUFSIZE);

      #ifdef HAVE_STARTTLS
         if(starttls == 1 && sdata.tls == 0){

            if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: waiting for ssl handshake", sdata.ttmpfile);

            rc = SSL_accept(data->ssl);

            if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: SSL_accept() finished", sdata.ttmpfile);

            if(rc == 1){
               sdata.tls = 1;
            }
            else {
               ERR_error_string_n(ERR_get_error(), ssl_error, SMALLBUFSIZE);
               syslog(LOG_PRIORITY, "%s: SSL_accept() failed, rc=%d, errorcode: %d, error text: %s\n", sdata.ttmpfile, rc, SSL_get_error(data->ssl, rc), ssl_error);
            }
         }
      #endif


      }

      if(state == SMTP_STATE_FINISHED){
         goto QUITTING;
      }

   } /* while */

   /*
    * if we are not in SMTP_STATE_QUIT and the message was not injected,
    * ie. we have timed out than send back 421 error message
    */

   if(state < SMTP_STATE_QUIT && inj == ERR){
      snprintf(buf, MAXBUFSIZE-1, SMTP_RESP_421_ERR, cfg->hostid);
      write1(new_sd, buf, strlen(buf), sdata.tls, data->ssl);

      if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: sent: %s", sdata.ttmpfile, buf);

      if(sdata.fd != -1){

         syslog(LOG_PRIORITY, "%s: removing stale files: %s, %s", sdata.ttmpfile, sdata.ttmpfile, sdata.tmpframe);

         close(sdata.fd);
         unlink(sdata.ttmpfile);
         unlink(sdata.tmpframe);
      }

      goto QUITTING;
   }


QUITTING:

   update_counters(&sdata, data, &counters, cfg);

#ifdef NEED_MYSQL
   close_prepared_statements(data);
   close_database(&sdata);
#endif

#ifdef HAVE_STARTTLS
   if(sdata.tls == 1){
      SSL_shutdown(data->ssl);
      SSL_free(data->ssl);
   }
#endif

   if(cfg->verbosity >= _LOG_INFO) syslog(LOG_PRIORITY, "processed %llu messages", counters.c_rcvd);

   return (int)counters.c_rcvd;
}


