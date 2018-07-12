/*
 * parser_utils.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <piler.h>
#include "trans.h"


void init_state(struct parser_state *state){
   int i;

   state->message_state = MSG_UNDEF;

   state->line_num = 0;

   state->is_header = 1;
   state->is_1st_header = 1;

   state->textplain = 1; /* by default we are a text/plain message */
   state->texthtml = 0;
   state->message_rfc822 = 0;

   state->base64 = 0;
   state->utf8 = 0;

   state->qp = 0;

   state->htmltag = 0;
   state->style = 0;

   state->skip_html = 0;

   state->content_type_is_set = 0;

   memset(state->message_id, 0, SMALLBUFSIZE);
   memset(state->message_id_hash, 0, 2*DIGEST_LENGTH+1);
   memset(state->miscbuf, 0, MAX_TOKEN_LEN);
   memset(state->qpbuf, 0, MAX_TOKEN_LEN);

   memset(state->type, 0, TINYBUFSIZE);

   memset(state->attachment_name_buf, 0, SMALLBUFSIZE);
   state->anamepos = 0;

   state->has_to_dump = 0;
   state->fd = -1;
   state->b64fd = -1;
   state->mfd = -1;
   state->realbinary = 0;
   state->octetstream = 0;
   state->pushed_pointer = 0;
   state->saved_size = 0;

   state->writebufpos = 0;
   state->abufpos = 0;

   inithash(state->boundaries);
   inithash(state->rcpt);
   inithash(state->rcpt_domain);
   inithash(state->journal_recipient);

   state->n_attachments = 0;

   for(i=0; i<MAX_ATTACHMENTS; i++){
      state->attachments[i].size = 0;
      state->attachments[i].dumped = 0;
      memset(state->attachments[i].type, 0, TINYBUFSIZE);
      memset(state->attachments[i].shorttype, 0, TINYBUFSIZE);
      memset(state->attachments[i].aname, 0, TINYBUFSIZE);
      memset(state->attachments[i].filename, 0, SMALLBUFSIZE);
      memset(state->attachments[i].internalname, 0, TINYBUFSIZE);
      memset(state->attachments[i].digest, 0, 2*DIGEST_LENGTH+1);
   }

   memset(state->reference, 0, SMALLBUFSIZE);

   memset(state->b_from, 0, SMALLBUFSIZE);
   memset(state->b_from_domain, 0, SMALLBUFSIZE);
   memset(state->b_to, 0, MAXBUFSIZE);
   memset(state->b_to_domain, 0, SMALLBUFSIZE);
   memset(state->b_subject, 0, MAXBUFSIZE);
   memset(state->b_body, 0, BIGBUFSIZE);
   memset(state->b_journal_to, 0, MAXBUFSIZE);

   state->tolen = 0;
   state->todomainlen = 0;
   state->bodylen = 0;
   state->journaltolen = 0;

   state->retention = 0;
}


long get_local_timezone_offset(){
   time_t t = time(NULL);
   struct tm lt = {0};
   localtime_r(&t, &lt);
   return lt.tm_gmtoff;
}


