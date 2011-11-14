/*
 * parser.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <piler.h>


struct _state parseMessage(struct session_data *sdata, struct __config *cfg){
   FILE *f;
   char buf[MAXBUFSIZE];
   struct _state state;

   initState(&state);

   f = fopen(sdata->ttmpfile, "r");
   if(!f){
      syslog(LOG_PRIORITY, "%s: cannot open", sdata->ttmpfile);
      return state;
   }

   while(fgets(buf, MAXBUFSIZE-1, f)){
      parseLine(buf, &state, sdata, cfg);
   }

   fclose(f);

   free_boundary(state.boundaries);

   if(state.message_id[0] == 0) snprintf(state.message_id, SMALLBUFSIZE-1, "null");

   if(state.b_from[strlen(state.b_from)-1] == ' ') state.b_from[strlen(state.b_from)-1] = '\0';
   if(state.b_to[strlen(state.b_to)-1] == ' ') state.b_to[strlen(state.b_to)-1] = '\0';
   if(state.b_subject[strlen(state.b_subject)-1] == ' ') state.b_subject[strlen(state.b_subject)-1] = '\0';

   make_body_digest(sdata);

   syslog(LOG_PRIORITY, "%s: from=%s, to=%s, subj=%s, message-id=%s", sdata->ttmpfile, state.b_from, state.b_to, state.b_subject, state.message_id);

   return state;
}


int parseLine(char *buf, struct _state *state, struct session_data *sdata, struct __config *cfg){
   char *p, *q, puf[MAXBUFSIZE], muf[MAXBUFSIZE], u[SMALLBUFSIZE], token[MAX_TOKEN_LEN];
   int x, b64_len, boundary_line=0;

   memset(token, 0, MAX_TOKEN_LEN);

   state->line_num++;

   if(*buf == '.' && *(buf+1) == '.') buf++;

   /* undefined message state */
   if(state->is_header == 1 && buf[0] != ' ' && buf[0] != '\t' && strchr(buf, ':')) state->message_state = MSG_UNDEF;

   /* skip empty lines */

   if(state->message_rfc822 == 0 && (buf[0] == '\r' || buf[0] == '\n') ){
      state->message_state = MSG_BODY;

      if(state->is_header == 1) state->is_header = 0;

      return 0;
   }


   trimBuffer(buf);


   /* skip the first line, if it's a "From <email address> date" format */
   if(state->line_num == 1 && strncmp(buf, "From ", 5) == 0) return 0;


   if(state->is_header == 0 && buf[0] != ' ' && buf[0] != '\t') state->message_state = MSG_BODY;

   if((state->content_type_is_set == 0 || state->is_header == 1) && strncasecmp(buf, "Content-Type:", strlen("Content-Type:")) == 0) state->message_state = MSG_CONTENT_TYPE;
   else if(strncasecmp(buf, "Content-Transfer-Encoding:", strlen("Content-Transfer-Encoding:")) == 0) state->message_state = MSG_CONTENT_TRANSFER_ENCODING;
   else if(strncasecmp(buf, "Content-Disposition:", strlen("Content-Disposition:")) == 0) state->message_state = MSG_CONTENT_DISPOSITION;


   if(state->message_state == MSG_CONTENT_TYPE || state->message_state == MSG_CONTENT_TRANSFER_ENCODING) state->is_header = 1;


   /* header checks */

   if(state->is_header == 1){

      if(strncasecmp(buf, "Received: from ", strlen("Received: from ")) == 0) state->message_state = MSG_RECEIVED;
      else if(strncasecmp(buf, "From:", strlen("From:")) == 0) state->message_state = MSG_FROM;
      else if(strncasecmp(buf, "To:", 3) == 0) state->message_state = MSG_TO;
      else if(strncasecmp(buf, "Cc:", 3) == 0) state->message_state = MSG_CC;
      else if(strncasecmp(buf, "Message-Id:", 11) == 0) state->message_state = MSG_MESSAGE_ID;
      else if(strncasecmp(buf, "Subject:", strlen("Subject:")) == 0) state->message_state = MSG_SUBJECT;
      else if(strncasecmp(buf, "Date:", strlen("Date:")) == 0 && sdata->sent == 0) sdata->sent = parse_date_header(buf);

      if(state->message_state == MSG_SUBJECT){
         p = &buf[0];
         if(strncmp(buf, "Subject:", strlen("Subject:")) == 0) p = &buf[8];
         if(*p == ' ') p++;
      }

      if(state->message_state == MSG_MESSAGE_ID && state->message_id[0] == 0){
         p = strchr(buf+11, ' ');
         if(p) p = buf + 12;
         else p = buf + 11;

         snprintf(state->message_id, SMALLBUFSIZE-1, "%s", p);
      }

      if(state->message_state == MSG_FROM){
         p = strchr(buf+5, ' ');
         if(p) p = buf + 6;
         else p = buf + 5;

         snprintf(state->from, SMALLBUFSIZE-1, "FROM*%s", p);
      }


      /* we are interested in only From:, To:, Subject:, Received:, Content-*: header lines */
      if(state->message_state <= 0) return 0;
   }




   if((p = strcasestr(buf, "boundary"))){
      x = extract_boundary(p, state);
   }


   /* Content-type: checking */

   if(state->message_state == MSG_CONTENT_TYPE){
      state->message_rfc822 = 0;

      /* extract Content type */

      p = strchr(buf, ':');
      if(p){
         p++;
         if(*p == ' ' || *p == '\t') p++;
         snprintf(state->attachments[state->n_attachments].type, SMALLBUFSIZE-1, "%s", p);
         state->content_type_is_set = 1;
         p = strchr(state->attachments[state->n_attachments].type, ';');
         if(p) *p = '\0';
      }

      p = strstr(buf, "name=");
      if(p){
         snprintf(state->attachments[state->n_attachments].filename, SMALLBUFSIZE-1, "%s", p);
      }


      if(strcasestr(buf, "text/plain") ||
         strcasestr(buf, "multipart/mixed") ||
         strcasestr(buf, "multipart/alternative") ||
         strcasestr(buf, "multipart/report") ||
         strcasestr(buf, "message/delivery-status") ||
         strcasestr(buf, "text/rfc822-headers") ||
         strcasestr(buf, "message/rfc822") ||
         strcasestr(buf, "application/ms-tnef")
      ){

             state->textplain = 1;
      }
      else if(strcasestr(buf, "text/html")){
             state->texthtml = 1;
      }

      /* switch (back) to header mode if we encounterd an attachment with
         "message/rfc822" content-type, 2010.05.16, SJ */

      if(strcasestr(buf, "message/rfc822")){
         state->message_rfc822 = 1;
         state->is_header = 1;
      }


      if(strcasestr(buf, "application/octet-stream")) state->octetstream = 1;

      if(strcasestr(buf, "charset") && strcasestr(buf, "UTF-8")) state->utf8 = 1;

      extractNameFromHeaderLine(buf, "name", state->attachments[state->n_attachments].filename);

      /*if(strlen(state->attachments[state->n_attachments].filename) > 5){
         state->has_to_dump = 1;
         snprintf(u, sizeof(u)-1, "%s.%d", sdata->ttmpfile, state->n_attachments);
         state->fd = open(u, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
      }*/

   }


   if(state->message_state == MSG_CONTENT_DISPOSITION && state->attachments[state->n_attachments].filename[0] == 0)
      extractNameFromHeaderLine(buf, "name", state->attachments[state->n_attachments].filename);


   if(state->message_state > 0 && state->message_state <= MSG_SUBJECT && state->message_rfc822 == 1) state->message_rfc822 = 0;


   /* check for textual base64 encoded part, 2005.03.25, SJ */

   if(state->message_state == MSG_CONTENT_TRANSFER_ENCODING){

      if(strcasestr(buf, "base64")){
         state->base64 = 1;
         state->has_base64 = 1;
      }

      if(strcasestr(buf, "quoted-printable")) state->qp = 1;


      if(strcasestr(buf, "image"))
         state->num_of_images++;

      if(strcasestr(buf, "msword"))
         state->num_of_msword++;
   }


   /* skip the boundary itself */

   boundary_line = is_boundary(state->boundaries, buf);

   if(!strstr(buf, "boundary=") && !strstr(buf, "boundary =") && boundary_line == 1){
      state->content_type_is_set = 0;

      /*if(state->has_to_dump == 1){
         if(state->fd != -1) close(state->fd);
         state->fd = -1;
      }*/

      if(state->n_attachments < MAX_ATTACHMENTS-1) state->n_attachments++;

      state->has_to_dump = 0;

      state->base64 = 0; state->textplain = 0; state->texthtml = state->octetstream = 0;
      state->skip_html = 0;
      state->utf8 = 0;
      state->qp = 0;

      state->realbinary = 0;

      return 0;      
   }

   if(boundary_line == 1){ return 0; }


   /* end of boundary check */


   /* skip non textual stuff */

   if(state->message_state == MSG_BODY){
      /*if(state->has_to_dump == 1 && state->fd != -1){
         write(state->fd, buf, strlen(buf));
      }*/

      if(state->base64 == 1) state->attachments[state->n_attachments].size += strlen(buf) / BASE64_RATIO;
      else state->attachments[state->n_attachments].size += strlen(buf);
   }


   if(state->message_state == MSG_BODY && strlen(buf) < 2) return 0;


   /*
    * sometimes spammers screw up their junk messages, and
    * use "application/octet-stream" type for textual parts.
    * Now clapf checks whether the attachment is really
    * binary. If it has no non-printable characters in a
    * base64 encoded line, then let's tokenize it.
    *
    * Note: in this case we cannot expect fully compliant 
    * message part. However this case should be very rare
    * since legitim messages use proper mime types.
    *
    * 2010.10.23, SJ
    */

   if(state->message_state == MSG_BODY && state->realbinary == 0 && state->octetstream == 1){
      snprintf(puf, MAXBUFSIZE-1, "%s", buf);
      if(state->base64 == 1) decodeBase64(puf);
      if(state->qp == 1) decodeQP(puf);
      state->realbinary += countNonPrintableCharacters(puf);
   }


   if(state->is_header == 0 && state->textplain == 0 && state->texthtml == 0 && (state->message_state == MSG_BODY || state->message_state == MSG_CONTENT_DISPOSITION) && (state->octetstream == 0 || state->realbinary > 0) ) return 0;

 
   /* base64 decode buffer */

   if(state->base64 == 1 && state->message_state == MSG_BODY) b64_len = decodeBase64(buf);


   /* fix encoded From:, To: and Subject: lines, 2008.11.24, SJ */

   if(state->message_state == MSG_FROM || state->message_state == MSG_TO || state->message_state == MSG_CC || state->message_state == MSG_SUBJECT) fixupEncodedHeaderLine(buf);


   /* fix soft breaks with quoted-printable decoded stuff, 2006.03.01, SJ */

   if(state->qp == 1) fixupSoftBreakInQuotedPritableLine(buf, state);


   /* fix base64 stuff if the line does not end with a line break, 2006.03.01, SJ */

   if(state->base64 == 1 && state->message_state == MSG_BODY) fixupBase64EncodedLine(buf, state);

   if(state->texthtml == 1 && state->message_state == MSG_BODY) markHTML(buf, state);


   if(state->message_state == MSG_BODY){
      if(state->qp == 1)   decodeQP(buf);
      if(state->utf8 == 1) decodeUTF8(buf);
   }

   decodeURL(buf);

   if(state->texthtml == 1) decodeHTML(buf);



   translateLine((unsigned char*)buf, state);

   reassembleToken(buf);


   if(state->is_header == 1) p = strchr(buf, ' ');
   else p = buf;


   //if(strlen(buf) > 3) printf("%d original: %s\n", state->message_state, buf);


   do {
      p = split(p, DELIMITER, puf, MAXBUFSIZE-1);

      if(strcasestr(puf, "http://") || strcasestr(puf, "https://")){
         q = puf;
         do {
            q = split_str(q, "http://", u, SMALLBUFSIZE-1);

            if(u[strlen(u)-1] == '.') u[strlen(u)-1] = '\0';

            if(strlen(u) > 2 && strncasecmp(u, "www.w3.org", 10) && strchr(u, '.') ){

               snprintf(muf, MAXBUFSIZE-1, "http://%s", u);
               fixURL(muf);

               strncat(muf, " ", MAXBUFSIZE-1);
               strncat(state->b_body, muf, BIGBUFSIZE-1);

            }
         } while(q);

         continue;
      }


      if(state->message_state != MSG_SUBJECT && (strlen(puf) < MIN_WORD_LEN || (strlen(puf) > MAX_WORD_LEN && state->message_state != MSG_FROM && state->message_state != MSG_TO && state->message_state != MSG_CC) || isHexNumber(puf)))
         continue;

      if(strlen(puf) < 2 || strncmp(puf, "HTML*", 5) == 0) continue;

      if(state->message_state == MSG_CONTENT_TYPE && strncmp(puf, "content-type", 12) == 0) continue;
      if(state->message_state == MSG_CONTENT_DISPOSITION && strncmp(puf, "content-disposition", 19) == 0) continue;
      if(state->message_state == MSG_CONTENT_TRANSFER_ENCODING && strncmp(puf, "content-transfer-encoding", 25) == 0) continue;

      degenerateToken((unsigned char*)puf);

      strncat(puf, " ", MAXBUFSIZE-1);

      if(state->message_state == MSG_SUBJECT)
         strncat(state->b_subject, puf, MAXBUFSIZE-1);
      else if(state->message_state == MSG_FROM && strchr(puf, '@'))
         strncat(state->b_from, puf, SMALLBUFSIZE-1);
      else if(state->message_state == MSG_TO && strchr(puf, '@'))
         strncat(state->b_to, puf, SMALLBUFSIZE-1);
      else if(state->message_state == MSG_CC && strchr(puf, '@'))
         strncat(state->b_to, puf, SMALLBUFSIZE-1);
      else if(state->is_header == 0)
         strncat(state->b_body, puf, BIGBUFSIZE-1);

   } while(p);


   return 0;
}

