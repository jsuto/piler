/*
 * rules.h, SJ
 */

#ifndef _RULES_H
 #define _RULES_H

#include "defs.h"

void load_archiving_rules(struct session_data *sdata, struct rule **rules);
int append_rule(struct rule **rule, char *from, char *to, char *subject, char *_size, int size);
struct rule *create_rule_item(char *from, char *to, char *subject, char *_size, int size);
char *check_againt_ruleset(struct rule *rule, char *from, char *to, char *subject, int size);
int check_size_rule(int message_size, int size, char *_size);
void free_rule(struct rule *rule);

#endif /* _RULES_H */

