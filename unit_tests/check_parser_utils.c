/*
 * check_parser_utils.c, SJ
 */

#include <stdio.h>
#include <locale.h>
#include <stdbool.h>
#include <assert.h>
#include "../src/piler.h"


struct date_test {
   char date_str[SMALLBUFSIZE];
   time_t timestamp;
};

struct name_from_header_test {
   char line[SMALLBUFSIZE];
   char *token;
   char *expected_result;
};

struct str_pair {
   char line[SMALLBUFSIZE];
   char *expected_result;
};


static void test_parse_date_header(){
   int i;
   struct __config cfg;
   struct date_test date_test[] = {
      {"Date: Mon, 02 Nov 2015 09:39:31 -0000", 1446457171},
      {"Date: Mon, 2 Nov 2015 10:39:45 +0100", 1446457185},
      {"Date: Sun,  1 Nov 2015 17:23:07 +0100 (CET)", 1446394987},
      {"Date: 3 Nov 2015 15:19:30 +0100", 1446560370},
      {"Date: Mon, 3 Feb 2014 13:21:07 +0100", 1391430067},
      {"Date: Sat, 4 Aug 2007 13:36:52 GMT-0700", 1186256212},
      {"Date: Sat, 4 Aug 07 13:36:52 GMT-0700", 1186256212},
      {"Date: 16 Dec 07 20:45:52", 1197837952},
      {"Date: 03 Jun 06 05:59:00 +0100", 1149307140},
      {"Date: 30.06.2005 17:47:42", 1120150062},
      {"Date: 03-Feb-2014 08:09:10", 1391414950},
      {"Date: 13 Mar 2013 14:56:02 UTC", 1363186562}
   };

   cfg = read_config("test.conf");

   setlocale(LC_MESSAGES, cfg.locale);
   setlocale(LC_CTYPE, cfg.locale);

   for(i=0; i<sizeof(date_test)/sizeof(struct date_test); i++){
      assert(parse_date_header(date_test[i].date_str, &cfg) == date_test[i].timestamp && "test_parse_date_header()");
   }

   printf("test_parse_date_header() OK\n");
}


static void test_extractNameFromHeaderLine(){
   int i;
   char resultbuf[SMALLBUFSIZE];
   struct name_from_header_test name_from_header_test[] = {
      {"Content-Type: text/plain; charset=UTF-8", "charset", "UTF-8"},
      {"Content-Type: text/plain; charset=\"utf-8\"", "charset", "utf-8"},
      {"Content-Type: text/plain; charset=us-ascii", "charset", "us-ascii"},
      {"Content-Type: text/plain; charset=\"iso-8859-2\"", "charset", "iso-8859-2"},
      {"Content-Type: text/calendar; method=REQUEST; charset=\"utf-8\"", "charset", "utf-8"},
      {"Content-Type: text/plain; charset=\"utf-8\"; format=flowed", "charset", "utf-8"},
      {"Content-Type: text/calendar; charset=\"utf-8\"; method=CANCEL", "charset", "utf-8"},
      {"Content-Type: text/plain; format=flowed; charset=\"US-ASCII\"", "charset", "US-ASCII"},
      {"Content-Type: text/plain; charset=\"UTF-8\"; format=flowed", "charset", "UTF-8"},
      {"Content-Type: multipart/alternative; charset=\"UTF-8\"; boundary=\"b1_8c387891290303ff3c6ca75c8e238b54\"", "charset", "UTF-8"},
      {"Content-Type: TEXT/PLAIN; format=flowed; charset=US-ASCII", "charset", "US-ASCII"},

      {"Content-Type: message/delivery-status; name=\"Delivery report\"", "name", "Delivery report"},
      {"Content-Disposition: attachment; filename=\"Delivery report\"; size=1193;", "name", "Delivery report"},
      {"Content-Disposition: attachment; filename=\"AAAAA_ABC_150924.xls\"", "name", "AAAAA_ABC_150924.xls"},
      {"Content-Type: image/jpeg; name=\"image001.jpg\"", "name", "image001.jpg"},
      {"Content-Disposition: inline; filename=\"image001.jpg\"; size=5403;", "name", "image001.jpg"},
      {"Content-Type: application/octet-stream; name=xxxxxDrRestoreGUI.log", "name", "xxxxxDrRestoreGUI.log"},
      {"Content-Disposition: inline; filename=\"Legjobb_Munkahely_Felmeres_2012.jpg\"", "name", "Legjobb_Munkahely_Felmeres_2012.jpg"},
      {"Content-Type: image/png; name=\"=?iso-8859-2?Q?N=E9vtelen.png?=\"", "name", "Névtelen.png"},
      {"Content-Type: application/msword; name=", "name", ""},
      {"Content-Disposition: attachment; filename=", "name", ""},
      {"Content-Disposition: attachment; filename=\"NDA - Suto - 20151109.pdf\";", "name", "NDA - Suto - 20151109.pdf"},
      {"Content-Type: image/png; name=\"Screenshot from 2015-11-10 10:07:13.png\"", "name", "Screenshot from 2015-11-10 10:07:13.png"},
      {"Content-Disposition: attachment; filename=\"zzzzz Email Examples.zip\";", "name", "zzzzz Email Examples.zip"},

      {"foo: bar; title=Economy", "title", "Economy"},
      {"foo: bar; title=\"US-$ rates\"", "title", "US-$ rates"},
      {"foo: bar; title*=iso-8859-1'en'%A3%20rates", "title", "£ rates"},
      {"foo: bar; title*=UTF-8''%c2%a3%20and%20%e2%82%ac%20rates", "title", "£ and € rates"}
   };


   for(i=0; i<sizeof(name_from_header_test)/sizeof(struct name_from_header_test); i++){
      extractNameFromHeaderLine(name_from_header_test[i].line, name_from_header_test[i].token, resultbuf);
      assert(strcmp(resultbuf, name_from_header_test[i].expected_result) == 0 && "test_extractNameFromHeaderLine");
   }

   printf("test_extractNameFromHeaderLine() OK\n");
}


