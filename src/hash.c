/*
 * hash.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <piler.h>


void inithash(struct node *xhash[]){
   int i;

   for(i=0;i<MAXHASH;i++){
      xhash[i] = NULL;
   }
}


void clearhash(struct node *xhash[]){
   int i;
   struct node *p, *q;

   for(i=0;i<MAXHASH;i++){
      q = xhash[i];
      while(q != NULL){
         p = q;

         q = q->r;
         if(p){
            if(p->str){
               free(p->str);
            }
            free(p);
         }
      }
      xhash[i] = NULL;
   }
}


struct node *makenewnode(struct node *xhash[], char *s){
   struct node *h;
   int len;

   if(s == NULL) return NULL;

   len = strlen(s);

   if((h = malloc(sizeof(struct node))) == NULL) return NULL;

   memset(h, 0, sizeof(struct node));

   h->str = malloc(len+2);

   if(h->str == NULL){
      free(h);
      return NULL;
   }

   memset(h->str, 0, len+2);

   snprintf(h->str, len+1, "%s", s);

   h->key = DJBHash(s, len);
   h->r = NULL;

   return h;
}


int addnode(struct node *xhash[], char *s){
   struct node *p=NULL, *q;
   unsigned int key = 0;
   int len;

   if(s == NULL) return 0;

   len = strlen(s);

   key = DJBHash(s, len);

   if(xhash[hash(key)] == NULL){
      xhash[hash(key)] = makenewnode(xhash, s);
   }
   else {
      q = xhash[hash(key)];
      while(q != NULL){
         p = q;
         if(p->key == key){
            return 0;
         }
         else {
            q = q->r;
         }
      }
      if(p) p->r = makenewnode(xhash, s);
   }

   return 1;
}


struct node *findnode(struct node *xhash[], char *s){
   struct node *q;
   unsigned int key;
   int len;

   if(s == NULL) return NULL;

   len = strlen(s);

   key = DJBHash(s, len);

   q = xhash[hash(key)];

   if(q == NULL) return NULL;

   while(q != NULL){

      if(strcmp(q->str, s) == 0){
         return q;
      }
      else {
         q = q->r;
      }
   }

   return NULL;
}


int is_substr_in_hash(struct node *xhash[], char *s){
   int i;
   struct node *q;

   for(i=0;i<MAXHASH;i++){
      q = xhash[i];
      while(q != NULL){
         if(q->str && strstr(s, q->str)) return 1;

         q = q->r;
      }
   }

   return 0;
}


inline int hash(unsigned int key){
   return key % MAXHASH;
}


unsigned int DJBHash(char* str, unsigned int len){
   unsigned int hash = 5381;
   unsigned int i    = 0;

   for(i=0; i < len; str++, i++){
      hash = ((hash << 5) + hash) + (*str);
   }

   return hash;
}

