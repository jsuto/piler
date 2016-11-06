/*
 * decoder.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <iconv.h>
#include "decoder.h"
#include "htmlentities.h"
#include "config.h"


static int b64[] = {

     255, 255, 255, 255,   255, 255, 255, 255,   255, 255, 255, 255,  255, 255, 255, 255,
     255, 255, 255, 255,   255, 255, 255, 255,   255, 255, 255, 255,  255, 255, 255, 255,
     255, 255, 255, 255,   255, 255, 255, 255,   255, 255, 255,  62,  255, 255, 255,  63,
      52,  53,  54,  55,    56,  57,  58,  59,    60,  61, 255, 255,  255,   0, 255, 255,

     255,   0,   1,   2,     3,   4,   5,   6,     7,   8,   9,  10,   11,  12,  13,  14,
      15,  16,  17,  18,    19,  20,  21,  22,    23,  24,  25, 255,  255, 255, 255, 255,
     255,  26,  27,  28,    29,  30,  31,  32,    33,  34,  35,  36,   37,  38,  39,  40,
      41,  42,  43,  44,    45,  46,  47,  48,    49,  50,  51, 255,  255, 255, 255, 255,

     255, 255, 255, 255,   255, 255, 255, 255,   255, 255, 255, 255,  255, 255, 255, 255,
     255, 255, 255, 255,   255, 255, 255, 255,   255, 255, 255, 255,  255, 255, 255, 255,
     255, 255, 255, 255,   255, 255, 255, 255,   255, 255, 255, 255,  255, 255, 255, 255,
     255, 255, 255, 255,   255, 255, 255, 255,   255, 255, 255, 255,  255, 255, 255, 255,

     255, 255, 255, 255,   255, 255, 255, 255,   255, 255, 255, 255,  255, 255, 255, 255,
     255, 255, 255, 255,   255, 255, 255, 255,   255, 255, 255, 255,  255, 255, 255, 255,
     255, 255, 255, 255,   255, 255, 255, 255,   255, 255, 255, 255,  255, 255, 255, 255,
     255, 255, 255, 255,   255, 255, 255, 255,   255, 255, 255, 255,  255, 255, 255, 255

};


static char hex_table[] = {
   0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  1,  2,  3,   4,  5,  6,  7,  8,  9,  0,  0,  0,  0,  0,  0,

   0, 10, 11, 12,  13, 14, 15,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0, 10, 11, 12,  13, 14, 15,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,

   0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,

   0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};


static int compmi(const void *m1, const void *m2){
   struct mi *mi1 = (struct mi *) m1;
   struct mi *mi2 = (struct mi *) m2;
   return strcmp(mi1->entity, mi2->entity);
}


inline void utf8_encode_char(unsigned char c, unsigned char *buf, int buflen, int *len){
   int count=0;

   memset(buf, 0, buflen);

      /*
       * Code point          1st byte    2nd byte    3rd byte    4th byte
       * ----------          --------    --------    --------    --------
       * U+0000..U+007F      00..7F
       * U+0080..U+07FF      C2..DF      80..BF
       * U+0800..U+0FFF      E0          A0..BF      80..BF
       */

      if(c <= 0x7F){
         *(buf+count) = c;
         count++;
      }

      else {
         *(buf+count) = ( 0xC0 | (c >> 6) );
         count++;
         *(buf+count) = ( 0x80 | (c & 0x3F) );
         count++;
      }

   *len = count;
}


void sanitiseBase64(char *s){
   char *p1;

   if(s == NULL) return;

   for(; *s; s++){
      if(b64[(unsigned int)(*s & 0xFF)] == 255){
         for(p1 = s; p1[0] != '\0'; p1++)
            p1[0] = p1[1];
      }
   }
}


inline static void pack_4_into_3(char *s, char *s2){
   int j, n[4], k1, k2;

   memset(s2, 0, 3);

   if(strlen(s) != 4) return;

   for(j=0; j<4; j++){
      k1 = s[j];
      n[j] = b64[k1];
   }

   k1 = n[0]; k1 = k1 << 2;
   k2 = n[1]; k2 = k2 >> 4;

   s2[0] = k1 | k2;

   k1 = (n[1] & 0x0F) << 4;
   k2 = n[2]; k2 = k2 >> 2;

   s2[1] = k1 | k2;

   k1 = n[2] << 6;
   k2 = n[3] >> 0;


  s2[2] = k1 | k2;
}


