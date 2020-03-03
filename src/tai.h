/*
 * tai.h, SJ
 */

#ifndef _TAI_H
   #define _TAI_H

#define TAI_PACK 8
#define TAIA_PACK 16
#define TIMESTAMP 25

typedef unsigned long long uint64;

struct tai {
   uint64 x;
};

struct taia {
   struct tai sec;
   unsigned long nano; /* 0...999999999 */
   unsigned long atto; /* 0...999999999 */
};


void taia_now(struct taia *t);
void taia_pack(char *s, struct taia *t);

#endif /* _TAI_H */
