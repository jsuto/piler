/*
 * boundary.h, SJ
 */

#ifndef _BOUNDARY_H
 #define _BOUNDARY_H

int append_boundary(struct boundary **boundaries, char *p);
struct boundary *new_boundary(char *s);
int is_boundary(struct boundary *boundaries, char *s);
void free_boundary(struct boundary *b);

#endif /* _LIST_H */

