/*
 * boundary.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <piler.h>


/*
 * append something to the boundary list if we have to
 */

int append_boundary(struct boundary **boundaries, char *p){
   struct boundary *q, *t, *u=NULL;

   q = *boundaries;

   while(q){
      if(strcmp(q->boundary_str, p) == 0)
         return 0;

      u = q;
      q = q->r;
   }

   t = new_boundary(p);
   if(t){
      if(*boundaries == NULL)
         *boundaries = t;
      else if(u)
         u->r = t;

      return 1;
   }

   return -1;
}



/*
 * create a new boundary structure
 */

struct boundary *new_boundary(char *s){
   struct boundary *h=NULL;

   if((h = malloc(sizeof(struct boundary))) == NULL)
      return NULL;

   strncpy(h->boundary_str, s, BOUNDARY_LEN-1);
   h->r = NULL;

   return h;
}


/*
 * is this a boundary string?
 */

int is_boundary(struct boundary *boundaries, char *s){
   struct boundary *p;

   p = boundaries;

   while(p != NULL){
      if(strstr(s, p->boundary_str)) return 1;
      p = p->r;
   }

   return 0;
}


/*
 * free boundary list
 */

void free_boundary(struct boundary *b){
   struct boundary *p;

   while(b){
      p = b->r;

      //printf("free boundary: %s\n", b->boundary_str);
      if(b)
         free(b);

      b = p;
   }
}



