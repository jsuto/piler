#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <signal.h>
#include <piler.h>


int get_session_slot(struct smtp_session **sessions, int max_connections);
void init_smtp_session(struct smtp_session *session, int slot, int sd, char *client_addr, struct config *cfg);

#define GOT_CRLF_DOT_CRLF(p) *p == '\r' && *(p+1) == '\n' && *(p+2) == '.' && *(p+3) == '\r' && *(p+4) == '\n' ? 1 : 0

uint64 get_sessions_total_memory(struct smtp_session **sessions, int max_connections){
   uint64 total = 0;

   for(int i=0; i<max_connections; i++){
      if(sessions[i]) total += sessions[i]->bufsize;
   }

   return total;
}


/*
 * If the sending party sets the email size when it sends the "mail from"
 * part in the smtp transaction, eg. MAIL FROM:<jajaja@akakak.lo> size=509603
 * then piler-smtp could know the email size in advance and could do
 * a better estimate on the allowed number of smtp sessions.
 */

uint64 get_sessions_total_expected_mail_size(struct smtp_session **sessions, int max_connections){
   uint64 total = 0;

   for(int i=0; i<max_connections; i++){
      if(sessions[i]) total += sessions[i]->mail_size;
   }

   return total;
}


int start_new_session(struct smtp_session **sessions, int socket, int *num_connections, struct smtp_acl *smtp_acl[], char *client_addr, struct config *cfg){
   int slot;

   /*
    * We have enough connections to serve already
    */

   if(*num_connections >= cfg->max_connections){
      syslog(LOG_PRIORITY, "ERROR: too many connections (%d), cannot accept socket %d", *num_connections, socket);
      send(socket, SMTP_RESP_421_ERR_ALL_PORTS_ARE_BUSY, strlen(SMTP_RESP_421_ERR_ALL_PORTS_ARE_BUSY), 0);
      close(socket);
      return -1;
   }

   // Check remote client against the allowed network ranges
   if(cfg->smtp_access_list && is_blocked_by_pilerscreen(smtp_acl, client_addr)){
      send(socket, SMTP_RESP_550_ERR, strlen(SMTP_RESP_550_ERR), 0);
      close(socket);
      return -1;
   }

   /*
    * We are under the max_smtp_memory threshold
    */

   uint64 expected_total_mail_size = get_sessions_total_expected_mail_size(sessions, cfg->max_connections);
   uint64 total_memory = get_sessions_total_memory(sessions, cfg->max_connections);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "DEBUG: total smtp memory allocated: %llu, expected total size: %llu", total_memory, expected_total_mail_size);

   if(total_memory > cfg->max_smtp_memory || expected_total_mail_size > cfg->max_smtp_memory){
      syslog(LOG_PRIORITY, "ERROR: too much memory consumption: %llu", total_memory);
      send(socket, SMTP_RESP_451_ERR_TOO_MANY_REQUESTS, strlen(SMTP_RESP_451_ERR_TOO_MANY_REQUESTS), 0);
      return -1;
   }

   slot = get_session_slot(sessions, cfg->max_connections);

   if(slot >= 0 && sessions[slot] == NULL){
      sessions[slot] = malloc(sizeof(struct smtp_session));
      if(sessions[slot]){
         init_smtp_session(sessions[slot], slot, socket, client_addr, cfg);

         char smtp_banner[SMALLBUFSIZE];
         snprintf(smtp_banner, sizeof(smtp_banner)-1, SMTP_RESP_220_BANNER, cfg->hostid);
         send(socket, smtp_banner, strlen(smtp_banner), 0);

         (*num_connections)++;

         return 0;
      }
      else {
         syslog(LOG_PRIORITY, "ERROR: malloc() in start_new_session()");
      }
   }
   else {
      syslog(LOG_PRIORITY, "ERROR: couldn't find a slot for the connection");
   }

   send(socket, SMTP_RESP_421_ERR_TMP, strlen(SMTP_RESP_421_ERR_TMP), 0);
   close(socket);

   return -1;
}


int get_session_slot(struct smtp_session **sessions, int max_connections){
   int i;

   for(i=0; i<max_connections; i++){
      if(sessions[i] == NULL) return i;
   }

   return -1;
}


struct smtp_session *get_session_by_socket(struct smtp_session **sessions, int max_connections, int socket){
   int i;

   for(i=0; i<max_connections; i++){
      if(sessions[i] && sessions[i]->net.socket == socket) return sessions[i];
   }

   return NULL;
}


void init_smtp_session(struct smtp_session *session, int slot, int sd, char *client_addr, struct config *cfg){
   session->slot = slot;

   session->protocol_state = SMTP_STATE_INIT;

   session->cfg = cfg;

   session->net.socket = sd;
   session->net.use_ssl = 0;  // use SSL/TLS
   session->net.starttls = 0; // SSL/TLS communication is active (1) or not (0)
   session->net.ctx = NULL;
   session->net.ssl = NULL;

   session->nullbyte = 0;

   snprintf(session->remote_host, sizeof(session->remote_host)-1, "%s", client_addr);

   session->buf = NULL;
   session->buflen = 0;
   session->bufsize = 0;

   reset_smtp_session(session);
}


void free_smtp_session(struct smtp_session *session){
   if(session){

      if(session->buf != NULL){
         free(session->buf);
      }

      if(session->net.use_ssl == 1){
         SSL_shutdown(session->net.ssl);
         SSL_free(session->net.ssl);
      }

      if(session->net.ctx){
         SSL_CTX_free(session->net.ctx);
      }

      free(session);
   }
}


