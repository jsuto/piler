/*
 * check_rules.c, SJ
 */

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <locale.h>
#include <stdbool.h>
#include <assert.h>
#include "../src/piler.h"


struct digest_test {
   char s[SMALLBUFSIZE];
   char *digest1;
   char *digest2;
};


struct digest_test tests[] = {
   {"1.eml", "b675536ea7ba16adf9b19ccc2ef60351c2f82b1fa7b92fb18569bd3ba753cc60", "bcd87663be92f688d9cccb62e6e0504ee3f7cdec8e5b7679f8724bacc3b0c57f"},
   {"2.eml", "668cb3b91b944af786667323442576b9813d65f3cd3bc33e9d5da303c79de038", "de90475409dd6ab24e80c1b7a987715c40fe8d28d91337b7f063b477159c7b3c"},
   {"3.eml", "0d546d4cb4a8ce74ea5fd4cc51dbb4ebeaa7542f1c691817579da7eeab8d4771", "f585d011340d292ee52ddedb07cda662a8f1e46329d14a2ce92dca0604387bab"},
   {"4.eml", "6008daed3613af4af71aa1f82550fd592d19c14c1aa7d8d5db2be1552174c518", "5f0744384bc3f96167380442c108c04a762313c79778c928ee5274981632ff45"},
   {"5.eml", "63e4649b52c0fb45571b32aecc502234e6414abb36317eea25a1bd2f4e3cbe9e", "04d6ae67d34b2d5c41cf96357b8eb3954ebd570943cd83cfb1db028801806efd"},
   {"6.eml", "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", "a01d130e2b5761bf28cddd116c00fe9b3982bfbeb4c629c569ccfea6ba965ce4"},
   {"7.eml", "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", "ca1597c1309b5b8418ccd5981a4b7672a0f3b0989720891eb3ba2a47069206b4"},
   {"8.eml", "01ba4719c80b6fe911b091a7c05124b64eeece964e09c058ef8f9805daca546b", "e3496806ff20e5075baf8b2ede02d1613acfeb261f0b3108eeeba92347d93e72"}
};


static void test_digest_string(){
   unsigned int i;
   char digest[2*DIGEST_LENGTH+1];
   struct digest_test tests[] = {
      {"Piler archives every email it receives.", "68bcdb6f15eeabdcedce3e4fc8faf7eb620272ebd55f365d08aca40adf18fe83", ""},
      {"It has a built-in access control to prevent a user to access other's messages.", "3acc35b154420e8749e7ebd9adc3a397d3ecfd86c891ecb87b7718e146d314d5", ""},
      {"Auditors can see every archived email.", "619e008882474f26a0fee3699d4694f256db116204ebd5d436052052beed60c3", ""},
      {"Piler parses the header and extracts the From:, To: and Cc: addresses (in case of From: it only stores the first email address, since some spammers include tons of addresses in the From: field), and when a user searches for his emails then piler tries to match his email addresses against the email addresses in the messages.", "ab1cc87020ffe78d310481381a42064601337b388b8804309b8f4a52cfd55e0f", ""},
      {"To sum it up, a regular user can see only the emails he sent or received.", "987282a0c946db345afc63d15e74b0ec53ac776b3ec638540befa9b9b64f8b02", ""}
   };

   for(i=0; i<sizeof(tests)/sizeof(struct digest_test); i++){
      digest_string(tests[i].s, &digest[0]);
      assert(strcmp(digest, tests[i].digest1) == 0 && "test_digest_string()");
   }

   printf("test_digest_string() OK\n");
}


static void test_digest_file(){
   unsigned int i;
   char digest[2*DIGEST_LENGTH+1];

   for(i=0; i<sizeof(tests)/sizeof(struct digest_test); i++){
      digest_file(tests[i].s, &digest[0]);
      assert(strcmp(digest, tests[i].digest2) == 0 && "test_digest_file()");
   }

   printf("test_digest_file() OK\n");
}


static void test_make_digests(struct __config *cfg){
   unsigned int i;
   int j;
   struct session_data sdata;
   struct parser_state state;
   struct __data data;
   struct stat st;


   if(open_database(&sdata, cfg) == ERR){
      printf("cannot open database\n");
      return;
   }


   for(i=0; i<sizeof(tests)/sizeof(struct digest_test); i++){

      if(stat(tests[i].s, &st) != 0){
         fprintf(stderr, "%s is not found, skipping\n", tests[i].s);
         continue;
      }

      init_session_data(&sdata, cfg);
 
      sdata.delivered = 0;
      sdata.tot_len = st.st_size;

      snprintf(sdata.ttmpfile, SMALLBUFSIZE-1, "%s", tests[i].s);
      snprintf(sdata.filename, SMALLBUFSIZE-1, "%s", tests[i].s);
      snprintf(sdata.tmpframe, SMALLBUFSIZE-1, "%s.m", tests[i].s);

      state = parse_message(&sdata, 1, &data, cfg);
      post_parse(&sdata, &state, cfg);

      make_digests(&sdata, cfg);

      //printf("%s => body digest: %s, digest: %s\n", tests[i].s, sdata.bodydigest, sdata.digest);

      for(j=1; j<=state.n_attachments; j++){
         unlink(state.attachments[j].internalname);
      }

      unlink(sdata.tmpframe);

      assert(strcmp(sdata.bodydigest, tests[i].digest1) == 0 && "test_make_digests()");
      assert(strcmp(sdata.digest, tests[i].digest2) == 0 && "test_make_digests()");

   }

   close_database(&sdata);

   printf("test_make_digests() OK\n");
}


int main(){
   struct __config cfg;

   (void) openlog("digest_test", LOG_PID, LOG_MAIL);

   cfg = read_config("test.conf");

   setlocale(LC_MESSAGES, cfg.locale);
   setlocale(LC_CTYPE, cfg.locale);


   test_digest_string();
   test_digest_file();

   test_make_digests(&cfg);

   return 0;
}

