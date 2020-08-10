#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "tai.h"



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