time_t parse_date_header(char *datestr){
   int n=0, len;
   long offset=0;
   time_t ts=0;
   char *p, *q, *r, *tz, s[SMALLBUFSIZE], tzh[4], tzm[3];
   struct tm tm;

   datestr += 5;
   p = datestr;

   for(; *datestr; datestr++){
      if(isspace(*datestr) || *datestr == '.') *datestr = ' ';
   }

   datestr = p;


   for(; *datestr != '\0' && datestr - p < 16; datestr++){
      if(*datestr == '-') *datestr = ' ';
   }

   datestr = p;

   if(*p == ' '){ p++; }

   tm.tm_wday = 0;
   tm.tm_mday = 0;
   tm.tm_mon = -1;
   tm.tm_year = 0;
   tm.tm_hour = 0;
   tm.tm_min = 0;
   tm.tm_sec = 0;
   tm.tm_isdst = -1;

   do {
      p = split_str(p, " ", s, sizeof(s)-1);

      len = strlen(s);

      if(len > 0){
         n++;

         /*
          *  A proper Date: header should look like this:
          *
          *  Date: Mon, 3 Feb 2014 13:21:07 +0100
          *
          *
          *  However some email applications provide crap, eg.
          *
          *  Sat, 4 Aug 2007 13:36:52 GMT-0700
          *  Sat, 4 Aug 07 13:36:52 GMT-0700
          *  16 Dec 07 20:45:52
          *  03 Jun 06 05:59:00 +0100
          *  30.06.2005 17:47:42
          *  03-Feb-2014 08:09:10
          *  13 Mar 2013 14:56:02 UT
          *
          *  [wday] mday mon year h:m:s offset
          */

         q = strchr(s, ','); if(q) *q='\0';

         if(strncasecmp(s, "Jan", 3) == 0) tm.tm_mon = 0;
         else if(strncasecmp(s, "Feb", 3) == 0) tm.tm_mon = 1;
         else if(strncasecmp(s, "Mar", 3) == 0) tm.tm_mon = 2;
         else if(strncasecmp(s, "Apr", 3) == 0) tm.tm_mon = 3;
         else if(strncasecmp(s, "May", 3) == 0) tm.tm_mon = 4;
         else if(strncasecmp(s, "Jun", 3) == 0) tm.tm_mon = 5;
         else if(strncasecmp(s, "Jul", 3) == 0) tm.tm_mon = 6;
         else if(strncasecmp(s, "Aug", 3) == 0) tm.tm_mon = 7;
         else if(strncasecmp(s, "Sep", 3) == 0) tm.tm_mon = 8;
         else if(strncasecmp(s, "Oct", 3) == 0) tm.tm_mon = 9;
         else if(strncasecmp(s, "Nov", 3) == 0) tm.tm_mon = 10;
         else if(strncasecmp(s, "Dec", 3) == 0) tm.tm_mon = 11;

         if(strncasecmp(s, "Mon", 3) == 0) tm.tm_wday = 1;
         else if(strncasecmp(s, "Tue", 3) == 0) tm.tm_wday = 2;
         else if(strncasecmp(s, "Wed", 3) == 0) tm.tm_wday = 3;
         else if(strncasecmp(s, "Thu", 3) == 0) tm.tm_wday = 4;
         else if(strncasecmp(s, "Fri", 3) == 0) tm.tm_wday = 5;
         else if(strncasecmp(s, "Sat", 3) == 0) tm.tm_wday = 6;
         else if(strncasecmp(s, "Sun", 3) == 0) tm.tm_wday = 0;


         if(len <= 2 && tm.tm_mday == 0){ tm.tm_mday = atoi(s); continue; }

         if(len <= 2 && tm.tm_mon == -1){ tm.tm_mon = atoi(s) - 1; continue; }

         if(len == 2){ if(atoi(s) >=90) tm.tm_year = atoi(s); else tm.tm_year = atoi(s) + 100; continue; }

         if(len == 4 && atoi(s) > 1900){ tm.tm_year = atoi(s) - 1900; continue; }

         if(len == 3){
            if(strcmp(s, "EDT") == 0) offset = -14400;
            else if(strcmp(s, "EST") == 0) offset = -18000;
            else if(strcmp(s, "CDT") == 0) offset = -18000;
            else if(strcmp(s, "CST") == 0) offset = -21600;
            else if(strcmp(s, "MDT") == 0) offset = -21600;
            else if(strcmp(s, "MST") == 0) offset = -25200;
            else if(strcmp(s, "PDT") == 0) offset = -25200;
            else if(strcmp(s, "PST") == 0) offset = -28800;

            continue;
         }

         if((len == 5 && (*s == '+' || *s == '-')) || (len == 8 && (strncmp(s, "GMT+", 4) == 0 || strncmp(s, "GMT-", 4) == 0))){
            offset = 0;
            tz = strpbrk(s, "+-");
            if(tz){
               memset(tzh, 0, 4);
               memset(tzm, 0, 3);
               strncpy(tzh, tz, 3);
               strncpy(tzm, tz+3, 2);
               offset += atoi(tzh) * 3600;
               offset += atoi(tzm) * 60;
            }
            continue;
         }

         if(len == 5 || len == 7 || len == 8){
            r = &s[0];

            q = strchr(r, ':'); if(!q) continue;
            *q = '\0'; tm.tm_hour = atoi(r); r = q+1; *q = ':';

            if(strlen(r) == 5) {
               q = strchr(r, ':'); if(!q) continue;
               *q = '\0'; tm.tm_min = atoi(r); r = q+1; *q = ':';

               tm.tm_sec = atoi(r);
            } else {
               tm.tm_min = atoi(r);
            }
            continue; 
         }
      }

   } while(p);

   if(tm.tm_mon == -1) tm.tm_mon = 0;

   ts = mktime(&tm);

   ts += get_local_timezone_offset() - offset;

   return ts;
}


