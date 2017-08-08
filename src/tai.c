#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "tai.h"

static char hex[16] = "0123456789abcdef";


void tai_pack(char *s, struct tai *t){
   uint64 x = t->x;

   s[7] = x & 255; x >>= 8;
   s[6] = x & 255; x >>= 8;
   s[5] = x & 255; x >>= 8;
   s[4] = x & 255; x >>= 8;
   s[3] = x & 255; x >>= 8;
   s[2] = x & 255; x >>= 8;
   s[1] = x & 255; x >>= 8;
   s[0] = x;
}


void taia_pack(char *s, struct taia *t){
   unsigned long x;

   tai_pack(s, &t->sec);
   s += 8;

   x = t->atto;
   s[7] = x & 255; x >>= 8;
   s[6] = x & 255; x >>= 8;
   s[5] = x & 255; x >>= 8;
   s[4] = x;

   x = t->nano;
   s[3] = x & 255; x >>= 8;
   s[2] = x & 255; x >>= 8;
   s[1] = x & 255; x >>= 8;
   s[0] = x;
}


void taia_now(struct taia *t){
   struct timeval now;

   gettimeofday(&now,(struct timezone *) 0);

   t->sec.x = 4611686018427387914ULL + (uint64)now.tv_sec;
   t->nano = 1000 * now.tv_usec + 500;
   t->atto = 0;
}


void tai_timestamp(char *s){
   struct tai now;
   char nowpack[TAI_PACK];
   int i;

   now.x = 4611686018427387914ULL + (uint64)time((long *) 0);

   tai_pack(nowpack, &now);

   for (i = 0;i < 8;++i) {
      *(s+i*2) = hex[(nowpack[i] >> 4) & 15];
      *(s+i*2+1) = hex[nowpack[i] & 15];
   }

   *(s+2*TAI_PACK) = '\0';
}