int decodeBase64(char *p){
   int len=0;
   unsigned char puf[MAXBUFSIZE];

   memset(puf, 0, sizeof(puf));

   len = decode_base64_to_buffer(p, strlen(p), &puf[0], sizeof(puf)-1);

   snprintf(p, MAXBUFSIZE-1, "%s", puf);

   return len;
}


int decode_base64_to_buffer(char *p, int plen, unsigned char *b, int blen){
   int i, len=0, decodedlen;
   char s[5], s2[3];

   if(plen < 4 || plen > blen)
      return 0;

   for(i=0; i<plen; i+=4){
      memcpy(s, p+i, 4);
      s[4] = '\0';
      decodedlen = 3;

      /* safety check against abnormally long lines */

      if(len + decodedlen > blen-1) break;

      if(strlen(s) == 4){
         pack_4_into_3(s, s2);
         if(s[3] == '=') decodedlen = 2;
         if(s[2] == '=') decodedlen = 1;

         memcpy(b+len, s2, decodedlen);

         len += decodedlen;
      }

   }

   return len;
}


void decodeQP(char *p){
   unsigned int i;
   int k=0, a, b;
   char c;

   if(p == NULL) return;

   for(i=0; i<strlen((char*)p); i++){
      c = p[i];

      if(p[i] == '=' && isxdigit(p[i+1]) && isxdigit(p[i+2])){
         a = p[i+1];
         b = p[i+2];

         c = 16 * hex_table[a] + hex_table[b];

         i += 2;
      }

      p[k] = c;
      k++;
   }

   p[k] = '\0';
}


void decodeHTML(char *p, int utf8){
   unsigned char buf[MAXBUFSIZE], __u[8];
   char *s, *q;
   int count=0, len, c;
   struct mi key, *res;

   if(p == NULL || strlen(p) == 0) return;

   s = p;

   memset(buf, 0, sizeof(buf));

   for(; *s; s++){
      if(*s == '&'){
         q = strchr(s, ';');
         if(q){
            *q = '\0';

            if(*(s+1) == '#'){
               c = atoi(s+2);
               if(c == 0) c = 'q';

               buf[count] = (unsigned char)c;
               count++;
            }
            else {
               key.entity = s;
               res = bsearch(&key, htmlentities, NUM_OF_HTML_ENTITIES, sizeof(struct mi), compmi);

               if(res && res->val <= 255){

                  if(utf8 == 1){
                     utf8_encode_char(res->val, &__u[0], sizeof(__u), &len);
                     memcpy(&buf[count], &__u[0], len);
                     count += len;
                  }
                  else {
                     buf[count] = res->val;
                     count++;
                  }
               }
               else {
                  buf[count] = 'q';
                  count++;
               }
            }

            s = q;
         }

      }
      else {
         buf[count] = *s;
         count++;
      }
   }

   buf[count] = '\0'; count++;

   memcpy(p, buf, count);
}


void decodeURL(char *p){
   unsigned int i;
   int c, k=0, a, b;

   if(p == NULL) return;

   for(i=0; i<strlen(p); i++){
      switch(p[i]){
         case '+':
            c = ' ';
            break;

         case '%':
            if(isxdigit(p[i+1]) && isxdigit(p[i+2])){
               a = p[i+1];
               b = p[i+2];

               c = 16 * hex_table[a] + hex_table[b];

               i += 2;
            }
            else
               c = p[i];

            break;

        default:
            c = p[i];
            break;
      }

      p[k] = c;
      k++;

   }

   p[k] = '\0';
}


int utf8_encode(char *inbuf, int inbuflen, char *outbuf, int outbuflen, char *encoding){
   iconv_t cd;
   size_t inbytesleft, outbytesleft;
   int ret = ERR;

   memset(outbuf, 0, outbuflen);

   cd = iconv_open("utf-8", encoding);

   if(cd != (iconv_t)-1){
      inbytesleft = inbuflen;
      outbytesleft = outbuflen-1;

      if(iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft) == (size_t) -1)
         ret = ERR;
      else
         ret = OK;

      iconv_close(cd);
   }

   return ret;
}