int extract_boundary(char *p, struct parser_state *state){
   char *q, *q2;

   p += strlen("boundary");

   q = strchr(p, '"');
   if(q) *q = ' ';

   /*
    * I've seen an idiot spammer using the following boundary definition in the header:
    *
    * Content-Type: multipart/alternative;
    *     boundary=3D"b1_52b92b01a943615aff28b7f4d2f2d69d"
    */

   if(strncmp(p, "=3D", 3) == 0){
      *(p+3) = '=';
      p += 3;
   }

   p = strchr(p, '=');
   if(p){
      p++;
      for(; *p; p++){
         if(isspace(*p) == 0)
            break;
      }

      q2 = strchr(p, ';');
      if(q2) *q2 = '\0';

      q = strrchr(p, '"');
      if(q) *q = '\0';

      q = strrchr(p, '\r');
      if(q) *q = '\0';

      q = strrchr(p, '\n');
      if(q) *q = '\0';

      addnode(state->boundaries, p);

      if(q2) *q2 = ';';

      return 1;
   }

   return 0;
}


void fixupEncodedHeaderLine(char *buf, int buflen){
   char *p, *q, *r, *s, *e, *end;
   /*
    * I thought SMALLBUFSIZE would be enough for v, encoding and tmpbuf(2*),
    * but then I saw a 6-7000 byte long subject line, so I've switched to MAXBUFSIZE
    */
   char v[MAXBUFSIZE], u[MAXBUFSIZE], puf[MAXBUFSIZE], encoding[MAXBUFSIZE], tmpbuf[2*MAXBUFSIZE];
   int need_encoding, ret, prev_encoded=0, n_tokens=0;
   int b64=0, qp=0;

   if(buflen < 5) return;

   memset(puf, 0, sizeof(puf));

   q = buf;

   do {
      q = split_str(q, " ", v, sizeof(v)-1);

      p = v;

      do {
         memset(u, 0, sizeof(u));

         /*
          * We can't use split_str(p, "=?", ...) it will fail with the following pattern
          *    =?UTF-8?B?SG9neWFuIMOtcmp1bmsgcGFuYXN6bGV2ZWxldD8=?=
          *
          * Also the below pattern requires special care:
          *    =?gb2312?B?<something>?==?gb2312?Q?<something else>?=
          *
          * And we have to check the following cases as well:
          *    Happy New Year! =?utf-8?q?=F0=9F=8E=86?=
          */

         b64 = qp = 0;
         memset(encoding, 0, sizeof(encoding));

         r = strstr(p, "=?");
         if(r){
            p = r + 2;

            e = strchr(p, '?');
            if(e){
               *e = '\0';
               snprintf(encoding, sizeof(encoding)-1, "%s", p);
               *e = '?';

               s = strcasestr(e, "?B?");
               if(s){
                  b64 = 1;
                  p = s + 3;
               }
               else {
                  s = strcasestr(e, "?Q?");
                  if(s){
                     qp = 1;
                     p = s + 3;
                  }
               }
            }

            end = strstr(p, "?=");
            if(end){
               *end = '\0';
            }


            snprintf(u, sizeof(u)-1, "%s", p);

            if(end) {
               p = end + 2;
            }
         }
         else {
            snprintf(u, sizeof(u)-1, "%s", p);
            p = NULL;
         }

         if(u[0] == 0) continue;

         n_tokens++;

         if(b64 == 1) decodeBase64(u);
         else if(qp == 1) decodeQP(u);


         /*
          * https://www.ietf.org/rfc/rfc2047.txt says that
          *
          * "When displaying a particular header field that contains multiple
          *  'encoded-word's, any 'linear-white-space' that separates a pair of
          *  adjacent 'encoded-word's is ignored." (6.2)
          */
         if(prev_encoded == 1 && (b64 == 1 || qp == 1)) {}
         else if(n_tokens > 1){
            strncat(puf, " ", sizeof(puf)-strlen(puf)-1);
         }

         if(b64 == 1 || qp == 1){
            prev_encoded = 1;
            need_encoding = 0;
            ret = ERR;

            if(encoding[0] && strcasecmp(encoding, "utf-8")){
               need_encoding = 1;
               ret = utf8_encode(u, strlen(u), &tmpbuf[0], sizeof(tmpbuf), encoding);
            }

            if(need_encoding == 1 && ret == OK){
               strncat(puf, tmpbuf, sizeof(puf)-strlen(puf)-1);
            }
            else {
               strncat(puf, u, sizeof(puf)-strlen(puf)-1);
            }
         }
         else {
            strncat(puf, u, sizeof(puf)-strlen(puf)-1);
         }

      } while(p);

   } while(q);

   snprintf(buf, buflen-1, "%s", puf);
}


