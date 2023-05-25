/*
 * check_attachments.c, SJ
 */

#include "test.h"


static void test_attachments(struct config *cfg){
   unsigned int i;
   int j;
   char n_attachments[10];
   struct stat st;
   struct session_data sdata;
   struct parser_state state;
   struct data data;

   struct attachments {
      char s[SMALLBUFSIZE];
      char *attachments[3];
      int n_attachments;
   };

   struct attachments tests[] = {
      {"1.eml", {"jatekbolt_logo.png", "ปัจจัยสำคัญ 5 ประการ กับการเคลื่อนไหวในตลาดอัตราแลกเปลี่ยน.เพิ่มเติม.docx", NULL}, 2},
      {"30-subject.eml", {NULL, NULL, NULL}, 0},
   };

   TEST_HEADER();

   snprintf(data.licence.hostname, TINYBUFSIZE-1, "example.com");

   for(i=0; i<sizeof(tests)/sizeof(struct attachments); i++){

      if(stat(tests[i].s, &st) != 0){
         fprintf(stderr, "%s is not found, skipping\n", tests[i].s);
         continue;
      }

      init_session_data(&sdata, cfg);

      snprintf(sdata.ttmpfile, SMALLBUFSIZE-1, "%s", tests[i].s);
      snprintf(sdata.filename, SMALLBUFSIZE-1, "%s", tests[i].s);
      snprintf(sdata.tmpframe, SMALLBUFSIZE-1, "%s.m", tests[i].s);

      state = parse_message(&sdata, 1, &data, cfg);
      post_parse(&sdata, &state, cfg);

      for(j=1; j<=state.n_attachments; j++){
         unlink(state.attachments[j].internalname);
      }

      unlink(sdata.tmpframe);


      snprintf(n_attachments, sizeof(n_attachments)-1, "%d", tests[i].n_attachments);

      ASSERT(state.n_attachments == tests[i].n_attachments, n_attachments);

      for(j=1; j<=state.n_attachments; j++){
         ASSERT(!strcmp(tests[i].attachments[j-1], state.attachments[j].filename), tests[i].attachments[j-1]);
      }
   }

   TEST_FOOTER();
}


int main(){
   struct config cfg;

   if(!can_i_write_directory(NULL)) __fatal("cannot write current directory!");

   cfg = read_config("test.conf");

   setlocale(LC_MESSAGES, cfg.locale);
   setlocale(LC_CTYPE, cfg.locale);


   test_attachments(&cfg);

   return 0;
}
