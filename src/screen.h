/*
 * screen.h, SJ
 */

#ifndef _SCREEN_H
 #define _SCREEN_H

#include "defs.h"

void init_smtp_acl(struct smtp_acl *smtp_acl[]);
void clear_smtp_acl(struct smtp_acl *smtp_acl[]);
int add_smtp_acl(struct smtp_acl *smtp_acl[], char *network_str, struct smtp_acl *acl);
void load_smtp_acl(struct smtp_acl *smtp_acl[]);
int is_blocked_by_pilerscreen(struct smtp_acl *smtp_acl[], char *ipaddr);

#endif /* _SCREEN_H */
