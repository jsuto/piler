/*
 * check_rules.c, SJ
 */

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <locale.h>
#include <stdbool.h>
#include <assert.h>
#include "../src/piler.h"


struct rule_test {
   char filename[SMALLBUFSIZE];
   char *expected_result;
   int days;
};


struct rule_query {
   char query[SMALLBUFSIZE];
   uint64 id;
};


struct rule_query rules[] = {
   {"insert into archiving_rule (`from`, `days`) values('hirlevel@jatekokbirodalma.hu', 101)", 0},
   {"insert into archiving_rule (`to`, `days`) values('xxx@acts.hu', 102)", 0},
   {"insert into archiving_rule (`to`, `subject`, `days`) values('sj@acts.hu', 'ultra slim', 103)", 0},
   {"insert into archiving_rule (`body`, `days`) values('quality and price is very good', 104)", 0},
   {"insert into archiving_rule (`attachment_type`, _attachment_size, attachment_size, `days`) values('image', '>', 2000000, 105)", 0},
   {"insert into archiving_rule (`attachment_type`, _attachment_size, attachment_size, `days`) values('image', '>', 20000, 106)", 0}
};


static void fill_rule_table(struct __config *cfg){
   int i;
   struct session_data sdata;

   if(open_database(&sdata, cfg) == ERR){
      printf("cannot open database\n");
      return;
   }

   printf("adding testing rules...\n");

   for(i=0; i<sizeof(rules)/sizeof(struct rule_query); i++){
      p_query(&sdata, rules[i].query, cfg);
      rules[i].id = mysql_insert_id(&(sdata.mysql)); 
   }

   close_database(&sdata);
}


static void restore_rule_table(struct __config *cfg){
   int i;
   char buf[SMALLBUFSIZE];
   struct session_data sdata;

   if(open_database(&sdata, cfg) == ERR){
      printf("cannot open database\n");
      return;
   }

   printf("removing testing rules...\n");

   for(i=0; i<sizeof(rules)/sizeof(struct rule_query); i++){
      snprintf(buf, sizeof(buf)-1, "delete from archiving_rule where id=%llu", rules[i].id);
      p_query(&sdata, buf, cfg);
   }

   close_database(&sdata);
}



static void test_archiving_rule(struct __config *cfg){
   int i, j;
   char *rule;
   struct session_data sdata;
   struct parser_state state;
   struct __data data;
   struct stat st;
   struct rule_test rule_test[] = {
      {"1.eml", "domain=,from=hirlevel@jatekokbirodalma.hu,to=,subject=,body=,size0,att.name=,att.type=,att.size0,spam=-1", 0},
      {"2.eml", "domain=,from=,to=sj@acts.hu,subject=ultra slim,body=,size0,att.name=,att.type=,att.size0,spam=-1", 0},
      {"3.eml", NULL, 0},
      {"4.eml", "domain=,from=,to=,subject=,body=quality and price is very good,size0,att.name=,att.type=,att.size0,spam=-1", 0},
      {"5.eml", "domain=,from=,to=,subject=,body=,size0,att.name=,att.type=image,att.size>20000,spam=-1", 0}
   };


   if(open_database(&sdata, cfg) == ERR){
      printf("cannot open database\n");
      return;
   }

   inithash(data.mydomains);
   load_mydomains(&sdata, &data, cfg);

   initrules(data.archiving_rules);

   load_rules(&sdata, &data, data.archiving_rules, SQL_ARCHIVING_RULE_TABLE, cfg);

   for(i=0; i<sizeof(rule_test)/sizeof(struct rule_test); i++){

      if(stat(rule_test[i].filename, &st) != 0){
         fprintf(stderr, "%s is not found, skipping\n", rule_test[i].filename);
         continue;
      }

      init_session_data(&sdata, cfg);
 
      sdata.delivered = 0;
      sdata.tot_len = st.st_size;

      snprintf(sdata.ttmpfile, SMALLBUFSIZE-1, "%s", rule_test[i].filename);
      snprintf(sdata.filename, SMALLBUFSIZE-1, "%s", rule_test[i].filename);
      snprintf(sdata.tmpframe, SMALLBUFSIZE-1, "%s.m", rule_test[i].filename);

      state = parse_message(&sdata, 1, &data, cfg);
      post_parse(&sdata, &state, cfg);

      rule = check_againt_ruleset(data.archiving_rules, &state, st.st_size, sdata.spam_message);


      for(j=1; j<=state.n_attachments; j++){
         unlink(state.attachments[j].internalname);
      }

      unlink(sdata.tmpframe);

      //printf("%s, '%s'\n", rule_test[i].filename, rule);

      if(rule == NULL && rule == rule_test[i].expected_result) continue;

      assert(strcmp(rule, rule_test[i].expected_result) == 0 && "test_archiving_rule()");

   }

   clearrules(data.archiving_rules);

   clearhash(data.mydomains);

   close_database(&sdata);

   printf("test_archiving_rule() OK\n");
}




int main(){
   struct __config cfg;

   if(!can_i_write_current_directory()) __fatal("cannot write current directory!");

   (void) openlog("rule_test", LOG_PID, LOG_MAIL);

   cfg = read_config("test.conf");

   setlocale(LC_MESSAGES, cfg.locale);
   setlocale(LC_CTYPE, cfg.locale);

   fill_rule_table(&cfg);


   test_archiving_rule(&cfg);

   restore_rule_table(&cfg);


   return 0;
}