void tear_down_session(struct smtp_session **sessions, int slot, int *num_connections, char *reason){
   if(sessions[slot] == NULL){
      syslog(LOG_PRIORITY, "session already torn down, slot=%d, reason=%s (%d active connections)", slot, reason, *num_connections);
      return;
   }

   if(*num_connections > 0) (*num_connections)--;

   syslog(LOG_PRIORITY, "disconnected from %s on fd=%d, slot=%d, reason=%s (%d active connections)",
          sessions[slot]->remote_host, sessions[slot]->net.socket, slot, reason, *num_connections);

   close(sessions[slot]->net.socket);

   if(sessions[slot]->fd != -1){
      syslog(LOG_PRIORITY, "Removing %s", sessions[slot]->ttmpfile);
      close(sessions[slot]->fd);
      unlink(sessions[slot]->ttmpfile);
      sessions[slot]->fd = -1;
   }

   free_smtp_session(sessions[slot]);
   sessions[slot] = NULL;
}


static inline int get_last_newline_position(char *buf, int buflen){
   int i;

   for(i=buflen; i>0; i--){
      if(*(buf+i) == '\n'){
         i++;
         break;
      }
   }

   return i;
}


void flush_buffer(struct smtp_session *session){
   // In the DATA phase skip the 1st character if it's a dot (.)
   // and there are more characters before the trailing CR-LF
   //
   // See https://www.ietf.org/rfc/rfc5321.html#section-4.5.2 for more
   for(int i=0; i<session->buflen; i++){
      if(*(session->buf+i) == '\n' && *(session->buf+i+1) == '.' && *(session->buf+i+2) == '.'){
         int dst = i + 2;
         int src = dst + 1;
         int l = session->buflen - src;
         memmove(session->buf + dst, session->buf + src, l);
         session->buflen -= 1;
      }
   }

   // Exclude the trailing \r\n.\r\n sequence

   session->buflen -= 5;

   if(write(session->fd, session->buf, session->buflen) != session->buflen){
      session->bad = 1;
      syslog(LOG_PRIORITY, "ERROR (line: %d) %s: failed to write %d bytes", __LINE__, __func__, session->buflen);
   }

   session->tot_len = session->buflen;
}


void handle_data(struct smtp_session *session, char *readbuf, int readlen, struct config *cfg){
   // Update lasttime if we have something to process
   time(&(session->lasttime));

   if(session->protocol_state == SMTP_STATE_BDAT){
      process_bdat(session, readbuf, readlen, cfg);
      return;
   }

   // realloc memory if the new chunk doesn't fit in

   if(session->buflen + readlen + 10 > session->bufsize){
      // Handle if the current memory allocation for this email is above the max_message_size threshold

      if(session->buflen > cfg->max_message_size){
         if(session->too_big == 0) syslog(LOG_PRIORITY, "ERROR: too big email: %d vs %d", session->buflen, cfg->max_message_size);
         session->bad = 1;
         session->too_big = 1;
      }

      if(session->bad == 0){
         char *q = realloc(session->buf, session->bufsize + SMTPBUFSIZE);
         if(q){
            session->buf = q;
            memset(session->buf+session->bufsize, 0, SMTPBUFSIZE);
            session->bufsize += SMTPBUFSIZE;
         } else {
            syslog(LOG_PRIORITY, "ERROR: realloc %s %s %d", session->ttmpfile, __func__, __LINE__);
            session->bad = 1;
         }
      }
   }

   // process smtp command
   if(session->protocol_state != SMTP_STATE_DATA){

      // We got ~2 MB of garbage and no valid smtp command
      // Terminate the connection
      if(session->buflen + readlen > SMTPBUFSIZE - 10){
         session->bad = 1;
      }

      // We are at the beginning of the smtp transaction
      if(session->bad == 1){
         write1(&(session->net), SMTP_RESP_451_ERR, strlen(SMTP_RESP_451_ERR));
         syslog(LOG_PRIORITY, "ERROR: sent 451 temp error back to client %s", session->ttmpfile);
         return;
      }

      //printf("got %d *%s*\n", readlen, readbuf);

      memcpy(session->buf + session->buflen, readbuf, readlen);
      session->buflen += readlen;

      int pos = get_last_newline_position(session->buf, session->buflen);

      if(pos < readlen) return; // no complete command

      process_smtp_command(session, cfg);

      memset(session->buf, 0, session->bufsize);
      session->buflen = 0;

      return;
   }

   if(session->bad == 0){
      memcpy(session->buf + session->buflen, readbuf, readlen);
      session->buflen += readlen;

      char *p = session->buf + session->buflen - 5;
      if(session->buflen >= 5 && GOT_CRLF_DOT_CRLF(p)){
         flush_buffer(session);
         process_command_period(session);
      }
   } else if(strstr(readbuf, "\r\n.\r\n")){
      process_command_period(session);
   }

}


void write_envelope_addresses(struct smtp_session *session, struct config *cfg){
   if(session->fd == -1) return;

   for(int i=0; i<session->num_of_rcpt_to; i++){
      char *p = strchr(session->rcptto[i], '@');
      if(p && strncmp(p+1, cfg->hostid, cfg->hostid_len)){
         char s[SMALLBUFSIZE];
         snprintf(s, sizeof(s)-1, "X-Piler-Envelope-To: %s\n", session->rcptto[i]);
         if(write(session->fd, s, strlen(s)) == -1) syslog(LOG_PRIORITY, "ERROR: %s: cannot write envelope to address", session->ttmpfile);
      }
   }
}
