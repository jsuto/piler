/*
 * rules.h, SJ
 */

#ifndef _RULES_H
 #define _RULES_H

#include "defs.h"

void load_rules(struct session_data *sdata, struct __data *data, struct node *xhash[], char *table, struct __config *cfg);
int append_rule(struct node *xhash[], struct rule_cond *rule_cond, struct __data *data);
struct rule *create_rule_item(struct rule_cond *rule_cond, struct __data *data);
char *check_againt_ruleset(struct node *xhash[], struct parser_state *state, int size, int spam);
time_t query_retain_period(struct __data *data, struct parser_state *state, int size, int spam, struct __config *cfg);
int get_folder_id_by_rule(struct __data *data, struct parser_state *state, int size, int spam, struct __config *cfg);
int check_size_rule(int message_size, int size, char *_size);
int check_spam_rule(int is_spam, int spam);
int check_attachment_rule(struct parser_state *state, struct rule *rule);

void initrules(struct node *xhash[]);
void clearrules(struct node *xhash[]);

#endif /* _RULES_H */

