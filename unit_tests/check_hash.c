/*
 * check_hash.c, SJ
 */

#include "test.h"
#include "words.h"

struct node *node[MAXHASH];

struct test_data_s_i test_words[] = { 
      { "aaatelergic", 1}, 
      { "aaatelescopic", 1}, 
      { "aaateleseismic", 1}, 
      { "telesiurgicbbb", 1}, 
      { "telesmaticbbb", 1}, 
      { "telesomaticbbb", 1}, 
      { "telXesXthetXic", 0}, 
      { "aaatelesticbbb", 1}, 
      { "aaateleutosporicbbb", 1}, 
      { "telXfairic", 0}, 
      { "telhXarmonic", 0}, 
      { "telXic", 0}, 
      { "aaaa", 0}, 
      { "aaa_bbb", 0}, 
      { "ajaj iii", 0}, 
      { "aaa@bbb", 0}, 
      { "ahah+uuu", 0}, 
      { "ccc&aa", 0}
};


struct test_data_s_uinti test_words_hash[] = {
      { "aaatelergic", 2391918743},
      { "aaatelescopic", 2083954803},
      { "aaateleseismic", 122299487},
      { "telesiurgicbbb", 1265893195},
      { "telesmaticbbb", 3159428598},
      { "telesomaticbbb", 2785346981},
      { "telXesXthetXic", 2996605099},
      { "aaatelesticbbb", 708743179},
      { "aaateleutosporicbbb", 78891264},
      { "telXfairic", 375384208},
      { "telhXarmonic", 2664510067},
      { "telXic", 500553166},
      { "aaaa", 2090068425},
      { "aaa_bbb", 400954957},
      { "ajaj iii", 3390570070},
      { "aaa@bbb", 399840910},
      { "ahah+uuu", 805671073},
      { "ccc&aa", 4127468790}
};


static void test_init_hash(){
   unsigned int i;

   TEST_HEADER();

   inithash(node);

   for(i=0; i<MAXHASH; i++){
      ASSERT(node[i] == NULL, "not null");
   }

   TEST_FOOTER();
}


static void test_clear_hash(){
   unsigned int i;

   TEST_HEADER();

   clearhash(node);

   for(i=0; i<MAXHASH; i++){
      ASSERT(node[i] == NULL, "not null");
   }

   TEST_FOOTER();
}


static void test_addnode(){
   unsigned int i, nwords = sizeof(words)/sizeof(char*);

   TEST_HEADER();

   for(i=0; i<nwords; i++){
      addnode(node, words[i]);
   }   

   for(i=0; i<nwords; i++){
      ASSERT(findnode(node, words[i]) != NULL, words[i]);
   }

   TEST_FOOTER();
}


static void test_is_substr_in_hash(){
   unsigned int i;

   TEST_HEADER();

   for(i=0; i<sizeof(test_words)/sizeof(struct test_data_s_i); i++){
      ASSERT(is_substr_in_hash(node, test_words[i].s) == test_words[i].result, test_words[i].s);
   }

   TEST_FOOTER();
}


static void test_DJBHash(){
   unsigned int i;

   TEST_HEADER();

   for(i=0; i<sizeof(test_words_hash)/sizeof(struct test_data_s_uinti); i++){
      ASSERT(DJBHash(test_words_hash[i].s, strlen(test_words_hash[i].s)) == test_words_hash[i].result, test_words_hash[i].s);
   }

   TEST_FOOTER();
}


int main(){
   test_init_hash();
   test_addnode();
   test_is_substr_in_hash();
   test_clear_hash();
   test_DJBHash();

   return 0;
}
