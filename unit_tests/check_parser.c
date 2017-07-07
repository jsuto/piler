/*
 * check_parser.c, SJ
 */

#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../src/piler.h"


struct parser_test {
   char s[SMALLBUFSIZE];
   char message_id[SMALLBUFSIZE];
   char from[SMALLBUFSIZE];
   char from_domain[SMALLBUFSIZE];
   char to[SMALLBUFSIZE];
   char to_domain[SMALLBUFSIZE];
   char reference[SMALLBUFSIZE];
   char subject[SMALLBUFSIZE];
   int n_attachments;
};


static void test_parser(struct __config *cfg){
   unsigned int i;
   int j;
   struct stat st;
   struct session_data sdata;
   struct parser_state state;
   struct __data data;
   struct parser_test tests[] = {

      {"1.eml", "<ajahhdddhjdhddh@jatekokbirodalma.hu>", "játékok birodalma játékbolt hirlevel@jatekokbirodalma.hu hirlevel jatekokbirodalma hu ", "jatekokbirodalma.hu", "architerv m sj@acts.hu sj acts hu ", "acts.hu ", "", "BLACK FRIDAY - Hihetetlen kedvezmények csak 1 napig november 27-én", 2},
      {"2.eml", "<20151101142653.111156815AF6D@acts.hu>", "jml lighting huixinsoft67@foxmail.com huixinsoft67 foxmail com ", "foxmail.com", "sj@acts.hu sj acts hu ", "acts.hu ", "", "New design ultra slim led  panel light", 0},
      {"5-ibm-images.eml", "<OFC1576266.0E8F7B7D-ONC12577CB.00303030-C12577CB.003053CD@hu.ibm.com>", "ibm rendezveny rendezveny@hu.ibm.com rendezveny hu ibm com ", "hu.ibm.com", "cim1@aaaa.bbb.fu cim1 aaaa bbb fu ajajaj@piler.aaa.fu ajajaj piler aaa fu ibm rendezveny rendezveny@hu.ibm.com rendezveny hu ibm com ", "aaaa.bbb.fu piler.aaa.fu hu.ibm.com ", "", "***Emlékeztető*** - Egészségipar - eEgészségügy (Út a jövőbe, párbeszéd a gazdaságélénkítésről)  2010. november 4.", 5},
      {"9-attached-text.eml", "<list-507327664@mail.aaa.fu>", "dr lucky amechi clubzenit@zenithoteles.com clubzenit zenithoteles com ", "zenithoteles.com", "usuarios-no-listados ", "", "", "Please read my attached letter", 1},
      {"13-xlsx.eml", "<alpine.LNX.2.00.1209261517040.17054@aaa.fu>", "aaaaa@aaa.fu aaaaa aaa fu ", "aaa.fu", "sj@acts.hu sj acts hu ", "acts.hu ", "", "ez egy teszt", 1},
      {"15-image-only-spam.eml", "<av5f1fCf5XO0oBab757826337RSFKvu@pnmarketing.com>", "kriegel paff sketches@pnmarketing.com sketches pnmarketing com ", "pnmarketing.com", "holmon knobel aaaaa@acts.hu aaaaa acts hu ", "acts.hu ", "", "Lack of concentration, backed up by a vocabulary of tremendous scope, a", 1},
      {"16-rfc822-attachment-1.eml", "<list-423974736@mail.aaa.fu>", "martonagnes martonagnes@lajt.hu martonagnes lajt hu erős istván eistvan@marosheviz.info ", "lajt.hu", "martonagnes@lajt.hu martonagnes lajt hu ", "lajt.hu ", "", "Féláras akció! 31000Ft/2fő/3nap húsvétkor is a Park Inn****-ben!", 2 },
      {"17-attached-text-bogus-mime.eml", "<list-507327664@mail.aaa.fu>", "dr lucky amechi clubzenit@zenithoteles.com clubzenit zenithoteles com ", "zenithoteles.com", "usuarios-no-listados ", "", "", "Please read my attached letter", 1},
      {"18-spam-html-encoding.eml", "<list-435458392@mail.aaa.fu>", "a1 hitelcentrum kft Üveges szilvia a1hitelcentrum@t-online.hu a1hitelcentrum t online hu ", "t-online.hu", "postmaster@aaa.fu postmaster aaa fu ", "aaa.fu ", "", "TÁJÉKOZTATÁS Vargay Péter", 0},
      {"19-pdf-attachment-bad-mime.eml", "<20100213$2b62e942$9cc2b$sxm@61-186.reverse.ukhost4u.com>", "jennifer - billing department billing@limitedsoftwareworld.com billing limitedsoftwareworld com ", "limitedsoftwareworld.com", "100000 100000@aaa.fu 100000 aaa fu ", "aaa.fu ", "", "Billing Summary for 100000, Processed on 2010-02-13 17:01:03", 1},
      {"20-pdf-attachment-bad-mime.eml", "<20100213$2b62e942$9cc2b$sxm@61-187.reverse.ukhost4u.com>", "jennifer - billing department billing@limitedsoftwareworld.com billing limitedsoftwareworld com ", "limitedsoftwareworld.com", "100000 100000@aaa.fu 100000 aaa fu ", "aaa.fu ", "", "Billing Summary for 100000, Processed on 2010-02-13 17:01:03", 1},
      {"21-register-tricky-urls.eml", "<E1IBifn-0001un-MD@admin4.theregister.co.uk>", "the register update-49363-08f0f768@list.theregister.co.uk update 49363 08f0f768 list theregister co uk ", "list.theregister.co.uk", "hello@mail.aaa.fu hello mail aaa fu ", "mail.aaa.fu ", "", "[sp@m]  Reg Headlines Friday July 20", 0},
      {"30-subject.eml", "<3660278814815884@pongr-fabd8067e>", "aaapsi.hu info@aaapsi.hu info aaapsi hu ", "aaapsi.hu", "hello@acts.hu hello acts hu ", "acts.hu ", "", "RE: hxx-ajajajaja.com  Aaagágyi és kia ttt webstat hiba", 0},
      {"31-subject.eml", "<3660278814815884@pongr-fabd8067e>", "aaapsi.hu info@aaapsi.hu info aaapsi hu ", "aaapsi.hu", "hello@acts.hu hello acts hu ", "acts.hu ", "", "Re: stanhu \"domain not found\"-dal eldobja a @fohu-ra küldött leveleket...", 0},
      {"32-subject.eml", "<3660278814815884@pongr-fabd8067e>", "aaapsi.hu info@aaapsi.hu info aaapsi hu ", "aaapsi.hu", "hello@acts.hu hello acts hu ", "acts.hu ", "", "<GD-XXXX/1-2015> www.ujsag.hu new virtual host reg. --> Aaaaaaaaa", 0},
      {"33-subject.eml", "<3660278814815884@pongr-fabd8067e>", "aaapsi.hu info@aaapsi.hu info aaapsi hu ", "aaapsi.hu", "hello@acts.hu hello acts hu ", "acts.hu ", "", "[JIRA] Commented: (AAAA-151) A aaa-nek kerek egy XXX-et, ZH74617282, ACC27363484944", 0},
   };


   if(open_database(&sdata, cfg) == ERR){
      printf("cannot open database\n");
      return;
   }


   for(i=0; i<sizeof(tests)/sizeof(struct parser_test); i++){

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

      //printf("%s, %s/%s %d / %d\n", tests[i].s, tests[i].message_id, state.message_id, tests[i].n_attachments, state.n_attachments);

      for(j=1; j<=state.n_attachments; j++){
         unlink(state.attachments[j].internalname);
      }

      unlink(sdata.tmpframe);

      assert(strcmp(state.message_id, tests[i].message_id) == 0 && "test_parser()1");
      assert(strcmp(state.b_from, tests[i].from) == 0 && "test_parser()2a");
      assert(strcmp(state.b_from_domain, tests[i].from_domain) == 0 && "test_parser()2b");
      assert(strcmp(state.b_to, tests[i].to) == 0 && "test_parser()3a");
      assert(strcmp(state.b_to_domain, tests[i].to_domain) == 0 && "test_parser()3b");
      assert(strcmp(state.b_subject, tests[i].subject) == 0 && "test_parser()4");
      assert(state.n_attachments == tests[i].n_attachments && "test_parser()5");
   }

   close_database(&sdata);

   printf("test_parser() OK\n");
}


int main(){

   struct __config cfg;

   if(!can_i_write_directory(NULL)) __fatal("cannot write current directory!");

   (void) openlog("mydomains_test", LOG_PID, LOG_MAIL);

   cfg = read_config("test.conf");

   setlocale(LC_MESSAGES, cfg.locale);
   setlocale(LC_CTYPE, cfg.locale);


   test_parser(&cfg);

   return 0;
}


