/*
 * list.h, SJ
 */

#ifndef _LIST_H
 #define _LIST_H

#include "defs.h"

int append_list(struct list **list, char *p);
struct list *create_list_item(char *s);
int is_string_on_list(struct list *list, char *s);
int is_item_on_string(struct list *list, char *s);
void free_list(struct list *list);

#endif /* _LIST_H */

