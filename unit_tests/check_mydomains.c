/*
 * check_mydomains.c, SJ
 */

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <locale.h>
#include <stdbool.h>
#include <assert.h>
#include "../src/piler.h"


char *domains[] = {
   "aaaa.fu",
   "hajaja.kom",
   "auajajau.zzz",
   "akauejujedhj.com",
   "738383.com",
   "27281919.com",
   "uazazahhja.ddd"
};


struct emails {
   char email[SMALLBUFSIZE];
   int match;
};


static void fill_domain_table(struct __config *cfg){
   unsigned int i;
   char buf[SMALLBUFSIZE];
   struct session_data sdata;

   if(open_database(&sdata, cfg) == ERR){
      printf("cannot open database\n");
      return;
   }

   printf("adding testing domains...\n");

   for(i=0; i<sizeof(domains)/sizeof(char*); i++){
      snprintf(buf, sizeof(buf)-1, "insert into domain (domain, mapped) values('%s', '%s')", domains[i], domains[i]);
      p_query(&sdata, buf);
   }

   close_database(&sdata);
}


static void restore_domain_table(struct __config *cfg){
   unsigned int i;
   char buf[SMALLBUFSIZE];
   struct session_data sdata;

   if(open_database(&sdata, cfg) == ERR){
      printf("cannot open database\n");
      return;
   }

   printf("removing testing domains...\n");

   for(i=0; i<sizeof(domains)/sizeof(char*); i++){
      snprintf(buf, sizeof(buf)-1, "delete from domain where domain='%s'", domains[i]);
      p_query(&sdata, buf);
   }

   close_database(&sdata);
}


static void test_mydomains(struct __config *cfg){
   unsigned int i;
   struct session_data sdata;
   struct __data data;
   struct emails emails[] = {
      {"ajaja@aaaa.fu ", 1},
      {"ajahahah@aaa.fu ", 0},
      {"kajajaj@auajajau.zzz", 1},
      {"djdkdjkdjkd@gmail.com ", 0},
      {"akajajajw@738383.com ", 1},
      {"eziezuiru@hotmail.fu ", 0},
      {"mysjdjdjf@uazazahhja.ddd ", 1},
      {"akdieckeeod@auajajau.zzz ", 1},
      {"aukjwdkldkld@acts.hu ", 0},
      {"aujwhcpajdl@fbi.gov.us ", 0},
      {"Uahjajahsjsh@aaaa.fu ", 1},
      {"aujedhkakd@aaaa.biz ", 0},
      {"aujdkdkfi@aaaa.fu ", 1}
   };

   if(open_database(&sdata, cfg) == ERR){
      printf("cannot open database\n");
      return;
   }

   inithash(data.mydomains);

   load_mydomains(&sdata, &data, cfg);

   for(i=0; i<sizeof(emails)/sizeof(struct emails); i++){
      //printf("%s / %d\n", emails[i].email, emails[i].match);
      assert(is_email_address_on_my_domains(emails[i].email, &data) == emails[i].match && "test_mydomains()");
   }

   clearhash(data.mydomains);

   close_database(&sdata);

   printf("test_mydomains() OK\n");
}


int main(){
   struct __config cfg;

   if(!can_i_write_directory(NULL)) __fatal("cannot write current directory!");

   (void) openlog("mydomains_test", LOG_PID, LOG_MAIL);

   cfg = read_config("test.conf");

   setlocale(LC_MESSAGES, cfg.locale);
   setlocale(LC_CTYPE, cfg.locale);

   fill_domain_table(&cfg);

   test_mydomains(&cfg);

   restore_domain_table(&cfg);


   return 0;
}

