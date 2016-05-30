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


struct parser_state parse_message(struct session_data *sdata, int take_into_pieces, struct __data *data, struct __config *cfg){
   FILE *f;
   int i;
   unsigned int len;
   char *p, buf[MAXBUFSIZE], puf[SMALLBUFSIZE];
   char writebuffer[MAXBUFSIZE], abuffer[MAXBUFSIZE];
   struct parser_state state;

   init_state(&state);

   f = fopen(sdata->filename, "r");
   if(!f){
      syslog(LOG_PRIORITY, "%s: cannot open", sdata->ttmpfile);
      return state;
   }


   if(sdata->num_of_rcpt_to > 0 && cfg->process_rcpt_to_addresses == 1){
      for(i=0; i<sdata->num_of_rcpt_to; i++){

         snprintf(puf, sizeof(puf)-1, "%s ", sdata->rcptto[i]);

         if(does_it_seem_like_an_email_address(puf) == 1){
            p = strstr(puf, cfg->hostid);
            if(!p){

               strtolower(puf);
               len = strlen(puf);

               if(state.tolen < MAXBUFSIZE-len-1){

                  if(findnode(state.rcpt, puf) == NULL){
                     addnode(state.journal_recipient, puf);
                     addnode(state.rcpt, puf);

                     memcpy(&(state.b_journal_to[state.journaltolen]), puf, len);
                     state.journaltolen += len;

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

   if(take_into_pieces == 1){
      state.mfd = open(sdata->tmpframe, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
      if(state.mfd == -1){
         syslog(LOG_PRIORITY, "%s: cannot open frame file: %s", sdata->ttmpfile, sdata->tmpframe);
         fclose(f);
         return state;
      }
   }

   while(fgets(buf, sizeof(buf)-1, f)){
      parse_line(buf, &state, sdata, take_into_pieces, &writebuffer[0], sizeof(writebuffer), &abuffer[0], sizeof(abuffer), data, cfg);
   }

   if(take_into_pieces == 1 && state.writebufpos > 0){
      write(state.mfd, writebuffer, state.writebufpos);
      memset(writebuffer, 0, sizeof(writebuffer));
      state.writebufpos = 0;
   }

   if(take_into_pieces == 1){
      close(state.mfd); state.mfd = 0;
   }

   fclose(f);

   return state;
}


void post_parse(struct session_data *sdata, struct parser_state *state, struct __config *cfg){
   int i, rec=0;
   unsigned int len;
   char *p;

   clearhash(state->boundaries);
   clearhash(state->rcpt);
   clearhash(state->rcpt_domain);
   clearhash(state->journal_recipient);

   trimBuffer(state->b_subject);

   if(sdata->internal_sender == 0) sdata->direction = DIRECTION_INCOMING;
   else {
      if(sdata->internal_recipient == 1) sdata->direction = DIRECTION_INTERNAL;
      if(sdata->external_recipient == 1) sdata->direction = DIRECTION_OUTGOING;
      if(sdata->internal_recipient == 1 && sdata->external_recipient == 1) sdata->direction = DIRECTION_INTERNAL_AND_OUTGOING;
   }


   for(i=1; i<=state->n_attachments; i++){
      digest_file(state->attachments[i].internalname, &(state->attachments[i].digest[0]));

      if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: attachment list: i:%d, name=*%s*, type: *%s*, size: %d, int.name: %s, digest: %s", sdata->ttmpfile, i, state->attachments[i].filename, state->attachments[i].type, state->attachments[i].size, state->attachments[i].internalname, state->attachments[i].digest);

      p = determine_attachment_type(state->attachments[i].filename, state->attachments[i].type);
      len = strlen(p);
      if(strlen(sdata->attachments) < SMALLBUFSIZE-len-1 && !strstr(sdata->attachments, p)) memcpy(&(sdata->attachments[strlen(sdata->attachments)]), p, len);

      if(state->attachments[i].dumped == 1){
         rec = 0;
         if(cfg->extract_attachments == 1 && state->bodylen < BIGBUFSIZE-1024) extract_attachment_content(sdata, state, state->attachments[i].aname, get_attachment_extractor_by_filename(state->attachments[i].filename), &rec, cfg);
      
         unlink(state->attachments[i].aname);
      }

   }


   if(state->message_id[0] == 0){
      if(cfg->archive_emails_not_having_message_id == 1)
         snprintf(state->message_id, SMALLBUFSIZE-1, "%s", sdata->ttmpfile);
      else snprintf(state->message_id, SMALLBUFSIZE-1, "null");
   }


   digest_string(state->message_id, &(state->message_id_hash[0]));

   if(sdata->sent == 0) sdata->sent = sdata->now;
}


void storno_attachment(struct parser_state *state){
   state->has_to_dump = 0;

   if(state->n_attachments <= 0) return;

   state->attachments[state->n_attachments].size = 0;
   state->attachments[state->n_attachments].dumped = 0;

   memset(state->attachments[state->n_attachments].type, 0, TINYBUFSIZE);
   memset(state->attachments[state->n_attachments].shorttype, 0, TINYBUFSIZE);
   memset(state->attachments[state->n_attachments].aname, 0, TINYBUFSIZE);
   memset(state->attachments[state->n_attachments].filename, 0, TINYBUFSIZE);
   memset(state->attachments[state->n_attachments].internalname, 0, TINYBUFSIZE);
   memset(state->attachments[state->n_attachments].digest, 0, 2*DIGEST_LENGTH+1);


   state->n_attachments--;
}


int parse_line(char *buf, struct parser_state *state, struct session_data *sdata, int take_into_pieces, char *writebuffer, int writebuffersize, char *abuffer, int abuffersize, struct __data *data, struct __config *cfg){
   char *p, *q, puf[SMALLBUFSIZE];
   unsigned char b64buffer[MAXBUFSIZE];
   char tmpbuf[MAXBUFSIZE];
   int n64, writelen, boundary_line=0, result;
   unsigned int len;

   if(cfg->debug == 1) printf("line: %s", buf);

   state->line_num++;
   len = strlen(buf);

   /*
    * check a few things in the 1st header
    */

   if(state->is_1st_header == 1){

      if(strncmp(buf, "Received: by piler", strlen("Received: by piler")) == 0){
         sdata->restored_copy = 1;
      }

      if(*(cfg->piler_header_field) != 0 && strncmp(buf, cfg->piler_header_field, strlen(cfg->piler_header_field)) == 0){
         sdata->restored_copy = 1;
      }

      if(sdata->ms_journal == 0 && strncmp(buf, "X-MS-Journal-Report:", strlen("X-MS-Journal-Report:")) == 0){
         //if(sdata->import == 0){
            sdata->ms_journal = 1;
            memset(state->message_id, 0, SMALLBUFSIZE);
         //}
      }

   }


   if(state->message_rfc822 == 0 && (buf[0] == '\r' || buf[0] == '\n') ){
      state->message_state = MSG_BODY;

      if(state->is_header == 1) state->is_header = 0;
      state->is_1st_header = 0;

      if(state->anamepos > 0){
         extractNameFromHeaderLine(state->attachment_name_buf, "name", state->filename);
      }

   }


   if(take_into_pieces == 1){
      if(state->message_state == MSG_BODY && state->fd != -1 && is_substr_in_hash(state->boundaries, buf) == 0){
         //n = write(state->fd, buf, len); // WRITE
         if(len + state->abufpos > abuffersize-1){
            write(state->fd, abuffer, state->abufpos); 

            if(state->b64fd != -1){
               abuffer[state->abufpos] = '\0';
               if(state->base64 == 1){
                  n64 = base64_decode_attachment_buffer(abuffer, &b64buffer[0], sizeof(b64buffer));
                  write(state->b64fd, b64buffer, n64);
               }
               else {
                  write(state->b64fd, abuffer, state->abufpos);
               }
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
            write(state->mfd, writebuffer, state->writebufpos); state->writebufpos = 0; memset(writebuffer, 0, writebuffersize);
         }
         memcpy(writebuffer+state->writebufpos, buf, len); state->writebufpos += len;
      }
   }


   if(state->message_state == MSG_BODY && state->has_to_dump == 1 &&  state->pushed_pointer == 0){
      //printf("####name: %s, type: %s, base64: %d\n", state->filename, state->type, state->base64);

      state->pushed_pointer = 1;


      // this is a real attachment to dump, it doesn't have to be base64 encoded!
      if(strlen(state->filename) > 4 && strlen(state->type) > 3 && state->n_attachments < MAX_ATTACHMENTS-1){
         state->n_attachments++;

         snprintf(state->attachments[state->n_attachments].filename, TINYBUFSIZE-1, "%s", state->filename);
         snprintf(state->attachments[state->n_attachments].type, TINYBUFSIZE-1, "%s", state->type);
         snprintf(state->attachments[state->n_attachments].internalname, TINYBUFSIZE-1, "%s.a%d", sdata->ttmpfile, state->n_attachments);
         snprintf(state->attachments[state->n_attachments].aname, TINYBUFSIZE-1, "%s.a%d.bin", sdata->ttmpfile, state->n_attachments);

         //printf("DUMP FILE: %s\n", state->attachments[state->n_attachments].internalname);

         if(take_into_pieces == 1){
            state->fd = open(state->attachments[state->n_attachments].internalname, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);

            fixupEncodedHeaderLine(state->attachments[state->n_attachments].filename, TINYBUFSIZE);

            p = get_attachment_extractor_by_filename(state->attachments[state->n_attachments].filename);

            snprintf(state->attachments[state->n_attachments].shorttype, TINYBUFSIZE-1, "%s", p);
 
            if(strcmp("other", p)){
               state->b64fd = open(state->attachments[state->n_attachments].aname, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
               state->attachments[state->n_attachments].dumped = 1;
            }



            if(state->fd == -1){
               storno_attachment(state);
               syslog(LOG_PRIORITY, "%s: error opening %s", sdata->ttmpfile, state->attachments[state->n_attachments].internalname);
            }
            else {
               snprintf(puf, sizeof(puf)-1, "ATTACHMENT_POINTER_%s.a%d_XXX_PILER", sdata->ttmpfile, state->n_attachments);
               //n = write(state->mfd, puf, strlen(puf)); // WRITE
               writelen = strlen(puf);
               if(writelen + state->writebufpos > writebuffersize-1){
                  write(state->mfd, writebuffer, state->writebufpos); state->writebufpos = 0; memset(writebuffer, 0, writebuffersize);
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


   // journal fix

   if(state->message_state == MSG_BODY && sdata->ms_journal == 1){
      state->is_header = 1;
      state->is_1st_header = 1;
   }


   /* header checks */

   if(state->is_header == 1){

      if(*(cfg->spam_header_line) != '\0' && strncmp(buf, cfg->spam_header_line, strlen(cfg->spam_header_line)) == 0){
         sdata->spam_message = 1;
      }

      if(strncasecmp(buf, "From:", strlen("From:")) == 0) state->message_state = MSG_FROM;

      else if(strncasecmp(buf, "Content-Type:", strlen("Content-Type:")) == 0){
         state->message_state = MSG_CONTENT_TYPE;

         if(state->anamepos > 0){
            extractNameFromHeaderLine(state->attachment_name_buf, "name", state->filename);
            memset(state->attachment_name_buf, 0, SMALLBUFSIZE);
            state->anamepos = 0;
         }

      }
      else if(strncasecmp(buf, "Content-Transfer-Encoding:", strlen("Content-Transfer-Encoding:")) == 0) state->message_state = MSG_CONTENT_TRANSFER_ENCODING;
      else if(strncasecmp(buf, "Content-Disposition:", strlen("Content-Disposition:")) == 0){
         state->message_state = MSG_CONTENT_DISPOSITION;

         if(state->anamepos > 0){
            extractNameFromHeaderLine(state->attachment_name_buf, "name", state->filename);
            memset(state->attachment_name_buf, 0, SMALLBUFSIZE);
            state->anamepos = 0;
         }

      }
      else if(strncasecmp(buf, "To:", 3) == 0) state->message_state = MSG_TO;
      else if(strncasecmp(buf, "Cc:", 3) == 0) state->message_state = MSG_CC;
      else if(strncasecmp(buf, "Bcc:", 4) == 0) state->message_state = MSG_CC;
      else if(strncasecmp(buf, "Message-Id:", 11) == 0) state->message_state = MSG_MESSAGE_ID;
      else if(strncasecmp(buf, "References:", 11) == 0) state->message_state = MSG_REFERENCES;
      else if(strncasecmp(buf, "Subject:", strlen("Subject:")) == 0) state->message_state = MSG_SUBJECT;
      else if(strncasecmp(buf, "Recipient:", strlen("Recipient:")) == 0) state->message_state = MSG_RECIPIENT;

      if(sdata->ms_journal == 1 && (state->message_state == MSG_TO || state->message_state == MSG_RECIPIENT) ){
         p = strstr(buf, "Expanded:");
         if(p) *p = '\0';
      }


      /*
       * by default sdata->sent = 0, and let the parser extract value from the Date: header
       */

      else if(strncasecmp(buf, "Date:", strlen("Date:")) == 0 && state->is_1st_header == 1 && sdata->sent == 0){

         if(strstr(buf, "=?") && strstr(buf, "?=")) fixupEncodedHeaderLine(buf, MAXBUFSIZE);

         sdata->sent = parse_date_header(buf);

         /* allow +2 days drift in the parsed Date: value */

         if(sdata->sent - sdata->now > 2*86400) sdata->sent = sdata->now;
      }

      else if(strncasecmp(buf, "Delivery-date:", strlen("Delivery-date:")) == 0 && sdata->delivered == 0) sdata->delivered = parse_date_header(buf);
      else if(strncasecmp(buf, "Received:", strlen("Received:")) == 0) state->message_state = MSG_RECEIVED;
      else if(cfg->extra_to_field[0] != '\0' && strncasecmp(buf, cfg->extra_to_field, strlen(cfg->extra_to_field)) == 0) state->message_state = MSG_TO;

      if(state->message_state == MSG_MESSAGE_ID && state->message_id[0] == 0){
         p = strchr(buf+11, ' ');
         if(p) p = buf + 12;
         else p = buf + 11;

         snprintf(state->message_id, SMALLBUFSIZE-1, "%s", p);
      }

      /* we are interested in only From:, To:, Subject:, Received:, Content-*: header lines */
      if(state->message_state <= 0) return 0;
   }


   if(state->message_state == MSG_CONTENT_TYPE){
      if((p = strcasestr(buf, "boundary"))){
         extract_boundary(p, state);
      }
   }


   /* 
    * A normal journal looks like this:
    *
    *   Sender: sender@domain
    *   Subject: Test normal
    *   Message-Id: ...
    *   Recipient: user1@domain
    *   Recipient: user2@domain, Forwarded: user1@domain
    *
    * However if outlook forwards an email, then the journal is somewhat changed:
    *
    *   Sender: sender@domain
    *   Subject: Test through outlook
    *   Message-Id: ...
    *   To: user1@domain
    *   To: user2@domain, Forwarded: user1@domain
    *
    *
    * Outlook.com has the following scheme, when expanded from a distribution list:
    *
    *   Sender: sender@domain
    *   Subject: Test Email
    *   Message-Id: ...
    *   To: user1@domain, Expanded: listaddress@domain
    *   To: user2@domain, Expanded: listaddress@domain
    *
    */




   if(state->is_1st_header == 1 && state->message_state == MSG_REFERENCES){
      if(strncasecmp(buf, "References:", 11) == 0) parse_reference(state, buf+11);
      else parse_reference(state, buf);
   }


   if(state->is_1st_header == 1){

      if(state->message_state == MSG_SUBJECT && strlen(state->b_subject) + strlen(buf) < MAXBUFSIZE-1){

         if(state->b_subject[0] == '\0'){
            p = &buf[0];
            if(strncmp(buf, "Subject:", strlen("Subject:")) == 0) p += strlen("Subject:");
            if(*p == ' ') p++;

            fixupEncodedHeaderLine(p, MAXBUFSIZE);
            strncat(state->b_subject, p, MAXBUFSIZE-strlen(state->b_subject)-1);
         }
         else {

            /*
             * if the next subject line is encoded, then strip the whitespace characters at the beginning of the line
             */

            p = buf;

            if(strcasestr(buf, "?Q?") || strcasestr(buf, "?B?")){
               while(isspace(*p)) p++;
            }

            fixupEncodedHeaderLine(p, MAXBUFSIZE);

            strncat(state->b_subject, p, MAXBUFSIZE-strlen(state->b_subject)-1);
         }
      }
      else { fixupEncodedHeaderLine(buf, MAXBUFSIZE); }
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
         //state->content_type_is_set = 1;
         p = strchr(state->type, ';');
         if(p) *p = '\0';
      }


      if(strcasestr(buf, "text/plain") ||
         strcasestr(buf, "multipart/mixed") ||
         strcasestr(buf, "multipart/alternative") ||
         strcasestr(buf, "multipart/report") ||
         strcasestr(buf, "message/delivery-status") ||
         strcasestr(buf, "text/rfc822-headers") ||
         strcasestr(buf, "message/rfc822")
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

         if(sdata->ms_journal == 1){
            state->is_1st_header = 1;

            // reset all headers, except To:

            memset(state->b_subject, 0, MAXBUFSIZE);
            memset(state->b_body, 0, BIGBUFSIZE);
            memset(state->b_from, 0, SMALLBUFSIZE);
            memset(state->b_from_domain, 0, SMALLBUFSIZE);
            memset(state->message_id, 0, SMALLBUFSIZE);

            sdata->ms_journal = 0;
         }
      }


      if(strcasestr(buf, "charset")) extractNameFromHeaderLine(buf, "charset", state->charset);
      if(strcasestr(state->charset, "UTF-8")) state->utf8 = 1;
   }


   if((state->message_state == MSG_CONTENT_TYPE || state->message_state == MSG_CONTENT_DISPOSITION) && strlen(state->filename) < 5){

      p = &buf[0];
      for(; *p; p++){
         if(*p != ' ' && *p != '\t') break;
      }

      len = strlen(p);

      if(len + state->anamepos < SMALLBUFSIZE-2){
         memcpy(&(state->attachment_name_buf[state->anamepos]), p, len);
         state->anamepos += len;
      }
   }


   if(state->message_state == MSG_CONTENT_TRANSFER_ENCODING){
      if(strcasestr(buf, "base64")) state->base64 = 1;
      if(strcasestr(buf, "quoted-printable")) state->qp = 1;
   }



   /* boundary check, and reset variables */

   boundary_line = is_substr_in_hash(state->boundaries, buf);


   if(!strstr(buf, "boundary=") && !strstr(buf, "boundary =") && boundary_line == 1){
      state->is_header = 1;

      //state->content_type_is_set = 0;

      if(state->has_to_dump == 1){
         if(take_into_pieces == 1 && state->fd != -1){
            if(state->abufpos > 0){
               write(state->fd, abuffer, state->abufpos);

               if(state->b64fd != -1){
                  abuffer[state->abufpos] = '\0';
                  if(state->base64 == 1){
                     n64 = base64_decode_attachment_buffer(abuffer, &b64buffer[0], sizeof(b64buffer));
                     write(state->b64fd, b64buffer, n64);
                  }
                  else {
                     write(state->b64fd, abuffer, state->abufpos);
                  }
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
      snprintf(state->charset, TINYBUFSIZE-1, "unknown");

      memset(state->attachment_name_buf, 0, SMALLBUFSIZE);
      state->anamepos = 0;

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
      decodeBase64(buf);
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

   if(state->texthtml == 1) decodeHTML(buf, state->utf8);

   /* encode the body if it's not utf-8 encoded */
   if(state->message_state == MSG_BODY && state->utf8 != 1){
      result = utf8_encode(buf, strlen(buf), &tmpbuf[0], sizeof(tmpbuf), state->charset);
      if(result == OK) snprintf(buf, MAXBUFSIZE-1, "%s", tmpbuf);
   }

   translateLine((unsigned char*)buf, state);

   reassembleToken(buf);


   if(state->is_header == 1) p = strchr(buf, ' ');
   else p = buf;

   //printf("a: %d/%d/%d/%d/j=%d %s\n", state->is_1st_header, state->is_header, state->message_rfc822, state->message_state, sdata->ms_journal, buf);

   do {
      memset(puf, 0, sizeof(puf));
      p = split(p, ' ', puf, sizeof(puf)-1, &result);

      if(puf[0] == '\0') continue;

      degenerateToken((unsigned char*)puf);

      if(puf[0] == '\0') continue;

      strncat(puf, " ", sizeof(puf)-strlen(puf)-1);

      if(strncasecmp(puf, "http://", 7) == 0 || strncasecmp(puf, "https://", 8) == 0) fixURL(puf, sizeof(puf)-1);

      len = strlen(puf);

      if(state->is_header == 0 && strncmp(puf, "__URL__", 7) && (puf[0] == ' ' || (len > MAX_WORD_LEN && cfg->enable_cjk == 0) || isHexNumber(puf)) ) continue;


      if(state->message_state == MSG_FROM && state->is_1st_header == 1 && strlen(state->b_from) < SMALLBUFSIZE-len-1){
         strtolower(puf);

         memcpy(&(state->b_from[strlen(state->b_from)]), puf, len);

         if(does_it_seem_like_an_email_address(puf) == 1 && state->b_from_domain[0] == '\0' && len > 5){
            q = strchr(puf, '@');
            if(q && strlen(q) > 5){
               memcpy(&(state->b_from_domain), q+1, strlen(q+1)-1);
               if(strstr(sdata->mailfrom, "<>")){
                  snprintf(sdata->fromemail, SMALLBUFSIZE-1, "%s", puf);
                  sdata->fromemail[len-1] = '\0';
               }
            }

            if(is_email_address_on_my_domains(puf, data) == 1) sdata->internal_sender = 1;

            if(strlen(state->b_from) < SMALLBUFSIZE-len-1){
               split_email_address(puf);
               memcpy(&(state->b_from[strlen(state->b_from)]), puf, len);
            }
         }
      }
      else if((state->message_state == MSG_TO || state->message_state == MSG_CC || state->message_state == MSG_RECIPIENT) && state->is_1st_header == 1 && state->tolen < MAXBUFSIZE-len-1){
         strtolower(puf);

         if(state->message_state == MSG_RECIPIENT && findnode(state->journal_recipient, puf) == NULL){
            addnode(state->journal_recipient, puf);
            memcpy(&(state->b_journal_to[state->journaltolen]), puf, len);
            if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: journal rcpt: '%s'", sdata->ttmpfile, puf);
         }


         if(findnode(state->rcpt, puf) == NULL){

            /* skip any address matching ...@cfg->hostid, 2013.10.29, SJ */
            q = strchr(puf, '@');
            if(q && strncmp(q+1, cfg->hostid, cfg->hostid_len) == 0){
               continue;
            }

            addnode(state->rcpt, puf);
            memcpy(&(state->b_to[state->tolen]), puf, len);
            state->tolen += len;

            if(does_it_seem_like_an_email_address(puf) == 1){
               if(is_email_address_on_my_domains(puf, data) == 1) sdata->internal_recipient = 1;
               else sdata->external_recipient = 1;

               //q = strchr(puf, '@');
               if(q){
                  if(findnode(state->rcpt_domain, q+1) == NULL){
                     addnode(state->rcpt_domain, q+1);
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

