/*
 * rules.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <piler.h>
#include "rules.h"


void load_archiving_rules(struct session_data *sdata, struct rule **rules){
   char s[SMALLBUFSIZE];
   MYSQL_RES *res;
   MYSQL_ROW row;

   snprintf(s, sizeof(s)-1, "SELECT `from`, `to`, `subject`, `_size`, `size` FROM `%s`", SQL_ARCHIVING_RULE_TABLE);

   if(mysql_real_query(&(sdata->mysql), s, strlen(s)) == 0){
      res = mysql_store_result(&(sdata->mysql));
      if(res != NULL){
         while((row = mysql_fetch_row(res))){
            append_rule(rules, (char*)row[0], (char*)row[1], (char*)row[2], (char*)row[3], atoi(row[4]));
         }

         mysql_free_result(res);
      }

   }

}


int append_rule(struct rule **rule, char *from, char *to, char *subject, char *_size, int size){
   struct rule *q, *t, *u=NULL;

   q = *rule;

   while(q){
      u = q;
      q = q->r;
   }

   t = create_rule_item(from, to, subject, _size, size);
   if(t){
      if(*rule == NULL)
         *rule = t;
      else if(u)
         u->r = t;

      return 1;
   }

   return -1;
}


struct rule *create_rule_item(char *from, char *to, char *subject, char *_size, int size){
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

   h->size = size;

   if(!_size) _size = &empty;
   snprintf(h->_size, 3, "%s", _size);

   len = strlen(from)+6 + strlen(to)+4 + strlen(subject)+9 + strlen(_size)+6 + 15;
   h->rulestr = malloc(len);

   if(h->rulestr) snprintf(h->rulestr, len-1, "from=%s,to=%s,subject=%s,size%s%d", from, to, subject, _size, size);
   else h->compiled = 0;

   h->r = NULL;

   return h;
}


char *check_againt_ruleset(struct rule *rule, char *from, char *to, char *subject, int size){
   size_t nmatch=0;
   struct rule *p;

   p = rule;

   while(p != NULL){

      if(
         p->compiled == 1 &&
         regexec(&(p->from), from, nmatch, NULL, 0) == 0 &&
         regexec(&(p->to), to, nmatch, NULL, 0) == 0 &&
         regexec(&(p->subject), subject, nmatch, NULL, 0) == 0 &&
         check_size_rule(size, p->size, p->_size) == 1
      ){
         return p->rulestr;
      }

      p = p->r;
   }

   return NULL;
}


int check_size_rule(int message_size, int size, char *_size){
   if(size <= 0) return 1;

   if(strcmp(_size, ">") == 0 && message_size > size) return 1;
   if(strcmp(_size, "<") == 0 && message_size < size) return 1;
   if(strcmp(_size, "=") == 0 && message_size == size) return 1;
   if( (strcmp(_size, "<>") == 0 || strcmp(_size, "!=") == 0) && message_size != size) return 1;

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
         free(p->rulestr);

         free(p);
      }

      p = q;
   }
}