static void test_fixupEncodedHeaderLine(){
   int i;
   char buf[SMALLBUFSIZE];
   struct str_pair pair[] = {

      {"=?utf-8?Q?Tanjoubi,_azaz_sz=C3=BClet=C3=A9snap!_10_=C3=A9ves_az_I_Love_Su?=  =?utf-8?Q?shi!?=", "Tanjoubi, azaz születésnap! 10 éves az I Love Su  shi!"},
      {"=?UTF-8?Q?IAM:_N2YPF_-_#1_Request_new_privilege?=", "IAM: N2YPF - #1 Request new privilege"},
      {"=?UTF-8?B?SG9neWFuIMOtcmp1bmsgcGFuYXN6bGV2ZWxldD8=?=", "Hogyan írjunk panaszlevelet?"},
      {"Re: [Bitbucket] Issue #627: ldap user can't login (jsuto/piler)", "Re: [Bitbucket] Issue #627: ldap user can't login (jsuto/piler)"},
      {"=?iso-8859-2?Q?RE:_test.aaa.fu_z=F3na?=", "RE: test.aaa.fu zóna"},
      {"=?iso-8859-2?Q?V=E1ltoz=E1s_az_IT_szervezetben_/_Personal_changes_in_the_?=", "Változás az IT szervezetben / Personal changes in the "},
      {"Re: AAAmil  /29  UZ736363", "Re: AAAmil  /29  UZ736363"},
      {"=?UTF-8?Q?[JIRA]_Created:_(HUDSS-196)_T=C5=B1zfal_?=", "[JIRA] Created: (HUDSS-196) Tűzfal "},
      {"=?iso-8859-2?Q?RE:_Baptista_Szeretetszolg=E1lat?=", "RE: Baptista Szeretetszolgálat"},
      {"=?iso-8859-2?B?SXR0IGF6IE1OQiBuYWd5IGRvYuFzYTogaXNt6XQgYmVsZW55+mxuYWsgYSBoaXRlbGV66XNiZSAoMjAxNS4xMS4wMy4gLSBzakBhY3RzLmh1KQ==?=", "Itt az MNB nagy dobása: ismét belenyúlnak a hitelezésbe (2015.11.03. - sj@acts.hu)"},
      {"=?UTF-8?B?TGludXggQURNSU4gaG96esOhZsOpcsOpc2Vr?=", "Linux ADMIN hozzáférések"},
      {"Burn 14 Calories a Minute with This Workout!", "Burn 14 Calories a Minute with This Workout!"},
      {"=?utf-8?Q?Help=20Net=20Security=20Newsletter=20=2D=20=20November=203rd=202015?=", "Help Net Security Newsletter -  November 3rd 2015"},
      {"=?ISO-8859-2?Q?Re=3A_kimen=F5_levelez=E9si_probl=E9ma_-_cscs=40aaaaa=2Efu?=", "Re: kimenő levelezési probléma - cscs@aaaaa.fu"},
      {"Re: cccc@aaa.fu - e-mail =?UTF-8?B?a8OpcmTDqXM=?=", "Re: cccc@aaa.fu - e-mail kérdés"},
      {"=?WINDOWS-1250?Q?<AZ-17226/1-2015>=20www.xxxxx.com=20new=20virtual=20?=", "<AZ-17226/1-2015> www.xxxxx.com new virtual "},
      {"Re: FW: =?ISO-8859-2?Q?Sopron-Gy=F5r_optikai_sz=E1l_probl=E9?=", "Re: FW: Sopron-Győr optikai szál problé"},
      {"=?UTF-8?Q?Megh=C3=ADv=C3=B3=20a=20Pulzus=20felm=C3=A9r=C3=A9sre=20/=20Inv?=  =?UTF-8?Q?itation=20to=20the=20Pulse=20Survey?=", "Meghívó a Pulzus felmérésre / Inv  itation to the Pulse Survey"},
      {"=?iso-8859-2?Q?vhost_l=E9trehoz=E1sa?=", "vhost létrehozása"},
      {"Re: MAIL =?UTF-8?B?U1pPTEfDgUxUQVTDgVMgSElCQSAgIEdUUzogOTE1NDUyMQ==?=", "Re: MAIL SZOLGÁLTATÁS HIBA   GTS: 9154521"},
      {"[spam???]  Better Sex. Better Body. Better Life.", "[spam???]  Better Sex. Better Body. Better Life."},
      {"1gy2tt. V3l4d. M5sk6nt", "1gy2tt. V3l4d. M5sk6nt"},
      {"=?iso-8859-2?B?03Jp4XNpIG1lZ2xlcGV06XMsIG5pbmNzIHT2YmIgbWVudHPpZyBBbWVyaWthIHN64W3hcmEgKDIwMTUuMTEuMDYuIC0gc2pAYWN0cy5odSk=?=", "Óriási meglepetés, nincs több mentség Amerika számára (2015.11.06. - sj@acts.hu)"},
      {"=?utf-8?B?Rlc6IEVtYWlsIGZvZ2Fkw6FzaSBoaWJh?=", "FW: Email fogadási hiba"},
      {"=?ISO-8859-15?Q?RE=3A_FW=3A_K=E9rd=E9s?=", "RE: FW: Kérdés"},
      {"=?iso-8859-2?Q?RE:_spam_tilt=E1s?=", "RE: spam tiltás"},
      {"Subject: Administrator has responded to your request for 'Cloud Operations", "Subject: Administrator has responded to your request for 'Cloud Operations"},
      {"Subject: =?GB2312?B?VFYgYmFjayBLSVQgc3RyaXAgcmYgcmVtb3RlIENvbnRyb2wgIFVTRDUuNS9TRVQ=?=", "Subject: TV back KIT strip rf remote Control  USD5.5/SET"},
      {"Subject: =?UTF-8?Q?Ha_rossz_a_k=C3=B6z=C3=A9rzete?=", "Subject: Ha rossz a közérzete"},
      {"Subject: =?UTF-8?B?SsOhdHNzeiBhIHZpbMOhZyBsZWdixZFrZXrFsWJiIGxvdHTDs2rDoW4gSU5HWUVO?=", "Subject: Játssz a világ legbőkezűbb lottóján INGYEN"},
      {"Subject: =?UTF-8?B?w5Zua29ybcOhbnl6YXRpIGFkYXRiw6F6aXMgMiwzIEZ0IC8gZGIgw6Fyb24=?=", "Subject: Önkormányzati adatbázis 2,3 Ft / db áron"},
      {"Subject: =?UTF-8?Q?Experience=20a=20Crazy=20Reward=20Delivered=20to=20you?=", "Subject: Experience a Crazy Reward Delivered to you"},
      {"Subject: =?windows-1251?B?ze7i7uPu5O3o5SDv7uTg8OroIOTr/yDC4Pjo?=", "Subject: Новогодние подарки для Ваши"},
      {"Subject: =?utf-8?Q?Divatos,_=C3=BCde_sz=C3=ADneinek_k=C3=B6sz=C3=B6nhet=C5=91en_el?=", "Subject: Divatos, üde színeinek köszönhetően el"},
   };


   for(i=0; i<sizeof(pair)/sizeof(struct str_pair); i++){
      snprintf(buf, sizeof(buf)-1, "%s", pair[i].line);

      fixupEncodedHeaderLine(buf, sizeof(buf)-1);

      assert(strcmp(buf, pair[i].expected_result) == 0 && "test_fixupEncodedHeaderLine");

      //printf("      {\"%s\", \"%s\"},\n", pair[i].line, buf);
   }

   printf("test_fixupEncodedHeaderLine() OK\n");
}