void fixupSoftBreakInQuotedPritableLine(char *buf, struct parser_state *state){
   int i=0;
   char *p, puf[MAXBUFSIZE];

   if(strlen(state->qpbuf) > 0){
      memset(puf, 0, sizeof(puf));
      snprintf(puf, sizeof(puf)-1, "%s%s", state->qpbuf, buf);
      snprintf(buf, MAXBUFSIZE-1, "%s", puf);
      memset(state->qpbuf, 0, MAX_TOKEN_LEN);
   }

   if(buf[strlen(buf)-1] == '='){
      buf[strlen(buf)-1] = '\0';
      i = 1;
   }

   if(i == 1){
      p = strrchr(buf, ' ');
      if(p){
         memset(state->qpbuf, 0, MAX_TOKEN_LEN);
         if(strlen(p) < MAX_TOKEN_LEN-1){
            memcpy(&(state->qpbuf[0]), p, MAX_TOKEN_LEN-1);
            *p = '\0';
         }

      }
   }
}


void fixupBase64EncodedLine(char *buf, struct parser_state *state){
   char *p, puf[MAXBUFSIZE];

   if(strlen(state->miscbuf) > 0){
      memset(puf, 0, sizeof(puf));
      strncpy(puf, state->miscbuf, sizeof(puf)-strlen(puf)-1);
      strncat(puf, buf, sizeof(puf)-strlen(puf)-1);

      memset(buf, 0, MAXBUFSIZE);
      memcpy(buf, puf, MAXBUFSIZE);

      memset(state->miscbuf, 0, MAX_TOKEN_LEN);
   }

   if(buf[strlen(buf)-1] != '\n'){
      p = strrchr(buf, ' ');
      if(p){
         memcpy(&(state->miscbuf[0]), p+1, MAX_TOKEN_LEN-1);
         *p = '\0';
      }
   }
}


