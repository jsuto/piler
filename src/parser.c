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


struct _state parse_message(struct session_data *sdata, int take_into_pieces, struct __config *cfg){
   FILE *f;
   int i, len;
   char *p, buf[MAXBUFSIZE], puf[SMALLBUFSIZE];
   char writebuffer[MAXBUFSIZE], abuffer[MAXBUFSIZE];
   struct _state state;

   init_state(&state);

   f = fopen(sdata->filename, "r");
   if(!f){
      syslog(LOG_PRIORITY, "%s: cannot open", sdata->ttmpfile);
      return state;
   }


   if(sdata->num_of_rcpt_to > 0){
      for(i=0; i<sdata->num_of_rcpt_to; i++){
         extractEmail(sdata->rcptto[i], puf);

         if(strlen(puf) > 5){
            p = strstr(puf, cfg->hostid);
            if(p && *(p-1) == '.'){

               *(p-1) = ' ';
               *p = '\0';

               len = strlen(puf);

               if(does_it_seem_like_an_email_address(puf) == 1){

                  if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: processing rcpt to address: *%s*", sdata->ttmpfile, puf);

                  if(state.tolen < MAXBUFSIZE-len-1){
                     if(is_string_on_list(state.rcpt, puf) == 0){
                        append_list(&(state.rcpt), puf);
                        memcpy(&(state.b_to[state.tolen]), puf, len);
                        state.tolen += len;

                        if(state.tolen < MAXBUFSIZE-len-1){
                           split_email_address(puf);
                           memcpy(&(state.b_to[state.tolen]), puf, len);
                           state.tolen += len;
                        }
                     }
                  }

               }
            }
         }

      }
   }

