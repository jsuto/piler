/*
 * list.h, SJ
 */

#ifndef _LIST_H
 #define _LIST_H

#include "defs.h"

int append_list(struct url **urls, char *p);
struct url *createListItem(char *s);
int isOnList(struct url *u, char *item);
void freeList(struct url *u);

#endif /* _LIST_H */