void markHTML(char *buf, struct parser_state *state){
   char *s, puf[MAXBUFSIZE], html[SMALLBUFSIZE];
   int k=0, j=0, pos=0;

   memset(puf, 0, MAXBUFSIZE);
   memset(html, 0, SMALLBUFSIZE);

   s = buf;

   for(; *s; s++){
      if(*s == '<'){
         state->htmltag = 1;
         puf[k] = ' ';
         k++;

         memset(html, 0, SMALLBUFSIZE); j=0;

         pos = 0;

         //printf("start html:%c\n", *s);
      }

      if(state->htmltag == 1){
    
         if(j == 0 && *s == '!'){
            state->skip_html = 1;
            //printf("skiphtml=1\n");
         }

         if(state->skip_html == 0){ 
            if(*s != '>' && *s != '<' && *s != '"'){
               //printf("j=%d/%c", j, *s);
               html[j] = tolower(*s);
               if(j < SMALLBUFSIZE-10) j++;
            }

            if(isspace(*s)){
               if(j > 0){
                  k += appendHTMLTag(puf, html, pos, state);
                  memset(html, 0, SMALLBUFSIZE); j=0;
               }
               pos++;
            }
         }
      }
      else {
         if(state->style == 0){
            puf[k] = *s;
            k++;
         }
      }

      if(*s == '>'){
         state->htmltag = 0;
         state->skip_html = 0;

         //printf("skiphtml=0\n");
         //printf("end html:%c\n", *s);

         //strncat(html, " ", SMALLBUFSIZE-1);

         if(j > 0){
            strncat(html, " ", SMALLBUFSIZE-1);
            k += appendHTMLTag(puf, html, pos, state);
            memset(html, 0, SMALLBUFSIZE); j=0;
         }
      }

   }

   //printf("append last in line:*%s*, html=+%s+, j=%d\n", puf, html, j);
   if(j > 0){ appendHTMLTag(puf, html, pos, state); }

   strcpy(buf, puf);
}


int appendHTMLTag(char *buf, char *htmlbuf, int pos, struct parser_state *state){
   char *p, html[SMALLBUFSIZE];
   int len;

   if(pos == 0 && strncmp(htmlbuf, "style ", 6) == 0) state->style = 1;
   if(pos == 0 && strncmp(htmlbuf, "/style ", 7) == 0) state->style = 0;

   return 0;

   //printf("appendHTML: pos:%d, +%s+\n", pos, htmlbuf);

   if(state->style == 1) return 0;

   if(strlen(htmlbuf) == 0) return 0;

   snprintf(html, SMALLBUFSIZE-1, "HTML*%s", htmlbuf);
   len = strlen(html);

   if(len > 8 && strchr(html, '=')){
      p = strstr(html, "cid:");
      if(p){
         *(p+3) = '\0';
         strncat(html, " ", SMALLBUFSIZE-1);
      }

      strncat(buf, html, MAXBUFSIZE-1);
      return len;
   }

   if(strstr(html, "http") ){
      strncat(buf, html+5, MAXBUFSIZE-1);
      return len-5;
   }

   return 0;
}


void translateLine(unsigned char *p, struct parser_state *state){
   int url=0;
   int has_url=0;
   unsigned char prev=' ';

   if(strcasestr((char *)p, "http://") || strcasestr((char *)p, "https://")) has_url = 1;

   for(; *p; p++){

      if( (state->message_state == MSG_RECEIVED || state->message_state == MSG_FROM || state->message_state == MSG_TO || state->message_state == MSG_CC || state->message_state == MSG_RECIPIENT) && *p == '@'){ continue; }

      if(state->message_state == MSG_FROM || state->message_state == MSG_TO || state->message_state == MSG_CC || state->message_state == MSG_RECIPIENT){

         /* To fix some unusual addresses, eg.
          *    "'user@domain'"    -> user@domain
          *    "''user@domain'"   -> 'user@domain
          *    "''user'@domain'"  -> 'user'@domain
          *    "'user'@domain'"   -> user'@domain
          */

         if(*p == '\'' && prev == '"') { *p = ' '; }
         if(*p == '\'' && *(p+1) == '"'){ *p = ' '; }

         if(*p == '_' || *p == '\'' || *p == '&' || *p == '+'){ continue; }

         prev = *p;
      }


      if(state->message_state == MSG_SUBJECT && (*p == '%' || *p == '_' || *p == '&') ){ continue; }

      if(state->message_state == MSG_CONTENT_TYPE && *p == '_' ){ continue; }

      if(*p == '.' || *p == '-'){ continue; }

      if(has_url == 1){
         if(strncasecmp((char *)p, "http://", 7) == 0){ p += 7; url = 1; continue; }
         if(strncasecmp((char *)p, "https://", 8) == 0){ p += 8; url = 1; continue; }

         if(url == 1 && (*p == '.' || *p == '-' || *p == '_' || *p == '/' || *p == '%' || *p == '?' || isalnum(*p)) ) continue;
         if(url == 1) url = 0;
      }

      if(delimiter_characters[(unsigned int)*p] != ' ') *p = ' ';
      /* we MUSTN'T convert it to lowercase in the 'else' case, because it breaks utf-8 encoding! */

   }

}


