/*
 * bdat.c, SJ
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


void reset_bdat_counters(struct smtp_session *session){
   session->bdat_rounds = 0;
   session->bdat_last_round = 0;
   session->bdat_bytes_to_read = 0;
   session->bad = 0;
}


void get_bdat_size_to_read(struct smtp_session *session, char *buf){
   char *p;

   session->bdat_rounds++;
   session->bdat_bytes_to_read = 0;

   session->protocol_state = SMTP_STATE_BDAT;

   // determine if this is the last BDAT command

   p = strcasestr(buf, " LAST");
   if(p){
      session->bdat_last_round = 1;
      syslog(LOG_INFO, "%s: BDAT LAST", session->ttmpfile);
      *p = '\0';
   }

   // determine the size to be read

   p = strchr(buf, ' ');
   if(p){
      session->bdat_bytes_to_read = atoi(p);
      if(session->cfg->verbosity >= _LOG_DEBUG) syslog(LOG_INFO, "%s: BDAT len=%d", session->ttmpfile, session->bdat_bytes_to_read);
   }

   if(!p || session->bdat_bytes_to_read <= 0){
      session->bdat_bytes_to_read = 0;
      syslog(LOG_INFO, "%s: ERROR: malformed BDAT command", session->ttmpfile);
   }
}


void process_bdat(struct smtp_session *session, char *readbuf, int readlen){
   int i;
   char buf[SMALLBUFSIZE];

   if(readlen <= 0) return;

   if(session->bdat_rounds == 1 && session->fd == -1){
      session->fd = open(session->ttmpfile, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP);
      if(session->fd == -1){
         syslog(LOG_PRIORITY, "%s: %s", ERR_OPEN_TMP_FILE, session->ttmpfile);
      }
   }

   session->bdat_bytes_to_read -= readlen;

   if(session->fd != -1){
      if(write(session->fd, readbuf, readlen) != -1){
         session->tot_len += readlen;

         if(session->cfg->verbosity >= _LOG_DEBUG) syslog(LOG_INFO, "%s: wrote %d bytes, %d bytes to go", session->ttmpfile, readlen, session->bdat_bytes_to_read);
      }
      else syslog(LOG_PRIORITY, "ERROR: write(), %s, %d, %s", __func__, __LINE__, __FILE__);
   }

   
   if(session->bdat_bytes_to_read < 0){
      // malformed data from client: we got more data then had been told in BDAT argument
      syslog(LOG_PRIORITY, "ERROR: invalid BDAT data. Expected %d, got %d bytes", session->bdat_bytes_to_read + readlen, readlen);
      session->bad = 1;

      close(session->fd);
      unlink(session->ttmpfile);
   }

   if(session->bdat_bytes_to_read == 0){

      if(session->bdat_last_round == 1){
         if(session->cfg->verbosity >= _LOG_DEBUG) syslog(LOG_INFO, "%s: read all bdat data in %d rounds", session->ttmpfile, session->bdat_rounds);

         // send back the smtp answers
         for(i=0; i<session->bdat_rounds; i++){
            if(session->fd == -1){
               send_smtp_response(session, SMTP_RESP_421_ERR_WRITE_FAILED);
            }
            else {
               if(i == 0){
                  fsync(session->fd);
                  close(session->fd);

                  move_email(session);

                  snprintf(buf, sizeof(buf)-1, "250 OK <%s>\r\n", session->ttmpfile);
                  send_smtp_response(session, buf);
                  syslog(LOG_PRIORITY, "received: %s, from=%s, size=%d", session->ttmpfile, session->mailfrom, session->tot_len);
               }
               else send_smtp_response(session, SMTP_RESP_250_BDAT);
            }
         }

         // technically we are not in the PERIOD state, but it's good enough
         // to quit the BDAT processing state
         session->protocol_state = SMTP_STATE_PERIOD;

         session->fd = -1;
      }

      else {
         // this is not the last BDAT round, let's go back
         // after the rcpt state, and wait for the next
         // BDAT command
         session->protocol_state = SMTP_STATE_RCPT_TO;
      }
   }
}
