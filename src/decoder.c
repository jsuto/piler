/*
 * decoder.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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


int decodeBase64(char *p){
   int i, j, n[4], k1, k2, len=0;
   char s[5], s2[3], puf[MAXBUFSIZE];

   if(strlen(p) < 4 || strlen(p) > MAXBUFSIZE/2)
      return 0;

   for(i=0; i<strlen(p); i++){
      memcpy(s, p+i, 4);
      s[4] = '\0';

      i += 3;

      if(strlen(s) == 4){
         memset(s2, 0, 3);

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

         // this is binary safe
         memcpy(puf+len, s2, 3);

         len += 3;
      }

   }

   *(puf+len) = '\0';

   snprintf(p, MAXBUFSIZE-1, "%s", puf);

   return len;

}


void decodeUTF8(char *p){
   int i, k=0, a, b;
   unsigned char c, c1, c2;

   if(p == NULL) return;

   for(i=0; i<strlen(p); i++){
      c = p[i];

      if(p[i] == '=' && isxdigit(p[i+1]) && isxdigit(p[i+2]) &&
           p[i+3] == '=' && isxdigit(p[i+4]) && isxdigit(p[i+5])){

         a = p[i+1];
         b = p[i+2];
         c1 = 16 * hex_table[a] + hex_table[b];

         a = p[i+4];
         b = p[i+5];
         c2 = 16 * hex_table[a] + hex_table[b];


         if(c1 >= 192 && c1 <= 223){
            c = 64 * (c1 - 192) + c2 - 128;
            i += 5;
         }
         
      }

      if(c >= 192 && c <= 223){
         c =  64 * (c - 192) + p[i+1] - 128;
         i++;
      }

      p[k] = c;
      k++;
   }

   p[k] = '\0';
}


void decodeQP(char *p){
   int i, k=0, a, b;
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


void decodeHTML(char *s){
   char *p;
   int i, c, k=0, unknown='q';
   struct mi key, *res;

   if(s == NULL) return;

   for(i=0; i<strlen(s); i++){
      c = s[i];

      if(*(s+i) == '&'){
         p = strchr(s+i, ';');
         if(p){
            *p = '\0';

            if(*(s+i+1) == '#'){
               c = atoi(s+i+2);
               if(c == 0) c = unknown;
            }
            else {
               key.entity = s+i;
               res = bsearch(&key, htmlentities, NUM_OF_HTML_ENTITIES, sizeof(struct mi), compmi);

               if(res && res->val <= 255) c = res->val;
               else c = unknown;
            }

            i += strlen(s+i);
            *p = ';';

         }
      }

      s[k] = c;
      k++;
   }

   s[k] = '\0';
}


void decodeURL(char *p){
   int i, c, k=0, a, b;

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

