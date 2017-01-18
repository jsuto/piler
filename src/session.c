#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <piler.h>


int get_session_slot(struct smtp_session **sessions, int max_connections);
void init_smtp_session(struct smtp_session *session, int slot, int sd, struct __config *cfg);


#ifdef HAVE_LIBWRAP
int is_blocked_by_tcp_wrappers(int sd){
   struct request_info req;

   request_init(&req, RQ_DAEMON, "piler", RQ_FILE, sd, 0);

   fromhost(&req);

   if(!hosts_access(&req)){
      send(sd, SMTP_RESP_550_ERR_YOU_ARE_BANNED_BY_LOCAL_POLICY, strlen(SMTP_RESP_550_ERR_YOU_ARE_BANNED_BY_LOCAL_POLICY), 0);
      syslog(LOG_PRIORITY, "denied connection from %s by tcp_wrappers", eval_client(&req));
      return 1;
   }

   return 0;
}
#endif


int start_new_session(struct smtp_session **sessions, int socket, int *num_connections, struct __config *cfg){
   char smtp_banner[SMALLBUFSIZE];
   int slot;

   /*
    * We have enough connections to serve already
    */

   if(*num_connections >= cfg->max_connections){
      syslog(LOG_PRIORITY, "too many connections (%d), cannot accept socket %d", *num_connections, socket);
      send(socket, SMTP_RESP_421_ERR_ALL_PORTS_ARE_BUSY, strlen(SMTP_RESP_421_ERR_ALL_PORTS_ARE_BUSY), 0);
      close(socket);
      return -1;
   }

#ifdef HAVE_LIBWRAP
   if(is_blocked_by_tcp_wrappers(socket) == 1){
      close(socket);
      return -1;
   }
#endif

   slot = get_session_slot(sessions, cfg->max_connections);

   if(slot >= 0 && sessions[slot] == NULL){
      sessions[slot] = malloc(sizeof(struct smtp_session));
      if(sessions[slot]){
         init_smtp_session(sessions[slot], slot, socket, cfg);
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
      if(sessions[i] && sessions[i]->socket == socket) return sessions[i];
   }

   return NULL;
}


void init_smtp_session(struct smtp_session *session, int slot, int sd, struct __config *cfg){
   struct sockaddr_in addr;
   socklen_t addr_size = sizeof(struct sockaddr_in);
   char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

   session->slot = slot;

   session->socket = sd;
   session->buflen = 0;
   session->protocol_state = SMTP_STATE_INIT;

   session->cfg = cfg;

   session->use_ssl = 0;  // use SSL/TLS
   session->starttls = 0; // SSL/TLS communication is active (1) or not (0)
   session->ctx = NULL;
   session->ssl = NULL;

   memset(session->buf, 0, SMALLBUFSIZE);
   memset(session->remote_host, 0, INET6_ADDRSTRLEN);

   reset_bdat_counters(session);

   time(&(session->lasttime));

   if(getpeername(sd, (struct sockaddr *)&addr, &addr_size) == 0 &&
      getnameinfo((struct sockaddr *)&addr, addr_size, hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV) == 0){
         snprintf(session->remote_host, INET6_ADDRSTRLEN-1, "%s", hbuf);
   }
}


void free_smtp_session(struct smtp_session *session){

   if(session){

      if(session->use_ssl == 1){
         SSL_shutdown(session->ssl);
         SSL_free(session->ssl);
      }

      if(session->ctx) SSL_CTX_free(session->ctx);

      free(session);
   }
}


void tear_down_session(struct smtp_session **sessions, int slot, int *num_connections){
   syslog(LOG_PRIORITY, "disconnected from %s", sessions[slot]->remote_host);

   close(sessions[slot]->socket);

   free_smtp_session(sessions[slot]);
   sessions[slot] = NULL;

   (*num_connections)--;
}


void handle_data(struct smtp_session *session, char *readbuf, int readlen){
   char *p, puf[MAXBUFSIZE];
   int result;

   // process BDAT stuff

   if(session->protocol_state == SMTP_STATE_BDAT){
      if(session->bad == 1){
         // something bad happened in the BDAT processing
         return;
      }

      process_bdat(session, readbuf, readlen);
   }

   // process DATA

   else if(session->protocol_state == SMTP_STATE_DATA){
      process_data(session, readbuf, readlen);
   }

   // process other SMTP commands

   else {

      if(session->buflen > 0){
         snprintf(puf, sizeof(puf)-1, "%s%s", session->buf, readbuf);
         snprintf(readbuf, BIGBUFSIZE-1, "%s", puf);

         session->buflen = 0;
         memset(session->buf, 0, SMALLBUFSIZE);
      }

      p = readbuf;

      do {
         memset(puf, 0, sizeof(puf));
         p = split(p, '\n', puf, sizeof(puf)-1, &result);

         if(puf[0] == '\0') continue;

         if(result == 1){
            process_smtp_command(session, puf);

            // if chunking is enabled and we have data after BDAT <len>
            // then process the rest

            if(session->cfg->enable_chunking == 1 && p && session->protocol_state == SMTP_STATE_BDAT){
               process_bdat(session, p, strlen(p));
               break;
            }
         }
         else {
            snprintf(session->buf, SMALLBUFSIZE-1, "%s", puf);
            session->buflen = strlen(puf);
         }
      } while(p);

   }

}