   if(take_into_pieces == 1){
      state.mfd = open(sdata->tmpframe, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
      if(state.mfd == -1){
         syslog(LOG_PRIORITY, "%s: cannot open frame file: %s", sdata->ttmpfile, sdata->tmpframe);
         fclose(f);
         return state;
      }
   }

   while(fgets(buf, sizeof(buf)-1, f)){
      parse_line(buf, &state, sdata, take_into_pieces, &writebuffer[0], sizeof(writebuffer), &abuffer[0], sizeof(abuffer), cfg);
   }

   if(take_into_pieces == 1 && state.writebufpos > 0){
      if(state.ms_journal == 1) remove_trailing_journal_boundary(&writebuffer[0], &state);

      len = write(state.mfd, writebuffer, state.writebufpos);
      memset(writebuffer, 0, sizeof(writebuffer));
      state.writebufpos = 0;
   }

   if(take_into_pieces == 1){
      close(state.mfd); state.mfd = 0;
   }

   fclose(f);

   return state;
}


void post_parse(struct session_data *sdata, struct _state *state, struct __config *cfg){
   int i, len, rec=0;
   char *p;

   free_list(state->boundaries);
   free_list(state->rcpt);
   free_list(state->rcpt_domain);

   trimBuffer(state->b_subject);
   fixupEncodedHeaderLine(state->b_subject);


   if(sdata->internal_sender == 0) sdata->direction = DIRECTION_INCOMING;
   else {
      if(sdata->internal_recipient == 1) sdata->direction = DIRECTION_INTERNAL;
      if(sdata->external_recipient == 1) sdata->direction = DIRECTION_OUTGOING;
      if(sdata->internal_recipient == 1 && sdata->external_recipient == 1) sdata->direction = DIRECTION_INTERNAL_AND_OUTGOING;
   }


   for(i=1; i<=state->n_attachments; i++){
      digest_file(state->attachments[i].internalname, &(state->attachments[i].digest[0]));
      fixupEncodedHeaderLine(state->attachments[i].filename);

      if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: attachment list: i:%d, name=*%s*, type: *%s*, size: %d, int.name: %s, digest: %s", sdata->ttmpfile, i, state->attachments[i].filename, state->attachments[i].type, state->attachments[i].size, state->attachments[i].internalname, state->attachments[i].digest);

      p = determine_attachment_type(state->attachments[i].filename, state->attachments[i].type);
      len = strlen(p);
      if(strlen(sdata->attachments) < SMALLBUFSIZE-len-1 && !strstr(sdata->attachments, p)) memcpy(&(sdata->attachments[strlen(sdata->attachments)]), p, len);

      if(state->attachments[i].dumped == 1){
         rec = 0;
         if(state->bodylen < BIGBUFSIZE-1024) extract_attachment_content(sdata, state, state->attachments[i].aname, get_attachment_extractor_by_filename(state->attachments[i].filename), &rec);
      
         unlink(state->attachments[i].aname);
      }

   }


   if(state->message_id[0] == 0){
      if(cfg->archive_emails_not_having_message_id == 1)
         snprintf(state->message_id, SMALLBUFSIZE-1, "%s", sdata->ttmpfile);
      else snprintf(state->message_id, SMALLBUFSIZE-1, "null");
   }

}


int parse_line(char *buf, struct _state *state, struct session_data *sdata, int take_into_pieces, char *writebuffer, int writebuffersize, char *abuffer, int abuffersize, struct __config *cfg){
   char *p, *q, puf[SMALLBUFSIZE];
   unsigned char b64buffer[MAXBUFSIZE];
   int x, n, n64, len, writelen, b64_len, boundary_line=0;

   if(cfg->debug == 1) printf("line: %s", buf);

   state->line_num++;
   len = strlen(buf);

   if(state->is_1st_header == 1 && (strncmp(buf, "Received: by piler", strlen("Received: by piler")) == 0 || strncmp(buf, "X-piler-id: ", strlen("X-piler-id: ")) == 0) ){
      sdata->restored_copy = 1;
   }

   if(state->is_1st_header == 1 && *(cfg->spam_header_line) != '\0' && strncmp(buf, cfg->spam_header_line, strlen(cfg->spam_header_line)) == 0){
      sdata->spam_message = 1;
   }

   if(state->is_1st_header == 1 && state->ms_journal == 0 && strncmp(buf, "X-MS-Journal-Report:", strlen("X-MS-Journal-Report:")) == 0){
      state->ms_journal = 1;
      memset(state->message_id, 0, SMALLBUFSIZE);

      memset(state->b_from, 0, SMALLBUFSIZE); 
      memset(state->b_from_domain, 0, SMALLBUFSIZE);
   }


   if(state->message_rfc822 == 0 && (buf[0] == '\r' || buf[0] == '\n') ){
      state->message_state = MSG_BODY;

      if(state->is_header == 1) state->is_header = 0;
      state->is_1st_header = 0;
   }

   if(state->ms_journal == 1 && strncasecmp(buf, "Received:", strlen("Received:")) == 0){
      state->is_1st_header = 1;
      state->is_header = 1;
      memset(state->b_body, 0, BIGBUFSIZE);
      state->bodylen = 0;
      memset(state->b_subject, 0, MAXBUFSIZE);
   }

   if(take_into_pieces == 1){
      if(state->message_state == MSG_BODY && state->fd != -1 && is_item_on_string(state->boundaries, buf) == 0){
         //n = write(state->fd, buf, len); // WRITE
         if(len + state->abufpos > abuffersize-1){
            n = write(state->fd, abuffer, state->abufpos); 

            if(state->b64fd != -1){
               abuffer[state->abufpos] = '\0';
               n64 = base64_decode_attachment_buffer(abuffer, state->abufpos, &b64buffer[0], sizeof(b64buffer));
               n64 = write(state->b64fd, b64buffer, n64);
            }

            state->abufpos = 0; memset(abuffer, 0, abuffersize);
         }
         memcpy(abuffer+state->abufpos, buf, len); state->abufpos += len;

         state->attachments[state->n_attachments].size += len;
      }
      else {
         state->saved_size += len;
         //n = write(state->mfd, buf, len); // WRITE
         if(len + state->writebufpos > writebuffersize-1){
            if(state->ms_journal == 1) remove_trailing_journal_boundary(writebuffer, state);
            n = write(state->mfd, writebuffer, state->writebufpos); state->writebufpos = 0; memset(writebuffer, 0, writebuffersize);
         }
         memcpy(writebuffer+state->writebufpos, buf, len); state->writebufpos += len;
      }
   }


   if(state->message_state == MSG_BODY && state->has_to_dump == 1 &&  state->pushed_pointer == 0){
      //printf("####name: %s, type: %s, base64: %d\n", state->filename, state->type, state->base64);

      state->pushed_pointer = 1;


      // this is a real attachment to dump
      if(state->base64 == 1 && strlen(state->filename) > 4 && strlen(state->type) > 3 && state->n_attachments < MAX_ATTACHMENTS-1){
         state->n_attachments++;

         snprintf(state->attachments[state->n_attachments].filename, TINYBUFSIZE-1, "%s", state->filename);
         snprintf(state->attachments[state->n_attachments].type, TINYBUFSIZE-1, "%s", state->type);
         snprintf(state->attachments[state->n_attachments].internalname, TINYBUFSIZE-1, "%s.a%d", sdata->ttmpfile, state->n_attachments);
         snprintf(state->attachments[state->n_attachments].aname, TINYBUFSIZE-1, "%s.a%d.bin", sdata->ttmpfile, state->n_attachments);

         //printf("DUMP FILE: %s\n", state->attachments[state->n_attachments].internalname);

         if(take_into_pieces == 1){
            state->fd = open(state->attachments[state->n_attachments].internalname, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);

            p = get_attachment_extractor_by_filename(state->attachments[state->n_attachments].filename);

            snprintf(state->attachments[state->n_attachments].shorttype, TINYBUFSIZE-1, "%s", p);
 
            if(strcmp("other", p)){
               state->b64fd = open(state->attachments[state->n_attachments].aname, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
               state->attachments[state->n_attachments].dumped = 1;
            }



            if(state->fd == -1){

               state->attachments[state->n_attachments].size = 0;
               state->attachments[state->n_attachments].dumped = 0;
               memset(state->attachments[state->n_attachments].type, 0, TINYBUFSIZE);
               memset(state->attachments[state->n_attachments].shorttype, 0, TINYBUFSIZE);
               memset(state->attachments[state->n_attachments].aname, 0, TINYBUFSIZE);
               memset(state->attachments[state->n_attachments].filename, 0, TINYBUFSIZE);
               memset(state->attachments[state->n_attachments].internalname, 0, TINYBUFSIZE);
               memset(state->attachments[state->n_attachments].digest, 0, 2*DIGEST_LENGTH+1);


               syslog(LOG_PRIORITY, "%s: error opening %s", sdata->ttmpfile, state->attachments[state->n_attachments].internalname);

               state->n_attachments--;
               state->has_to_dump = 0;

            }
            else {
               snprintf(puf, sizeof(puf)-1, "ATTACHMENT_POINTER_%s.a%d_XXX_PILER", sdata->ttmpfile, state->n_attachments);
               //n = write(state->mfd, puf, strlen(puf)); // WRITE
               writelen = strlen(puf);
               if(writelen + state->writebufpos > writebuffersize-1){
                  n = write(state->mfd, writebuffer, state->writebufpos); state->writebufpos = 0; memset(writebuffer, 0, writebuffersize);
               }
               memcpy(writebuffer+state->writebufpos, puf, writelen); state->writebufpos += writelen;
            }
         }

      }
      else {
         state->has_to_dump = 0;
      }

   }



   if(*buf == '.' && *(buf+1) == '.') buf++;

   /* undefined message state */
   if(state->is_header == 1 && buf[0] != ' ' && buf[0] != '\t' && strchr(buf, ':')) state->message_state = MSG_UNDEF;

   /* skip empty lines */

   if(state->message_rfc822 == 0 && (buf[0] == '\r' || buf[0] == '\n') ){
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

      if(strncasecmp(buf, "From:", strlen("From:")) == 0) state->message_state = MSG_FROM;
      else if(strncasecmp(buf, "To:", 3) == 0) state->message_state = MSG_TO;
      else if(strncasecmp(buf, "Cc:", 3) == 0) state->message_state = MSG_CC;
      else if(strncasecmp(buf, "Message-Id:", 11) == 0) state->message_state = MSG_MESSAGE_ID;
      else if(strncasecmp(buf, "References:", 11) == 0) state->message_state = MSG_REFERENCES;
      else if(strncasecmp(buf, "Subject:", strlen("Subject:")) == 0) state->message_state = MSG_SUBJECT;
      else if(strncasecmp(buf, "Recipient:", strlen("Recipient:")) == 0) state->message_state = MSG_RECIPIENT;
      else if(strncasecmp(buf, "Date:", strlen("Date:")) == 0 && sdata->sent == 0) sdata->sent = parse_date_header(buf);
      else if(strncasecmp(buf, "Received:", strlen("Received:")) == 0) state->message_state = MSG_RECEIVED;

      if(state->message_state == MSG_MESSAGE_ID && state->message_id[0] == 0){
         p = strchr(buf+11, ' ');
         if(p) p = buf + 12;
         else p = buf + 11;

         snprintf(state->message_id, SMALLBUFSIZE-1, "%s", p);
      }

      /* we are interested in only From:, To:, Subject:, Received:, Content-*: header lines */
      if(state->message_state <= 0) return 0;
   }


   if((p = strcasestr(buf, "boundary"))){
      x = extract_boundary(p, state);
   }


   if(state->message_state == MSG_BODY && state->ms_journal == 1 && strncasecmp(buf, "Recipient:", strlen("Recipient:")) == 0){
      state->is_header = 1;
      state->is_1st_header = 1;
      state->message_state = MSG_RECIPIENT;
   }

   if(state->message_state == MSG_RECIPIENT){
      p = strstr(buf, "Expanded:");
      if(p) *p = '\0';
   }

   if(state->message_state == MSG_RECEIVED && state->ms_journal == 1 && state->ms_journal_dropped == 0){
      state->ms_journal_dropped = 1;
      state->writebufpos = 0; memset(writebuffer, 0, writebuffersize);
      memcpy(writebuffer+state->writebufpos, buf, len); state->writebufpos += strlen(buf);
      memcpy(writebuffer+state->writebufpos, "\n", 1); state->writebufpos++;
   }


   if(state->is_1st_header == 1 && state->message_state == MSG_REFERENCES){
      if(strncasecmp(buf, "References:", 11) == 0) parse_reference(state, buf+11);
      else parse_reference(state, buf);
   }


   if(state->is_1st_header == 1 && state->message_state == MSG_SUBJECT && strlen(state->b_subject) + strlen(buf) < MAXBUFSIZE-1){

      if(state->b_subject[0] == '\0'){
         strncat(state->b_subject, buf+strlen("Subject:"), MAXBUFSIZE-1);
      }
      else {

         p = strrchr(state->b_subject, ' ');
         if(p && ( strcasestr(p+1, "?Q?") || strcasestr(p+1, "?B?") ) ){
            strncat(state->b_subject, buf+1, MAXBUFSIZE-1);
         }
         else strncat(state->b_subject, buf, MAXBUFSIZE-1);

      }

   }

   if(state->is_1st_header == 1){
      fixupEncodedHeaderLine(buf);
   }


   /* Content-type: checking */

   if(state->message_state == MSG_CONTENT_TYPE){
      state->message_rfc822 = 0;

      /* extract Content type */

      p = strchr(buf, ':');
      if(p){
         p++;
         if(*p == ' ' || *p == '\t') p++;
         snprintf(state->type, TINYBUFSIZE-1, "%s", p);
         state->content_type_is_set = 1;
         p = strchr(state->type, ';');
         if(p) *p = '\0';
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

      /* switch (back) to header mode if we encounterd an attachment with "message/rfc822" content-type */

      if(strcasestr(buf, "message/rfc822")){
         state->message_rfc822 = 1;
         state->is_header = 1;
      }


      if(strcasestr(buf, "charset") && strcasestr(buf, "UTF-8")) state->utf8 = 1;
   }


   if((state->message_state == MSG_CONTENT_TYPE || state->message_state == MSG_CONTENT_DISPOSITION) && strlen(state->filename) < 5){
      extractNameFromHeaderLine(buf, "name", state->filename);
   }


   if(state->message_state == MSG_CONTENT_TRANSFER_ENCODING){
      if(strcasestr(buf, "base64")) state->base64 = 1;
      if(strcasestr(buf, "quoted-printable")) state->qp = 1;
   }



   /* boundary check, and reset variables */

   boundary_line = is_item_on_string(state->boundaries, buf);

   if(!strstr(buf, "boundary=") && !strstr(buf, "boundary =") && boundary_line == 1){
      state->content_type_is_set = 0;

      if(state->has_to_dump == 1){
         if(take_into_pieces == 1 && state->fd != -1){
            if(state->abufpos > 0){
               n = write(state->fd, abuffer, state->abufpos);

               if(state->b64fd != -1){
                  abuffer[state->abufpos] = '\0';
                  n64 = base64_decode_attachment_buffer(abuffer, state->abufpos, &b64buffer[0], sizeof(b64buffer));
                  n64 = write(state->b64fd, b64buffer, n64);
               }

               state->abufpos = 0; memset(abuffer, 0, abuffersize); 
            }
            close(state->fd);
            close(state->b64fd);
         }
         state->fd = -1;
         state->b64fd = -1;
      }


      state->has_to_dump = 1;

      state->base64 = 0; state->textplain = 0; state->texthtml = state->octetstream = 0;
      state->skip_html = 0;
      state->utf8 = 0;
      state->qp = 0;

      state->realbinary = 0;

      state->pushed_pointer = 0;

      memset(state->filename, 0, TINYBUFSIZE);
      memset(state->type, 0, TINYBUFSIZE);

      state->message_state = MSG_UNDEF;

      return 0;      
   }

   if(boundary_line == 1){ return 0; }


   /* end of boundary check */


   /* skip irrelevant headers */
   if(state->is_header == 1 && state->message_state != MSG_FROM && state->message_state != MSG_TO && state->message_state != MSG_CC && state->message_state != MSG_RECIPIENT) return 0;


   /* don't process body if it's not a text or html part */
   if(state->message_state == MSG_BODY && state->textplain == 0 && state->texthtml == 0) return 0;


   if(state->base64 == 1 && state->message_state == MSG_BODY){
      b64_len = decodeBase64(buf);
      fixupBase64EncodedLine(buf, state);
   }


   /* remove all HTML tags */
   if(state->texthtml == 1 && state->message_state == MSG_BODY) markHTML(buf, state);

   if(state->message_state == MSG_BODY && state->qp == 1){
      fixupSoftBreakInQuotedPritableLine(buf, state); // 2011.12.07
      decodeQP(buf);
   }

   /* I believe that we can live without this function call */
   //decodeURL(buf);

   if(state->texthtml == 1) decodeHTML(buf);

   /* encode the body if it's not utf-8 encoded */
   if(state->message_state == MSG_BODY && state->utf8 != 1) utf8_encode((unsigned char*)buf);


   translateLine((unsigned char*)buf, state);

   reassembleToken(buf);


   if(state->is_header == 1) p = strchr(buf, ' ');
   else p = buf;

   //printf("a: %d/%d/%d/%d %s\n", state->is_1st_header, state->is_header, state->message_rfc822, state->message_state, buf);

   do {
      memset(puf, 0, sizeof(puf));
      p = split(p, ' ', puf, sizeof(puf)-1);

      if(puf[0] == '\0') continue;

      degenerateToken((unsigned char*)puf);

      if(puf[0] == '\0') continue;

      strncat(puf, " ", sizeof(puf)-1);

      if(strncasecmp(puf, "http://", 7) == 0 || strncasecmp(puf, "https://", 8) == 0) fixURL(puf);

      if(state->is_header == 0 && strncmp(puf, "__URL__", 7) && (puf[0] == ' ' || strlen(puf) > MAX_WORD_LEN || isHexNumber(puf)) ) continue;


      len = strlen(puf);


      if(state->message_state == MSG_FROM && state->is_1st_header == 1 && strlen(state->b_from) < SMALLBUFSIZE-len-1){
         memcpy(&(state->b_from[strlen(state->b_from)]), puf, len);

         if(does_it_seem_like_an_email_address(puf) == 1){
            q = strchr(puf, '@');
            if(q) memcpy(&(state->b_from_domain[strlen(state->b_from_domain)]), q+1, len);

            if(is_email_address_on_my_domains(puf, cfg) == 1) sdata->internal_sender = 1;

            if(strlen(state->b_from) < SMALLBUFSIZE-len-1){
               split_email_address(puf);
               memcpy(&(state->b_from[strlen(state->b_from)]), puf, len);
            }
         }
      }
      else if((state->message_state == MSG_TO || state->message_state == MSG_CC || state->message_state == MSG_RECIPIENT) && state->is_1st_header == 1 && state->tolen < MAXBUFSIZE-len-1){

         if(is_string_on_list(state->rcpt, puf) == 0){
            append_list(&(state->rcpt), puf);
            memcpy(&(state->b_to[state->tolen]), puf, len);
            state->tolen += len;

            if(does_it_seem_like_an_email_address(puf) == 1){
               if(is_email_address_on_my_domains(puf, cfg) == 1) sdata->internal_recipient = 1;
               else sdata->external_recipient = 1;

               q = strchr(puf, '@');
               if(q){
                  if(is_string_on_list(state->rcpt_domain, q+1) == 0){
                     append_list(&(state->rcpt_domain), q+1);
                     memcpy(&(state->b_to_domain[strlen(state->b_to_domain)]), q+1, strlen(q+1));
                  }
               }

               if(state->tolen < MAXBUFSIZE-len-1){
                  split_email_address(puf);
                  memcpy(&(state->b_to[state->tolen]), puf, len);
                  state->tolen += len;
               }

            }
         }
      }
      else if(state->message_state == MSG_BODY && len >= cfg->min_word_len && state->bodylen < BIGBUFSIZE-len-1){
         memcpy(&(state->b_body[state->bodylen]), puf, len);
         state->bodylen += len;
      }

   } while(p);

   return 0;
}

