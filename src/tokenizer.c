/*
 * tokenizer.c, SJ
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

void tokenize(char *buf, struct parser_state *state, struct session_data *sdata, struct data *data, struct config *cfg){
   char *p, *q, puf[SMALLBUFSIZE];
   int result;
   unsigned int len;

   translateLine((unsigned char*)buf, state);

   reassembleToken(buf);


   p = buf;

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

      // skip body tokens if not an URL && (empty token || too long)
      if(state->is_header == 0 && strncmp(puf, "__URL__", 7) && (puf[0] == ' ' || (len > MAX_WORD_LEN && cfg->enable_cjk == 0)) ){
         continue;
      }

      char md5buf[2*DIGEST_LENGTH+2];

      if(state->message_state == MSG_FROM && state->is_1st_header == 1 && strlen(state->b_from) < SMALLBUFSIZE-len-1){
         strtolower(puf);

         q = strchr(puf, '@');
         if(q) fix_plus_sign_in_email_address(puf, &q, &len);

         memcpy(&(state->b_from[strlen(state->b_from)]), puf, len);

         if(len >= MIN_EMAIL_ADDRESS_LEN && does_it_seem_like_an_email_address(puf) == 1 && state->b_from_domain[0] == '\0'){
            if(q && strlen(q) > 5){
               memcpy(&(state->b_from_domain), q+1, strlen(q+1)-1);
               if(strstr(sdata->mailfrom, "<>")){
                  snprintf(sdata->fromemail, SMALLBUFSIZE-1, "%s", puf);
                  sdata->fromemail[len-1] = '\0';
               }
            }

            if(is_email_address_on_my_domains(puf, data) == 1) sdata->internal_sender = 1;

            if(len >= MAX_EMAIL_ADDRESS_SPHINX_LEN && strlen(state->b_from) < SMALLBUFSIZE-len-1){
               create_md5_from_email_address(puf, md5buf);
               memcpy(&(state->b_from[strlen(state->b_from)]), md5buf, strlen(md5buf));
            }

            if(strlen(state->b_from) < SMALLBUFSIZE-len-1){
               split_email_address(puf);
               memcpy(&(state->b_from[strlen(state->b_from)]), puf, len);
            }
         }
      }
      else if(state->message_state == MSG_SENDER && state->is_1st_header == 1 && strlen(state->b_sender) < SMALLBUFSIZE-len-1){
         strtolower(puf);

         q = strchr(puf, '@');
         if(q) fix_plus_sign_in_email_address(puf, &q, &len);

         memcpy(&(state->b_sender[strlen(state->b_sender)]), puf, len);

         if(len >= MIN_EMAIL_ADDRESS_LEN && does_it_seem_like_an_email_address(puf) == 1 && state->b_sender_domain[0] == '\0'){
            if(q && strlen(q) > 5){
               memcpy(&(state->b_sender_domain), q+1, strlen(q+1)-1);
            }

            if(len >= MAX_EMAIL_ADDRESS_SPHINX_LEN && strlen(state->b_sender) < SMALLBUFSIZE-len-1){
               create_md5_from_email_address(puf, md5buf);
               memcpy(&(state->b_sender[strlen(state->b_sender)]), md5buf, strlen(md5buf));
            }

            if(strlen(state->b_sender) < SMALLBUFSIZE-len-1){
               split_email_address(puf);
               memcpy(&(state->b_sender[strlen(state->b_sender)]), puf, len);
            }
         }
      }
      else if((state->message_state == MSG_TO || state->message_state == MSG_CC || state->message_state == MSG_RECIPIENT || state->message_state == MSG_ENVELOPE_TO) && state->is_1st_header == 1 && state->tolen < MAXBUFSIZE-len-1){
         strtolower(puf);

         /* fix aaa+bbb@ccc.fu address to aaa@ccc.fu, 2017.02.04, SJ */
         q = strchr(puf, '@');
         if(q) fix_plus_sign_in_email_address(puf, &q, &len);

         if((state->message_state == MSG_RECIPIENT || state->message_state == MSG_ENVELOPE_TO) && findnode(state->journal_recipient, puf) == NULL && state->journaltolen < sizeof(state->b_journal_to)-len-1){
            addnode(state->journal_recipient, puf);
            memcpy(&(state->b_journal_to[state->journaltolen]), puf, len);
            state->journaltolen += len;
            if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: journal rcpt: '%s'", sdata->ttmpfile, puf);
         }

         if(len >= MAX_EMAIL_ADDRESS_SPHINX_LEN){
            create_md5_from_email_address(puf, md5buf);
            add_recipient(md5buf, strlen(md5buf), sdata, state, data, cfg);
         }

         add_recipient(puf, len, sdata, state, data, cfg);
      }
      else if(state->message_state == MSG_BODY && len >= (unsigned int)(cfg->min_word_len) && state->bodylen < BIGBUFSIZE-len-1){
         // 99% of email addresses are longer than 8 characters
         if(len >= MIN_EMAIL_ADDRESS_LEN && does_it_seem_like_an_email_address(puf)){
            fix_email_address_for_sphinx(puf);
         }

         memcpy(&(state->b_body[state->bodylen]), puf, len);
         state->bodylen += len;
      }

   } while(p);

}
