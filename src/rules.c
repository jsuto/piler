/*
 * rules.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <piler.h>
#include "rules.h"


void load_rules(struct session_data *sdata, struct __data *data, struct node *xhash[], char *table){
   char s[SMALLBUFSIZE];
   char domain[SMALLBUFSIZE], from[SMALLBUFSIZE], to[SMALLBUFSIZE], subject[SMALLBUFSIZE], _size[SMALLBUFSIZE], attachment_name[SMALLBUFSIZE], attachment_type[SMALLBUFSIZE], _attachment_size[SMALLBUFSIZE];
   int size=0, attachment_size=0, spam=0, days=0;

   memset(domain, 0, sizeof(domain));
   memset(from, 0, sizeof(from));
   memset(to, 0, sizeof(to));
   memset(subject, 0, sizeof(subject));
   memset(_size, 0, sizeof(_size));
   memset(attachment_name, 0, sizeof(attachment_name));
   memset(attachment_type, 0, sizeof(attachment_type));
   memset(_attachment_size, 0, sizeof(_attachment_size));


   snprintf(s, sizeof(s)-1, "SELECT `domain`, `from`, `to`, `subject`, `_size`, `size`, `attachment_name`, `attachment_type`, `_attachment_size`, `attachment_size`, `spam`, `days` FROM `%s`", table);

   if(prepare_sql_statement(sdata, &(data->stmt_generic), s) == ERR) return;


   p_bind_init(data);

   if(p_exec_query(sdata, data->stmt_generic, data) == ERR) goto ENDE;



   p_bind_init(data);

   data->sql[data->pos] = &domain[0]; data->type[data->pos] = TYPE_STRING; data->len[data->pos] = sizeof(domain)-2; data->pos++;
   data->sql[data->pos] = &from[0]; data->type[data->pos] = TYPE_STRING; data->len[data->pos] = sizeof(from)-2; data->pos++;
   data->sql[data->pos] = &to[0]; data->type[data->pos] = TYPE_STRING; data->len[data->pos] = sizeof(to)-2; data->pos++;
   data->sql[data->pos] = &subject[0]; data->type[data->pos] = TYPE_STRING; data->len[data->pos] = sizeof(subject)-2; data->pos++;
   data->sql[data->pos] = &_size[0]; data->type[data->pos] = TYPE_STRING; data->len[data->pos] = sizeof(_size)-2; data->pos++;
   data->sql[data->pos] = (char *)&size; data->type[data->pos] = TYPE_LONG; data->len[data->pos] = sizeof(size); data->pos++;
   data->sql[data->pos] = &attachment_name[0]; data->type[data->pos] = TYPE_STRING; data->len[data->pos] = sizeof(attachment_name)-2; data->pos++;
   data->sql[data->pos] = &attachment_type[0]; data->type[data->pos] = TYPE_STRING; data->len[data->pos] = sizeof(attachment_type)-2; data->pos++;
   data->sql[data->pos] = &_attachment_size[0]; data->type[data->pos] = TYPE_STRING; data->len[data->pos] = sizeof(_attachment_size)-2; data->pos++;
   data->sql[data->pos] = (char *)&attachment_size; data->type[data->pos] = TYPE_LONG; data->len[data->pos] = sizeof(attachment_size); data->pos++;
   data->sql[data->pos] = (char *)&spam; data->type[data->pos] = TYPE_LONG; data->len[data->pos] = sizeof(spam); data->pos++;
   data->sql[data->pos] = (char *)&days; data->type[data->pos] = TYPE_LONG; data->len[data->pos] = sizeof(days); data->pos++;



   p_store_results(sdata, data->stmt_generic, data);

   while(p_fetch_results(data->stmt_generic) == OK){
      append_rule(xhash, domain, from, to, subject, _size, size, attachment_name, attachment_type, _attachment_size, attachment_size, spam, days, data);

      memset(domain, 0, sizeof(domain));
      memset(from, 0, sizeof(from));
      memset(to, 0, sizeof(to));
      memset(subject, 0, sizeof(subject));
      memset(_size, 0, sizeof(_size));
      memset(attachment_name, 0, sizeof(attachment_name));
      memset(attachment_type, 0, sizeof(attachment_type));
      memset(_attachment_size, 0, sizeof(_attachment_size));

      size=0; attachment_size=0; spam=0; days=0;
   }

   p_free_results(data->stmt_generic);

ENDE:
   close_prepared_statement(data->stmt_generic);

}


int append_rule(struct node *xhash[], char *domain, char *from, char *to, char *subject, char *_size, int size, char *attachment_name, char *attachment_type, char *_attachment_size, int attachment_size, int spam, int days, struct __data *data){
   struct node *q, *Q=NULL, *node;
   struct rule *rule;
   int rc=0;

   if((node = malloc(sizeof(struct node))) == NULL) return rc;

   memset(node, 0, sizeof(struct node));
   node->r = NULL;

   rule = create_rule_item(domain, from, to, subject, _size, size, attachment_name, attachment_type, _attachment_size, attachment_size, spam, days, data);

   if(rule == NULL){
      free(node);
      syslog(LOG_INFO, "could not load rule=%s/%s/%s/%s/%s,%d", domain, from, to, subject, _size, size);
      return rc;
   }

   node->str = rule;

   q = xhash[0];

   while(q != NULL){
      Q = q;
      q = q->r;
   }

   if(Q == NULL) xhash[0] = node;
   else {
      Q->r = node;
   }

   rc = 1;

   return rc;
}


struct rule *create_rule_item(char *domain, char *from, char *to, char *subject, char *_size, int size, char *attachment_name, char *attachment_type, char *_attachment_size, int attachment_size, int spam, int days, struct __data *data){
   struct rule *h=NULL;
   char empty = '\0';
   int len;

   if((h = malloc(sizeof(struct rule))) == NULL)
      return NULL;


   h->compiled = 1;

   h->domain = NULL;
   h->domainlen = data->length[0];

   if(h->domainlen > 2){
      h->domain = malloc(h->domainlen+2);
      if(h->domain) snprintf(h->domain, h->domainlen, "%s", domain);
      else {
         h->compiled = 0;
         syslog(LOG_INFO, "malloc error in create_rule_item() for '%s'", domain);
      }
   }

   h->emptyfrom = h->emptyto = h->emptysubject = h->emptyaname = h->emptyatype = 0;


   if(!from || strlen(from) < 1){ from = &empty; h->emptyfrom = 1; }
   if(regcomp(&(h->from), from, REG_ICASE | REG_EXTENDED)) h->compiled = 0;

   if(!to || strlen(to) < 1){ to = &empty; h->emptyto = 1; }
   if(regcomp(&(h->to), to, REG_ICASE | REG_EXTENDED)) h->compiled = 0;

   if(!subject || strlen(subject) < 1){ subject = &empty; h->emptysubject = 1; }
   if(regcomp(&(h->subject), subject, REG_ICASE | REG_EXTENDED)) h->compiled = 0;

   h->spam = spam;
   h->days = days;

   h->size = size;

   if(!_size) _size = &empty;
   snprintf(h->_size, 3, "%s", _size);

   if(!attachment_name || strlen(attachment_name) < 1){ attachment_name = &empty; h->emptyaname = 1; }
   if(regcomp(&(h->attachment_name), attachment_name, REG_ICASE | REG_EXTENDED)) h->compiled = 0;

   if(!attachment_type || strlen(attachment_type) < 1){ attachment_type = &empty; h->emptyatype = 1; }
   if(regcomp(&(h->attachment_type), attachment_type, REG_ICASE | REG_EXTENDED)) h->compiled = 0;


   h->attachment_size = attachment_size;

   if(!_attachment_size) _attachment_size = &empty;
   snprintf(h->_attachment_size, 3, "%s", _attachment_size);

   len = strlen(domain)+8 + strlen(from)+6 + strlen(to)+4 + strlen(subject)+9 + strlen(_size)+6 + strlen(attachment_name)+10 + strlen(attachment_type)+10 + strlen(_attachment_size)+10 + 9 + 15 + 15;
   h->rulestr = malloc(len);



   if(h->rulestr) snprintf(h->rulestr, len-1, "domain=%s,from=%s,to=%s,subject=%s,size%s%d,att.name=%s,att.type=%s,att.size%s%d,spam=%d", domain, from, to, subject, _size, size, attachment_name, attachment_type, _attachment_size, attachment_size, spam);
   else h->compiled = 0;

   h->r = NULL;

   return h;
}


char *check_againt_ruleset(struct node *xhash[], struct _state *state, int size, int spam){
   size_t nmatch=0;
   struct rule *p;
   struct node *q;
   int ismatch;

   q = xhash[0];

   while(q != NULL){

      if(q->str){
         p = q->str;

         if(p){
            ismatch = 0;

            ismatch += check_spam_rule(spam, p->spam);
            ismatch += check_size_rule(size, p->size, p->_size);
            ismatch += check_attachment_rule(state, p);

            if(p->compiled == 1){
               if(p->emptyfrom == 1){
                  ismatch += RULE_UNDEF;
               }
               else if(regexec(&(p->from), state->b_from, nmatch, NULL, 0) == 0) ismatch += RULE_MATCH; else ismatch += RULE_NO_MATCH;

               if(p->emptyto == 1){
                  ismatch += RULE_UNDEF;
               }
               else if(regexec(&(p->to), state->b_to, nmatch, NULL, 0) == 0) ismatch += RULE_MATCH; else ismatch += RULE_NO_MATCH;

               if(p->emptysubject == 1){
                  ismatch += RULE_UNDEF;
               }
               else if(regexec(&(p->subject), state->b_subject, nmatch, NULL, 0) == 0) ismatch += RULE_MATCH; else ismatch += RULE_NO_MATCH;
            }

            if(ismatch > 0){
               return p->rulestr;
            }

         }
      }

      q = q->r;
   }

   return NULL;
}


unsigned long query_retain_period(struct __data *data, struct _state *state, int size, int spam, struct __config *cfg){
   size_t nmatch=0;
   struct rule *p;
   struct node *q;
   int ismatch;

   q = data->retention_rules[0];

   while(q != NULL){

      if(q->str){
         p = q->str;

         ismatch = 0;

         if(p->domainlen > 2){
            if(strcasestr(state->b_to_domain, p->domain) || strcasestr(state->b_from_domain, p->domain)){
               state->retention = p->days;
               return p->days * 86400;
            }
         }
         else {

            ismatch += check_spam_rule(spam, p->spam);
            ismatch += check_size_rule(size, p->size, p->_size);
            ismatch += check_attachment_rule(state, p);

            if(p->compiled == 1){
               if(p->emptyfrom == 1){
                  ismatch += RULE_UNDEF;
               }
               else if(regexec(&(p->from), state->b_from, nmatch, NULL, 0) == 0) ismatch += RULE_MATCH; else ismatch += RULE_NO_MATCH;

               if(p->emptyto == 1){
                  ismatch += RULE_UNDEF;
               }
               else if(regexec(&(p->to), state->b_to, nmatch, NULL, 0) == 0) ismatch += RULE_MATCH; else ismatch += RULE_NO_MATCH;

               if(p->emptysubject == 1){
                  ismatch += RULE_UNDEF;
               }
               else if(regexec(&(p->subject), state->b_subject, nmatch, NULL, 0) == 0) ismatch += RULE_MATCH; else ismatch += RULE_NO_MATCH;
            }

            if(ismatch > 0){
               state->retention = p->days;
               return p->days * 86400;
            }
         }

      }

      q = q->r;
   }


   state->retention = cfg->default_retention_days;

   return cfg->default_retention_days * 86400;
}


int check_size_rule(int message_size, int size, char *_size){
   if(size <= 0) return RULE_UNDEF;

   if(strcmp(_size, ">") == 0 && message_size > size) return RULE_MATCH;
   if(strcmp(_size, "<") == 0 && message_size < size) return RULE_MATCH;
   if(strcmp(_size, "=") == 0 && message_size == size) return RULE_MATCH;
   if( (strcmp(_size, "<>") == 0 || strcmp(_size, "!=") == 0) && message_size != size) return RULE_MATCH;

   return RULE_NO_MATCH;
}


int check_spam_rule(int is_spam, int spam){
   if(spam == -1) return RULE_UNDEF;
   if(is_spam == spam) return RULE_MATCH;
   return RULE_NO_MATCH;
}


int check_attachment_rule(struct _state *state, struct rule *rule){
   int i;
   size_t nmatch=0;
   int ismatch = 0;

   if(state->n_attachments == 0) return RULE_UNDEF;

   if(rule->emptyaname == 1 && rule->emptyatype == 1) return RULE_UNDEF;

   for(i=1; i<=state->n_attachments; i++){
      ismatch = 0;

      if(rule->emptyaname == 0){
         if(regexec(&(rule->attachment_name), state->attachments[i].filename, nmatch, NULL, 0) == 0)
            ismatch += RULE_MATCH;
         else
            ismatch += RULE_NO_MATCH;
      }

      if(rule->emptyatype == 0){
         if(regexec(&(rule->attachment_type), state->attachments[i].type, nmatch, NULL, 0) == 0)
            ismatch += RULE_MATCH;
         else
            ismatch += RULE_NO_MATCH;
      }

      ismatch += check_size_rule(state->attachments[i].size, rule->attachment_size, rule->_attachment_size);

      if(ismatch > 0) return RULE_MATCH;
   }

   return RULE_NO_MATCH;
}


void initrules(struct node *xhash[]){
   xhash[0] = NULL;
}


void clearrules(struct node *xhash[]){
   struct node *p, *q;
   struct rule *rule;

   q = xhash[0];

   while(q != NULL){
      p = q;
      q = q->r;

      if(p){
         if(p->str){
            rule = (struct rule*)p->str;

            regfree(&(rule->from));
            regfree(&(rule->to));
            regfree(&(rule->attachment_name));
            regfree(&(rule->attachment_type));

            free(rule->rulestr);

            if(rule->domain) free(rule->domain);

            free(rule);
         }
         free(p);
      }
   }

   xhash[0] = NULL;
}