void fix_email_address_for_sphinx(char *s){
   for(; *s; s++){
      if(*s == '@' || *s == '.' || *s == '+' || *s == '-' || *s == '_') *s = 'X';
   }
}


void split_email_address(char *s){
   for(; *s; s++){
      if(*s == '@' || *s == '.' || *s == '+' || *s == '-' || *s == '_') *s = ' ';
   }
}


int does_it_seem_like_an_email_address(char *email){
   char *p;

   if(email == NULL) return 0;

   if(strlen(email) < 5) return 0;

   p = strchr(email, '@');
   if(!p) return 0;

   if(strlen(p+1) < 3) return 0;

   if(!strchr(p+1, '.')) return 0;

   return 1;
}


/*
 * reassemble 'V i a g r a' to 'Viagra'
 */

void reassembleToken(char *p){
   unsigned int i, k=0;

   for(i=0; i<strlen(p); i++){

      if(isspace(*(p+i-1)) && !isspace(*(p+i)) && isspace(*(p+i+1)) && !isspace(*(p+i+2)) && isspace(*(p+i+3)) && !isspace(*(p+i+4)) && isspace(*(p+i+5)) ){
         p[k] = *(p+i); k++;
         p[k] = *(p+i+2); k++;
         p[k] = *(p+i+4); k++;

         i += 5;
      }
      else {
         p[k] = *(p+i);
         k++;
      }
   }

   p[k] = '\0';
}


void degenerateToken(unsigned char *p){
   int i=1, d=0, dp=0;
   unsigned char *s;

   /* quit if this the string does not end with a punctuation character */

   if(!ispunct(*(p+strlen((char *)p)-1)))
      return;

   s = p;

   for(; *p; p++){
      if(ispunct(*p)){
         d = i;

         if(!ispunct(*(p-1)))
            dp = d;
      }
      else
         d = dp = i;

      i++;
   }

   *(s+dp) = '\0';

   if(*(s+dp-1) == '.' || *(s+dp-1) == '!' || *(s+dp-1) == '?') *(s+dp-1) = '\0';
}


void fixURL(char *buf, int buflen){
   int len=0;
   char *p, *q, fixed_url[SMALLBUFSIZE];

   if(strlen(buf) < 3) return;

   memset(fixed_url, 0, sizeof(fixed_url));

   p = buf;

   if(strncasecmp(buf, "http://", 7) == 0) p += 7;
   if(strncasecmp(buf, "https://", 8) == 0) p += 8;

   q = strchr(p, '/');
   if(q) *q = '\0';

   snprintf(fixed_url, sizeof(fixed_url)-1, "__URL__%s ", p);
   fix_email_address_for_sphinx(fixed_url+7);

   len = strlen(fixed_url);
   if(len > 9 && fixed_url[len-2] == 'X'){
      fixed_url[len-2] = ' ';
      fixed_url[len-1] = '\0';
   }

   snprintf(buf, buflen, "%s", fixed_url);
}


