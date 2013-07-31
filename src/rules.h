/*
 * rules.h, SJ
 */

#ifndef _RULES_H
 #define _RULES_H

#include "defs.h"

void load_rules(struct session_data *sdata, struct __data *data, struct rule **rules, char *table);
int append_rule(struct rule **rule, char *domain, char *from, char *to, char *subject, char *_size, int size, char *attachment_type, char *_attachment_size, int attachment_size, int spam, int days);
struct rule *create_rule_item(char *domain, char *from, char *to, char *subject, char *_size, int size, char *attachment_type, char *_attachment_size, int attachment_size, int spam, int days);
char *check_againt_ruleset(struct rule *rule, struct _state *state, int size, int spam);
unsigned long query_retain_period(struct __data *data, struct _state *state, int size, int spam, struct __config *cfg);
int check_size_rule(int message_size, int size, char *_size);
int check_spam_rule(int is_spam, int spam);
int check_attachment_rule(struct _state *state, struct rule *rule);
void free_rule(struct rule *rule);

#endif /* _RULES_H */

