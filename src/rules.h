/*
 * rules.h, SJ
 */

#ifndef _RULES_H
 #define _RULES_H

#include "defs.h"

void load_archiving_rules(struct session_data *sdata, struct rule **rules);
int append_rule(struct rule **rule, char *from, char *to, char *subject, char *_size, int size, char *attachment_type, char *_attachment_size, int attachment_size);
struct rule *create_rule_item(char *from, char *to, char *subject, char *_size, int size, char *attachment_type, char *_attachment_size, int attachment_size);
char *check_againt_ruleset(struct rule *rule, struct _state *state, int size);
int check_size_rule(int message_size, int size, char *_size);
int check_attachment_rule(struct _state *state, struct rule *rule);
void free_rule(struct rule *rule);

#endif /* _RULES_H */