void extractNameFromHeaderLine(char *s, char *name, char *resultbuf, int resultbuflen){
   int extended=0;
   char buf[SMALLBUFSIZE], puf[SMALLBUFSIZE], *p, *q, *encoding;

   snprintf(buf, sizeof(buf)-1, "%s", s);

   memset(resultbuf, 0, resultbuflen);

   p = strstr(buf, name);
   if(p){

      /*
       *
       * Some examples from http://tools.ietf.org/html/rfc5987:
       *
       *   Non-extended notation, using "token":
       *
       *      foo: bar; title=Economy
       *
       *   Non-extended notation, using "quoted-string":
       *
       *      foo: bar; title="US-$ rates"
       *
       *   Extended notation, using the Unicode character U+00A3 (POUND SIGN):
       *
       *      foo: bar; title*=iso-8859-1'en'%A3%20rates
       *
       *   Extended notation, using the Unicode characters U+00A3 (POUND SIGN) and U+20AC (EURO SIGN):
       *
       *      foo: bar; title*=UTF-8''%c2%a3%20and%20%e2%82%ac%20rates
       *
       *   Odd one having two filename definitions, and having a semicolon (;) in the filename:
       *
       *      filename*=utf-8''P;LAN%20Holden%204.docx;filename="P;LAN Holden 4.docx"*
       *
       *
       */

      p += strlen(name);
      if(*p == '*'){
         extended = 1;
      }

      p = strchr(p, '=');
      if(p){
         p++;

         // skip any whitespace after name=, ie. name = "
         while(*p==' ' || *p=='\t') p++;

         // if there's a double quote after the equal symbol (=), ie. name*="utf-8....
         if(*p == '"'){
            p++;
            q = strchr(p, '"');

            if(q) *q = '\0';
         }
         else {
            // no " after =, so split on ;
            q = strchr(p, ';');
            if(q) *q = '\0';
         }


         if(extended == 1){
            encoding = p;
            q = strchr(p, '\'');
            if(q){
               *q = '\0';
               p = q + 1;
               q = strchr(p, '\'');
               if(q) p = q + 1;
            }

            decodeURL(p);

            if(strlen(encoding) > 2 && strcasecmp(encoding, "utf-8"))
               utf8_encode(p, strlen(p), resultbuf, resultbuflen-2, encoding);
            else
               snprintf(resultbuf, resultbuflen-2, "%s", p);
         }
         else {
            snprintf(puf, sizeof(puf)-1, "%s", p);
            fixupEncodedHeaderLine(puf, sizeof(puf));

            snprintf(resultbuf, resultbuflen-2, "%s", puf);
         }

      }
   }

}


char *determine_attachment_type(char *filename, char *type){
   char *p;

   if(strncasecmp(type, "text/", strlen("text/")) == 0) return "text,";
   if(strncasecmp(type, "image/", strlen("image/")) == 0) return "image,";
   if(strncasecmp(type, "audio/", strlen("audio/")) == 0) return "audio,";
   if(strncasecmp(type, "video/", strlen("video/")) == 0) return "video,";
   if(strncasecmp(type, "text/x-card", strlen("text/x-card")) == 0) return "vcard,";

   if(strncasecmp(type, "application/pdf", strlen("application/pdf")) == 0) return "pdf,";

   if(strncasecmp(type, "application/ms-tnef", strlen("application/ms-tnef")) == 0) return "tnef,";
   if(strncasecmp(type, "application/msword", strlen("application/msword")) == 0) return "word,";

   // a .csv file has the same type
   if(strncasecmp(type, "application/vnd.ms-excel", strlen("application/vnd.ms-excel")) == 0) return "excel,";

   if(strncasecmp(type, "application/vnd.ms-powerpoint", strlen("application/vnd.ms-powerpoint")) == 0) return "powerpoint,";

   if(strncasecmp(type, "application/vnd.visio", strlen("application/vnd.visio")) == 0) return "visio,";

   if(strncasecmp(type, "application/vnd.openxmlformats-officedocument.wordprocessingml.document", strlen("application/vnd.openxmlformats-officedocument.wordprocessingml.document")) == 0) return "word,";
   if(strncasecmp(type, "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", strlen("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet")) == 0) return "excel,";
   if(strncasecmp(type, "application/vnd.openxmlformats-officedocument.presentationml.presentation", strlen("application/vnd.openxmlformats-officedocument.presentationml.presentation")) == 0) return "powerpoint,";

   if(strncasecmp(type, "application/x-shockwave-flash", strlen("application/x-shockwave-flash")) == 0) return "flash,";

   if(strcasestr(type, "opendocument")) return "odf,";



   if(strncasecmp(type, "application/", 12) == 0){

      p = strrchr(filename, '.');
      if(p){
         p++;

         if(strncasecmp(p, "pdf", 3) == 0) return "pdf,";

         if(strncasecmp(p, "zip", 3) == 0) return "compressed,";
         if(strncasecmp(p, "rar", 3) == 0) return "compressed,";

         // tar.gz has the same type
         if(strncasecmp(p, "x-gzip", 3) == 0) return "compressed,";

         if(strncasecmp(p, "rtf", 3) == 0) return "word,";
         if(strncasecmp(p, "doc", 3) == 0) return "word,";
         if(strncasecmp(p, "docx", 4) == 0) return "word,";
         if(strncasecmp(p, "xls", 3) == 0) return "excel,";
         if(strncasecmp(p, "xlsx", 4) == 0) return "excel,";
         if(strncasecmp(p, "ppt", 3) == 0) return "powerpoint,";
         if(strncasecmp(p, "pptx", 4) == 0) return "powerpoint,";

         if(strncasecmp(p, "png", 3) == 0) return "image,";
         if(strncasecmp(p, "gif", 3) == 0) return "image,";
         if(strncasecmp(p, "jpg", 3) == 0) return "image,";
         if(strncasecmp(p, "jpeg", 4) == 0) return "image,";
         if(strncasecmp(p, "tiff", 4) == 0) return "image,";
      } 
   }

   return "other,";
}