static void test_translateLine(){
   int i;
   char buf[SMALLBUFSIZE];
   struct parser_state state;
   struct str_pair pair[] = {
      {"From: \"'user@domain'\"", "From    user@domain  "},
      {"From: \"''user@domain'\"", "From    'user@domain  "},
      {"From: \"''user'@domain'\"", "From    'user'@domain  "},
      {"From: \"'user'@domain'\"", "From    user'@domain  "},
      {"From: Mike D'Amaaaaa <mike@aaa.fu>", "From  Mike D'Amaaaaa  mike@aaa.fu "},
      {"From: VMware Technical Support <webform@vmware.com>", "From  VMware Technical Support  webform@vmware.com "}

      /*
       * TODO: we need many more tests here
       */
   };


   for(i=0; i<sizeof(pair)/sizeof(struct str_pair); i++){

      init_state(&state);
      state.message_state = MSG_FROM;
      state.is_header = 1;

      snprintf(buf, sizeof(buf)-1, "%s", pair[i].line);

      translateLine((unsigned char*)buf, &state);

      //printf("      {\"%s\", \"%s\"},\n", pair[i].line, buf);

      assert(strcmp(buf, pair[i].expected_result) == 0 && "test_translateLine");
   }

   printf("test_translateLine() OK\n");
}


