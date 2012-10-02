/*
 * list.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "list.h"
#include "config.h"


int append_list(struct list **list, char *p){
   struct list *q, *t, *u=NULL;

   q = *list;

   while(q){
      if(strcmp(q->s, p) == 0)
         return 0;

      u = q;
      q = q->r;
   }

   t = create_list_item(p);
   if(t){
      if(*list == NULL)
         *list = t;
      else if(u)
         u->r = t;

      return 1;
   }

   return -1;
}


struct list *create_list_item(char *s){
   struct list *h=NULL;

   if((h = malloc(sizeof(struct list))) == NULL)
      return NULL;

   snprintf(h->s, SMALLBUFSIZE-1, "%s", s);
   h->r = NULL;

   return h;
}


int is_string_on_list(struct list *list, char *s){
   struct list *p;

   p = list;

   while(p != NULL){
      if(strcasecmp(p->s, s) == 0) return 1;
      p = p->r;
   }

   return 0;
}


int is_item_on_string(struct list *list, char *s){
   struct list *p;

   p = list;

   while(p != NULL){
      if(strstr(s, p->s)) return 1;
      p = p->r;
   }

   return 0;
}


void free_list(struct list *list){
   struct list *p, *q;

   p = list;

   while(p != NULL){
      q = p->r;

      if(p)
         free(p);

      p = q;
   }
}



