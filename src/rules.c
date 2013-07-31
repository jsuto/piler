/*
 * rules.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <piler.h>
#include "rules.h"


void load_rules(struct session_data *sdata, struct __data *data, struct rule **rules, char *table){
   char s[SMALLBUFSIZE];
   char domain[SMALLBUFSIZE], from[SMALLBUFSIZE], to[SMALLBUFSIZE], subject[SMALLBUFSIZE], _size[SMALLBUFSIZE], attachment_type[SMALLBUFSIZE], _attachment_size[SMALLBUFSIZE];
   int size=0, attachment_size=0, spam=0, days=0;

   memset(domain, 0, sizeof(domain));
   memset(from, 0, sizeof(from));
   memset(to, 0, sizeof(to));
   memset(subject, 0, sizeof(subject));
   memset(_size, 0, sizeof(_size));
   memset(attachment_type, 0, sizeof(attachment_type));
   memset(_attachment_size, 0, sizeof(_attachment_size));


   snprintf(s, sizeof(s)-1, "SELECT `domain`, `from`, `to`, `subject`, `_size`, `size`, `attachment_type`, `_attachment_size`, `attachment_size`, `spam`, `days` FROM `%s`", table);

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
   data->sql[data->pos] = &attachment_type[0]; data->type[data->pos] = TYPE_STRING; data->len[data->pos] = sizeof(attachment_type)-2; data->pos++;
   data->sql[data->pos] = &_attachment_size[0]; data->type[data->pos] = TYPE_STRING; data->len[data->pos] = sizeof(_attachment_size)-2; data->pos++;
   data->sql[data->pos] = (char *)&attachment_size; data->type[data->pos] = TYPE_LONG; data->len[data->pos] = sizeof(attachment_size); data->pos++;
   data->sql[data->pos] = (char *)&spam; data->type[data->pos] = TYPE_LONG; data->len[data->pos] = sizeof(spam); data->pos++;
   data->sql[data->pos] = (char *)&days; data->type[data->pos] = TYPE_LONG; data->len[data->pos] = sizeof(days); data->pos++;



   p_store_results(sdata, data->stmt_generic, data);

   while(p_fetch_results(data->stmt_generic) == OK){
      append_rule(rules, domain, from, to, subject, _size, size, attachment_type, _attachment_size, attachment_size, spam, days);

      memset(domain, 0, sizeof(domain));
      memset(from, 0, sizeof(from));
      memset(to, 0, sizeof(to));
      memset(subject, 0, sizeof(subject));
      memset(_size, 0, sizeof(_size));
      memset(attachment_type, 0, sizeof(attachment_type));
      memset(_attachment_size, 0, sizeof(_attachment_size));

      size=0, attachment_size=0, spam=0, days=0;
   }

   p_free_results(data->stmt_generic);

ENDE:
   close_prepared_statement(data->stmt_generic);

}


int append_rule(struct rule **rule, char *domain, char *from, char *to, char *subject, char *_size, int size, char *attachment_type, char *_attachment_size, int attachment_size, int spam, int days){
   struct rule *q, *t, *u=NULL;

   q = *rule;

   while(q){
      u = q;
      q = q->r;
   }

   t = create_rule_item(domain, from, to, subject, _size, size, attachment_type, _attachment_size, attachment_size, spam, days);
   if(t){
      if(*rule == NULL)
         *rule = t;
      else if(u)
         u->r = t;

      return 1;
   }

   return -1;
}


struct rule *create_rule_item(char *domain, char *from, char *to, char *subject, char *_size, int size, char *attachment_type, char *_attachment_size, int attachment_size, int spam, int days){
   struct rule *h=NULL;
   char empty = '\0';
   int len;

   if((h = malloc(sizeof(struct rule))) == NULL)
      return NULL;


   h->compiled = 1;

   h->domain = NULL;
   h->domainlen = strlen(domain);

   if(h->domainlen > 2){
      h->domain = malloc(h->domainlen+2);
      if(h->domain) snprintf(h->domain, h->domainlen, "%s", domain);
      else {
         h->compiled = 0;
         syslog(LOG_INFO, "malloc error in create_rule_item() for '%s'", domain);
      }
   }

   if(!from) from = &empty;
   if(regcomp(&(h->from), from, REG_ICASE | REG_EXTENDED)) h->compiled = 0;

   if(!to) to = &empty;
   if(regcomp(&(h->to), to, REG_ICASE | REG_EXTENDED)) h->compiled = 0;

   if(!subject) subject = &empty;
   if(regcomp(&(h->subject), subject, REG_ICASE | REG_EXTENDED)) h->compiled = 0;

   h->spam = spam;
   h->days = days;

   h->size = size;

   if(!_size) _size = &empty;
   snprintf(h->_size, 3, "%s", _size);


   if(!attachment_type) attachment_type = &empty;
   if(regcomp(&(h->attachment_type), attachment_type, REG_ICASE | REG_EXTENDED)) h->compiled = 0;


   h->attachment_size = attachment_size;

   if(!_attachment_size) _attachment_size = &empty;
   snprintf(h->_attachment_size, 3, "%s", _attachment_size);

   len = strlen(domain)+8 + strlen(from)+6 + strlen(to)+4 + strlen(subject)+9 + strlen(_size)+6 + strlen(attachment_type)+10 + strlen(_attachment_size)+10 + 9 + 15 + 15;
   h->rulestr = malloc(len);



   if(h->rulestr) snprintf(h->rulestr, len-1, "domain=%s,from=%s,to=%s,subject=%s,size%s%d,att.type=%s,att.size%s%d,spam=%d", domain, from, to, subject, _size, size, attachment_type, _attachment_size, attachment_size, spam);
   else h->compiled = 0;

   h->r = NULL;

   return h;
}


char *check_againt_ruleset(struct rule *rule, struct _state *state, int size, int spam){
   size_t nmatch=0;
   struct rule *p;

   p = rule;

   while(p != NULL){

      if(
         p->compiled == 1 &&
         regexec(&(p->from), state->b_from, nmatch, NULL, 0) == 0 &&
         regexec(&(p->to), state->b_to, nmatch, NULL, 0) == 0 &&
         regexec(&(p->subject), state->b_subject, nmatch, NULL, 0) == 0 &&
         check_size_rule(size, p->size, p->_size) == 1 &&
         check_attachment_rule(state, p) == 1 &&
         check_spam_rule(spam, p->spam) == 1
      ){
         return p->rulestr;
      }

      p = p->r;
   }

   return NULL;
}


unsigned long query_retain_period(struct __data *data, struct _state *state, int size, int spam, struct __config *cfg){
   size_t nmatch=0;
   struct rule *p;

   p = data->retention_rules;

   while(p != NULL){

      if(p->domainlen > 2){
         if(strcasestr(state->b_to_domain, p->domain) || strcasestr(state->b_from_domain, p->domain)){
            if(cfg->verbosity >= _LOG_INFO) syslog(LOG_INFO, "from domain: '%s', to domain: '%s', retention days: %d", state->b_from_domain, state->b_to_domain, p->days);
            return p->days * 86400;
         }
      }
      else if (
         p->compiled == 1 &&
         regexec(&(p->from), state->b_from, nmatch, NULL, 0) == 0 &&
         regexec(&(p->to), state->b_to, nmatch, NULL, 0) == 0 &&
         regexec(&(p->subject), state->b_subject, nmatch, NULL, 0) == 0 &&
         check_size_rule(size, p->size, p->_size) == 1 &&
         check_attachment_rule(state, p) == 1 &&
         check_spam_rule(spam, p->spam) == 1
      ){
         if(cfg->verbosity >= _LOG_INFO) syslog(LOG_INFO, "from domain: '%s', to domain: '%s', retention days: %d", state->b_from_domain, state->b_to_domain, p->days);
         return p->days * 86400;
      }

      p = p->r;
   }

   return cfg->default_retention_days * 86400;
}


int check_size_rule(int message_size, int size, char *_size){
   if(size <= 0) return 1;

   if(strcmp(_size, ">") == 0 && message_size > size) return 1;
   if(strcmp(_size, "<") == 0 && message_size < size) return 1;
   if(strcmp(_size, "=") == 0 && message_size == size) return 1;
   if( (strcmp(_size, "<>") == 0 || strcmp(_size, "!=") == 0) && message_size != size) return 1;

   return 0;
}


int check_spam_rule(int is_spam, int spam){
   if(spam == -1) return 1;
   if(is_spam == spam) return 1;
   return 0;
}


int check_attachment_rule(struct _state *state, struct rule *rule){
   int i;
   size_t nmatch=0;

   if(state->n_attachments == 0) return 1;

   for(i=1; i<=state->n_attachments; i++){
      if(
         regexec(&(rule->attachment_type), state->attachments[i].type, nmatch, NULL, 0) == 0 &&
         check_size_rule(state->attachments[i].size, rule->attachment_size, rule->_attachment_size) == 1
      ){
         return 1;
      }
   }

   return 0;
}


void free_rule(struct rule *rule){
   struct rule *p, *q;

   p = rule;

   while(p != NULL){
      q = p->r;

      if(p){
         regfree(&(p->from));
         regfree(&(p->to));
         regfree(&(p->attachment_type));

         free(p->rulestr);

         if(p->domain) free(p->domain);

         free(p);
      }

      p = q;
   }
}