char *get_attachment_extractor_by_filename(char *filename){
   char *p;

   if(strcasecmp(filename, "winmail.dat") == 0) return "tnef";

   p = strrchr(filename, '.');
   if(!p) return "other";

   if(strcasecmp(p, ".pdf") == 0) return "pdf";
   if(strcasecmp(p, ".zip") == 0) return "zip";
   if(strcasecmp(p, ".gz") == 0) return "gzip";
   if(strcasecmp(p, ".rar") == 0) return "rar";
   if(strcasecmp(p, ".odt") == 0) return "odf";
   if(strcasecmp(p, ".odp") == 0) return "odf";
   if(strcasecmp(p, ".ods") == 0) return "odf";
   if(strcasecmp(p, ".doc") == 0) return "doc";
   if(strcasecmp(p, ".docx") == 0) return "docx";
   if(strcasecmp(p, ".xls") == 0) return "xls";
   if(strcasecmp(p, ".xlsx") == 0) return "xlsx";
   if(strcasecmp(p, ".ppt") == 0) return "ppt";
   if(strcasecmp(p, ".pptx") == 0) return "pptx";
   if(strcasecmp(p, ".rtf") == 0) return "rtf";
   if(strcasecmp(p, ".txt") == 0) return "text";
   if(strcasecmp(p, ".csv") == 0) return "text";

   return "other";
}


void parse_reference(struct parser_state *state, char *s){
   int len;
   char puf[SMALLBUFSIZE];

   if(strlen(state->reference) > 10) return;

   do {
      s = split_str(s, " ", puf, sizeof(puf)-1);
      len = strlen(puf);

      if(len > 10 && len < SMALLBUFSIZE-1){
         memcpy(&(state->reference[strlen(state->reference)]), puf, len);
         return;
      }
   } while(s);

}


int base64_decode_attachment_buffer(char *p, unsigned char *b, int blen){
   int b64len=0;
   char puf[2*SMALLBUFSIZE];

   do {
      p = split_str(p, "\n", puf, sizeof(puf)-1);
      trimBuffer(puf);
      b64len += decode_base64_to_buffer(puf, strlen(puf), b+b64len, blen);
   } while(p);

   return b64len;
}


void fix_plus_sign_in_email_address(char *puf, char **at_sign, unsigned int *len){
   int n;
   char *r;

   r = strchr(puf, '+');
   if(r){
      n = strlen(*at_sign);
      memmove(r, *at_sign, n);
      *(r+n) = '\0';
      *len = strlen(puf);
      *at_sign = r;
   }
}

