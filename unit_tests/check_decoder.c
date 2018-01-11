/*
 * check_decoder.c, SJ
 */

#include "test.h"

struct config cfg;

static void test_decodeBase64(){
   unsigned int i;
   struct test_data_s_s test_strings[] = {
      { "Zm9vYmFy", "foobar" },
      { "Zm9vYmE=", "fooba" },
      { "Zm9vYg==", "foob" },
      { "Zm9v", "foo" },
      { "Zm8=", "fo" },
      { "Zg==", "f" },
      { "", "" },
   };

   TEST_HEADER();

   for(i=0; i<sizeof(test_strings)/sizeof(struct test_data_s_s); i++){
      decodeBase64(test_strings[i].s);
      ASSERT(strcmp(test_strings[i].s, test_strings[i].result) == 0, test_strings[i].result);
   }

   TEST_FOOTER();
}


static void test_decode_base64_to_buffer(){
   unsigned int i;
   unsigned char puf[MAXBUFSIZE];
   struct test_data_s_s test_strings[] = { 
      { "Zm9vYmFy", "foobar" },
      { "Zm9vYmE=", "fooba" },
      { "Zm9vYg==", "foob" },
      { "Zm9v", "foo" },
      { "Zm8=", "fo" },
      { "Zg==", "f" },
      { "", "" },
   };  

   TEST_HEADER();

   for(i=0; i<sizeof(test_strings)/sizeof(struct test_data_s_s); i++){
      memset(puf, 0, sizeof(puf));
      decode_base64_to_buffer(test_strings[i].s, strlen(test_strings[i].s), &puf[0], sizeof(puf)-1);
      ASSERT(strcmp((char*)puf, test_strings[i].result) == 0, test_strings[i].result);
   }   

   TEST_FOOTER();
}


static void test_decodeURL(){
   unsigned int i;
   struct test_data_s_s test_strings[] = {
      { "%2fhello%3dworld", "/hello=world" },
      { "hello+world%21", "hello world!" },
      { "%3F%21%3D%25", "?!=%" },
      { "%C3%A9ljen+m%C3%A1jus+elseje", "éljen május elseje" },
      { "", "" },
   };

   TEST_HEADER();

   for(i=0; i<sizeof(test_strings)/sizeof(struct test_data_s_s); i++){
      decodeURL(test_strings[i].s);
      ASSERT(strcmp(test_strings[i].s, test_strings[i].result) == 0, test_strings[i].result);
   }

   TEST_FOOTER();
}


static void test_decodeQP(){
   unsigned int i;
   struct test_data_s_s test_strings[] = { 
      { "=2fhello=3dworld", "/hello=world" },
      { "hello=20world=21", "hello world!" },
      { "=3F=21=3D=25", "?!=%" },
      { "=C3=A9ljen=20m=C3=A1jus=20elseje", "éljen május elseje" },
      { "", "" },
   };  

   TEST_HEADER();

   for(i=0; i<sizeof(test_strings)/sizeof(struct test_data_s_s); i++){
      decodeQP(test_strings[i].s);
      ASSERT(strcmp(test_strings[i].s, test_strings[i].result) == 0, test_strings[i].result);
   }   

   TEST_FOOTER();
}


static void test_decodeHTML(){
   unsigned int i;
   struct test_data_s_s test_strings[] = {
      { "1&gt;2", "1>2" },
      { "&eacute;ljen m&aacute;jus elseje!", "éljen május elseje!" },
      { "boni&amp;kl&aacute;jd", "boni&klájd" },
   };

   TEST_HEADER();

   for(i=0; i<sizeof(test_strings)/sizeof(struct test_data_s_s); i++){
      decodeHTML(test_strings[i].s, 1);
      ASSERT(strcmp(test_strings[i].s, test_strings[i].result) == 0, test_strings[i].result);
   }

   TEST_FOOTER();
}


int main(){
   cfg = read_config("test.conf");
   cfg.server_id = 0;

   test_decode_base64_to_buffer();
   test_decodeBase64();
   test_decodeURL();
   test_decodeQP();
   test_decodeHTML();


   return 0;
}
