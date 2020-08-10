/*
 * check_misc.c, SJ
 */

#include "test.h"

struct config cfg;
struct smtp_session session;
struct data data;


static void test_strtolower(){
   unsigned int i;
   struct test_data_s_s test_data_s_s[] = {
      { "aaaa", "aaaa"},
      { "aBhu+18", "abhu+18"},
      { "u Uj i", "u uj i"},
      { "eee?", "eee?"},
      { "EEE?E", "eee?e"},
   };

   TEST_HEADER();

   for(i=0; i<sizeof(test_data_s_s)/sizeof(struct test_data_s_s); i++){
      strtolower(test_data_s_s[i].s);
      ASSERT(strcmp(test_data_s_s[i].s, test_data_s_s[i].result) == 0, test_data_s_s[i].result);
   }

   TEST_FOOTER();
}


static void test_extract_verp_address(){
   unsigned int i;
   struct test_data_s_s test_data_s_s[] = {
      { "archive+user=domain.com@myarchive.local", "user@domain.com"},
   };

   TEST_HEADER();

   for(i=0; i<sizeof(test_data_s_s)/sizeof(struct test_data_s_s); i++){
      extract_verp_address(test_data_s_s[i].s);
      ASSERT(strcmp(test_data_s_s[i].s, test_data_s_s[i].result) == 0, test_data_s_s[i].result);
   }

   TEST_FOOTER();
}


static void test_extract_email(){
   unsigned int i;
   char s[SMALLBUFSIZE];
   struct test_data_s_s test_data_s_s[] = {
      { "MAIL FROM:<aaa@aaa.fu>\r\n", "aaa@aaa.fu" },
      { "MAIL FROM: <aaa@aaa.fu>\r\n", "aaa@aaa.fu" },
      { "RCPT TO: <aaa@aaa.fu>\r\n", "aaa@aaa.fu" },
      { "<aaa@aaa.fu>", "aaa@aaa.fu" },
   };

   TEST_HEADER();

   for(i=0; i<sizeof(test_data_s_s)/sizeof(struct test_data_s_s); i++){
      extractEmail(test_data_s_s[i].s, s);
      ASSERT(strcmp(s, test_data_s_s[i].result) == 0, test_data_s_s[i].result);
   }

   TEST_FOOTER();
}


static void test_trim_buffer(){
   unsigned int i;
   struct test_data_s_s test_data_s_s[] = {
      { "auaua\r\n", "auaua" },
      { "hello\n", "hello" },
      { "qqq\r", "qqq" },
      { "akaka\nkkk", "akaka" },
      { "qqq\r\naaa", "qqq" },
   };

   TEST_HEADER();

   for(i=0; i<sizeof(test_data_s_s)/sizeof(struct test_data_s_s); i++){
      trimBuffer(test_data_s_s[i].s);
      ASSERT(strcmp(test_data_s_s[i].s, test_data_s_s[i].result) == 0, test_data_s_s[i].result);
   }

   TEST_FOOTER();
}


static void test_make_random_string(){
   unsigned int i;
   char buf[SMALLBUFSIZE];

   TEST_HEADER();

   for(i=0; i<10; i++){
      make_random_string((unsigned char*)&buf[0], QUEUE_ID_LEN);
      printf("%s ", buf);
   }

   TEST_FOOTER();
}


static void test_create_id(){
   unsigned int i;
   char buf[SMALLBUFSIZE];

   TEST_HEADER();

   for(i=0; i<10; i++){
      create_id(buf, 0xf);
      ASSERT(strncmp(buf, "40000000", strlen("40000000")) == 0, buf);
      ASSERT(buf[24] == '0' && buf[25] == 'f', buf);
   }

   TEST_FOOTER();
}


static void test_split(){
   unsigned int i;
   int result;
   char buf[SMALLBUFSIZE];

   struct test_data_s_s_i test_data_s_s_i[] = {
      { "hello\nworld\n", "world\n", 1 },
      { "hello\nworld", "world", 1 },
      { "hello\n", "", 1 },
      { "helloworld", "", 0 },
   };

   TEST_HEADER();

   for(i=0; i<sizeof(test_data_s_s_i)/sizeof(struct test_data_s_s_i); i++){
      char *p = split(test_data_s_s_i[i].s1, '\n', buf, sizeof(buf)-1, &result);

      if(p){ ASSERT(strcmp(buf, "hello") == 0 && strcmp(p, test_data_s_s_i[i].s2) == 0 && result == test_data_s_s_i[i].result, test_data_s_s_i[i].s1); }
      else { ASSERT(p == NULL && result == test_data_s_s_i[i].result, test_data_s_s_i[i].s1); }
   }

   TEST_FOOTER();
}


static void test_split_str(){
   unsigned int i;
   char buf[SMALLBUFSIZE];

   struct test_data_s_s test_data_s_s[] = {
      { "aaaXXbbbXX", "bbbXX" },
      { "aaaXXbbb", "bbb" },
      { "aaaXX", "" },
      { "aaa", "" },
   };

   TEST_HEADER();

   for(i=0; i<sizeof(test_data_s_s)/sizeof(struct test_data_s_s); i++){
      char *p = split_str(test_data_s_s[i].s, "XX", buf, sizeof(buf)-1);

      if(p){ ASSERT(strcmp(buf, "aaa") == 0 && strcmp(test_data_s_s[i].result, p) == 0, test_data_s_s[i].s); }
      else { ASSERT(strcmp(buf, "aaa") == 0 && p == NULL, test_data_s_s[i].s); }
   }

   TEST_FOOTER();
}


int main(){
   if(!can_i_write_directory(NULL)) __fatal("cannot write current directory!");

   (void) openlog("mydomains_test", LOG_PID, LOG_MAIL);

   cfg = read_config("test.conf");
   cfg.server_id = 0;
   session.cfg = &cfg;

   setlocale(LC_MESSAGES, cfg.locale);
   setlocale(LC_CTYPE, cfg.locale);

   test_strtolower();
   test_extract_verp_address();
   test_extract_email();
   test_trim_buffer();
   test_make_random_string();
   test_create_id();
   test_split();
   test_split_str();


   return 0;
}
