/*
 * hash.h, SJ
 */

#ifndef _HASH_H
 #define _HASH_H

#include "cfg.h"
#include "defs.h"


void inithash(struct node *xhash[]);
void clearhash(struct node *xhash[]);
struct node *makenewnode(char *s);
int addnode(struct node *xhash[], char *s);
struct node *findnode(struct node *xhash[], char *s);
int is_substr_in_hash(struct node *xhash[], char *s);
inline int hash(unsigned int key);
unsigned int DJBHash(char* str, unsigned int len);

#endif /* _HASH_H */
