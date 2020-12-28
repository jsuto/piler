#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <signal.h>
#include <piler.h>


int get_session_slot(struct smtp_session **sessions, int max_connections);
void init_smtp_session(struct smtp_session *session, int slot, int sd, struct config *cfg);


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
   if(cfg->smtp_access_list && is_blocked_by_pilerscreen(smtp_acl, client_addr, cfg)){
      send(socket, SMTP_RESP_550_ERR, strlen(SMTP_RESP_550_ERR), 0);
      close(socket);
      return -1;
   }

   slot = get_session_slot(sessions, cfg->max_connections);

   if(slot >= 0 && sessions[slot] == NULL){
      sessions[slot] = malloc(sizeof(struct smtp_session));
      if(sessions[slot]){
         init_smtp_session(sessions[slot], slot, socket, cfg);

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


void init_smtp_session(struct smtp_session *session, int slot, int sd, struct config *cfg){
   struct sockaddr_in addr;
   socklen_t addr_size = sizeof(struct sockaddr_in);
   char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
   int i;

   session->slot = slot;

   session->buflen = 0;
   session->protocol_state = SMTP_STATE_INIT;

   session->cfg = cfg;

   session->net.socket = sd;
   session->net.use_ssl = 0;  // use SSL/TLS
   session->net.starttls = 0; // SSL/TLS communication is active (1) or not (0)
   session->net.ctx = NULL;
   session->net.ssl = NULL;

   session->last_data_char = 0;

   session->fd = -1;

   memset(session->mailfrom, 0, SMALLBUFSIZE);

   session->num_of_rcpt_to = 0;
   for(i=0; i<MAX_RCPT_TO; i++) memset(session->rcptto[i], 0, SMALLBUFSIZE);

   memset(session->buf, 0, MAXBUFSIZE);
   memset(session->remote_host, 0, INET6_ADDRSTRLEN);

   reset_bdat_counters(session);

   time(&(session->lasttime));

   if(getpeername(session->net.socket, (struct sockaddr *)&addr, &addr_size) == 0 &&
      getnameinfo((struct sockaddr *)&addr, addr_size, hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV) == 0){
         snprintf(session->remote_host, INET6_ADDRSTRLEN-1, "%s", hbuf);
   }
}


void free_smtp_session(struct smtp_session *session){

   if(session){

      if(session->net.use_ssl == 1){
         SSL_shutdown(session->net.ssl);
         SSL_free(session->net.ssl);
      }

      if(session->net.ctx) SSL_CTX_free(session->net.ctx);

      free(session);
   }
}


void tear_down_session(struct smtp_session **sessions, int slot, int *num_connections){
   syslog(LOG_PRIORITY, "disconnected from %s on fd=%d (%d active connections)", sessions[slot]->remote_host, sessions[slot]->net.socket, (*num_connections)-1);

   close(sessions[slot]->net.socket);

   if(sessions[slot]->fd != -1){
      syslog(LOG_PRIORITY, "Removing %s", sessions[slot]->ttmpfile);
      close(sessions[slot]->fd);
      unlink(sessions[slot]->ttmpfile);
      sessions[slot]->fd = -1;
   }

   free_smtp_session(sessions[slot]);
   sessions[slot] = NULL;

   (*num_connections)--;
}


void handle_data(struct smtp_session *session, char *readbuf, int readlen, struct config *cfg){
   int puflen, rc;
   char *p, copybuf[BIGBUFSIZE+MAXBUFSIZE], puf[MAXBUFSIZE];

   // if there's something in the saved buffer, then let's merge them

   if(session->buflen > 0){
      memset(copybuf, 0, sizeof(copybuf));

      memcpy(copybuf, session->buf, session->buflen);
      memcpy(&copybuf[session->buflen], readbuf, readlen);

      session->buflen = 0;
      memset(session->buf, 0, MAXBUFSIZE);

      p = &copybuf[0];
   }
   else {
      p = readbuf;
   }


   do {
      puflen = read_one_line(p, '\n', puf, sizeof(puf)-1, &rc);
      p += puflen;

      if(puflen > 0){
         // Update lasttime if we have a line to process
         time(&(session->lasttime));

         // pass the puffer to process_data() only if there was an '\n'
         // on the line or the puffer does not start with a period
         if(session->protocol_state == SMTP_STATE_DATA && (rc == OK || puf[0] != '.')){
            sig_block(SIGALRM);
            process_data(session, puf, puflen);
            sig_unblock(SIGALRM);
         }
         else if(session->protocol_state == SMTP_STATE_BDAT){
            process_bdat(session, puf, puflen, cfg);
         }
         else if(rc == OK){
            process_smtp_command(session, puf, cfg);
         }
         else {
            snprintf(session->buf, MAXBUFSIZE-1, "%s", puf);
            session->buflen = puflen;
         }
      }

   } while(puflen > 0);

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