static void test_fixURL(){
   int i;
   char buf[SMALLBUFSIZE];
   struct str_pair pair[] = {
      {"http://www.aaa.fu", "__URL__wwwXaaaXfu "},
      {"http://www.aaa.fu/", "__URL__wwwXaaaXfu "},
      {"http://www.aaa.fu/ahaha", "__URL__wwwXaaaXfu "},
      {"http://www.aaa.fu/bbb/ccc", "__URL__wwwXaaaXfu "},
      {"http://www.aaa.fu/ahahah/aiaiai?url=http://iii.oo/", "__URL__wwwXaaaXfu "},
      {"https://www.aaa.fu/", "__URL__wwwXaaaXfu "}
   };


   for(i=0; i<sizeof(pair)/sizeof(struct str_pair); i++){

      snprintf(buf, sizeof(buf)-1, "%s", pair[i].line);

      fixURL(buf, sizeof(buf)-1);

      //printf("      {\"%s\", \"%s\"},\n", pair[i].line, buf);

      assert(strcmp(buf, pair[i].expected_result) == 0 && "test_fixURL");

   }

   printf("test_fixURL() OK\n");

}


static void test_degenerateToken(){
   int i;
   char buf[SMALLBUFSIZE];
   struct str_pair pair[] = {
      {"Hello", "Hello"},
      {"Hello!", "Hello"},
      {"Hello!!", "Hello"},
      {"Hello!!!", "Hello"},
      {"Hello?", "Hello"},
      {"Hello??", "Hello"},
      {"Hello???", "Hello"},
      {"Hello.", "Hello"},
      {"Hello..", "Hello"},
      {"Hello...", "Hello"}
   };


   for(i=0; i<sizeof(pair)/sizeof(struct str_pair); i++){

      snprintf(buf, sizeof(buf)-1, "%s", pair[i].line);

      degenerateToken((unsigned char*)buf);

      //printf("      {\"%s\", \"%s\"},\n", pair[i].line, buf);

      assert(strcmp(buf, pair[i].expected_result) == 0 && "test_degenerateToken");

   }

   printf("test_degenerateToken() OK\n");

}


/*
   other functions to test in the future:

   - int extract_boundary(char *p, struct parser_state *state)
   - void fix_email_address_for_sphinx(char *s)
   - void reassembleToken(char *p)

*/




int main(){
   test_parse_date_header();
   test_extractNameFromHeaderLine();
   test_fixupEncodedHeaderLine();
   test_translateLine();
   test_fixURL();
   test_degenerateToken();

   return 0;
}

