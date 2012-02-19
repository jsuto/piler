/*
 * rules.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <piler.h>
#include "rules.h"


void load_rules(struct session_data *sdata, struct rule **rules, char *table){
   char s[SMALLBUFSIZE];
   MYSQL_RES *res;
   MYSQL_ROW row;

   snprintf(s, sizeof(s)-1, "SELECT `from`, `to`, `subject`, `_size`, `size`, `attachment_type`, `_attachment_size`, `attachment_size`, `spam`, `days` FROM `%s`", table);

   if(mysql_real_query(&(sdata->mysql), s, strlen(s)) == 0){
      res = mysql_store_result(&(sdata->mysql));
      if(res != NULL){
         while((row = mysql_fetch_row(res))){
            append_rule(rules, (char*)row[0], (char*)row[1], (char*)row[2], (char*)row[3], atoi(row[4]), (char*)row[5], (char*)row[6], atoi(row[7]), atoi(row[8]), atoi(row[9]));
         }

         mysql_free_result(res);
      }

   }

}


int append_rule(struct rule **rule, char *from, char *to, char *subject, char *_size, int size, char *attachment_type, char *_attachment_size, int attachment_size, int spam, int days){
   struct rule *q, *t, *u=NULL;

   q = *rule;

   while(q){
      u = q;
      q = q->r;
   }

   t = create_rule_item(from, to, subject, _size, size, attachment_type, _attachment_size, attachment_size, spam, days);
   if(t){
      if(*rule == NULL)
         *rule = t;
      else if(u)
         u->r = t;

      return 1;
   }

   return -1;
}


struct rule *create_rule_item(char *from, char *to, char *subject, char *_size, int size, char *attachment_type, char *_attachment_size, int attachment_size, int spam, int days){
   struct rule *h=NULL;
   char empty = '\0';
   int len;

   if((h = malloc(sizeof(struct rule))) == NULL)
      return NULL;


   h->compiled = 1;

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

   len = strlen(from)+6 + strlen(to)+4 + strlen(subject)+9 + strlen(_size)+6 + strlen(attachment_type)+10 + strlen(_attachment_size)+10 + 8 + 15 + 15;
   h->rulestr = malloc(len);



   if(h->rulestr) snprintf(h->rulestr, len-1, "from=%s,to=%s,subject=%s,size%s%d,att.type=%s,att.size%s%d,spam=%d", from, to, subject, _size, size, attachment_type, _attachment_size, attachment_size, spam);
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


unsigned long query_retain_period(struct rule *rule, struct _state *state, int size, int spam, struct __config *cfg){
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

         free(p);
      }

      p = q;
   }
}



