/*
 * base64.c, SJ
 */

#include <stdio.h>
#include <string.h>


char base64_value(char c){
   static const char *base64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

   if((int)c > 63) return '=';

   return base64_table[(int)c];
}


void base64_encode_block(unsigned char *in, int inlen, char *out){
   char a, b, c, d, fragment;

   sprintf(out, "====");

   if(inlen <= 0) return;

   fragment = *in & 0x3;

   a = *in >> 2;

   out[0] = base64_value(a);

   b = fragment << 4;

   if(inlen > 1)
      b += *(in+1) >> 4;

   out[1] = base64_value(b);

   if(inlen == 1) return;


   c = *(in+1) & 0xf;
   c = c << 2;

   if(inlen > 2){
      fragment = *(in+2) & 0xfc;
      c += fragment >> 6;

      d = *(in+2) & 0x3f;
      out[3] = base64_value(d);
   }

   out[2] = base64_value(c);
}


void base64_encode(unsigned char *in, int inlen, char *out, int outlen){
   int i=0, j, pos=0;
   unsigned char buf[3];

   memset(buf, 0, 3);
   memset(out, 0, outlen);

   for(j=0; j<inlen; j++){

      if(i == 3){
         base64_encode_block(buf, 3, &out[pos]); pos += 4;

         memset(buf, 0, 3);

         i = 0;
      }

      buf[i] = *(in+j);
      i++;
   }

   base64_encode_block(buf, i, &out[pos]);
}
