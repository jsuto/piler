#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <piler.h>


int test_url(char *url, char *expected_result){
   char buf[SMALLBUFSIZE];

   snprintf(buf, sizeof(buf)-1, "%s", url);
   fixURL(buf);

   if(strcmp(buf, expected_result)){
      printf("FAILED: '%s' => fixed: '%s', expected: '%s'\n", url, buf, expected_result);
      return 1;
   }

   return 0;
}


int test_translate(char *what, char *expected, struct _state *state){
   char buf[SMALLBUFSIZE];

   snprintf(buf, sizeof(buf)-1, "%s", what);

   translateLine((unsigned char*)buf, state);

   if(strcmp(buf, expected)){
      printf("FAILED: '%s' => fixed: '%s', expected: '%s'\n", what, buf, expected);
      return 1;
   }

   return 0;
}


int test_urls(){
   int count=0;

   count += test_url("http://sourceforge.net/projects/blogsmanager/", "__URL__sourceforgeXnet ");
   count += test_url("http://localhost/blogs/_authors_list.php?a=search&value=1&SearchFor=muuratsalo&SearchOption=Contains&SearchField=[SQL", "__URL__localhost ");
   count += test_url("http://www.debian.org/security/faq", "__URL__wwwXdebianXorg ");
   count += test_url("http://www.debian.org/", "__URL__wwwXdebianXorg ");
   count += test_url("https://www.debian.org", "__URL__wwwXdebianXorg ");
   count += test_url("HTTP://www.debian.o", "__URL__wwwXdebianXo ");
   count += test_url("http://www.debian.", "__URL__wwwXdebian ");
   count += test_url("www.debian.org", "__URL__wwwXdebianXorg ");
   count += test_url("http://web.nvd.nist.gov/view/vuln/detail?vulnId=3DCVE-2011-3892", "__URL__webXnvdXnistXgov ");
   count += test_url("http://bugs.sitracker.org/view.php?id=1737", "__URL__bugsXsitrackerXorg ");
   count += test_url("http://googlechromereleases.blogspot.com/2011/11/stable-channel-update.ht", "__URL__googlechromereleasesXblogspotXcom ");
   count += test_url("http://security.gentoo.org/glsa/glsa-201111-05.xml", "__URL__securityXgentooXorg ");
   count += test_url("https://bugs.gentoo.org.", "__URL__bugsXgentooXorg ");
   count += test_url("https://bugs.gentoo.org./ajajajajaaj", "__URL__bugsXgentooXorg ");
   count += test_url("http://creativecommons.org/licenses/by-sa/2.5", "__URL__creativecommonsXorg ");
   count += test_url("http://www.site.com/[path]/wp-content/plugins/advanced-text-widget/advancedtext.php?page=[xss]", "__URL__wwwXsiteXcom ");
   count += test_url("http://canadamedshealth.ru", "__URL__canadamedshealthXru ");
   count += test_url("http://[HOSTNAME]:4848/configuration/httpListenerEdit.jsf?name=<script>alert(document.cookie);</script>&configName=server-config", "__URL__[HOSTNAME]:4848 ");
   count += test_url("http://go.theregister.com/news/http://www.theregister.co.uk/2007/07/", "__URL__goXtheregisterXcom ");
   count += test_url("http://dl.shadowserver.org/IpE6yFKxIPARB8447vAQoyeVtbs?Rq123jTRTTrzApVs0vTzyQ", "__URL__dlXshadowserverXorg ");
   count += test_url("http://dl.shadowserver.org/IpE6yFKx%EAPARB8447vAQoyeVtbs?Rq123jTRTTrzApVs0vTzyQ", "__URL__dlXshadowserverXorg ");
   count += test_url("", "");

   return count;
}


int test_translates(){
   int count=0;
   struct _state state;

   init_state(&state);

   count += test_translate("To: \"Suto, Janos\" <Janos.Suto@foo.bar>", "To   Suto  Janos   Janos.Suto@foo.bar", &state);
   count += test_translate("Ez most akkor beteg, vagy sem?", "Ez most akkor beteg  vagy sem ", &state);
   count += test_translate("MAIL FROM:    <zoltan.szabo@zte.com.cn>", "MAIL FROM      zoltan.szabo@zte.com.cn ", &state);
   count += test_translate("Enjoy your game, and then enjoy your jackpot!", "Enjoy your game  and then enjoy your jackpot!", &state);
   count += test_translate("Az �n által megküldött,", "Az �n által megküldött ", &state);

   return count;
}


int test_html(char *what, char *expected, struct _state *state){
   char buf[SMALLBUFSIZE];

   snprintf(buf, sizeof(buf)-1, "%s", what);

   markHTML(buf, state);

   if(strcmp(buf, expected)){
      printf("FAILED: '%s' => fixed: '%s', expected: '%s'\n", what, buf, expected);
      return 1;
   }

   //printf("OK: '%s' => fixed: '%s', expected: '%s'\n", what, buf, expected);

   return 0;
}


int test_htmls(){
   int count=0;
   struct _state state;

   init_state(&state);

   count += test_html("</div>", " ", &state);
   count += test_html("jackpot!</o:p></p>", "jackpot!  ", &state);
   count += test_html("<p class=3DMsoNormal><o:p>Enjoy your game, and then enjoy your =", "  Enjoy your game, and then enjoy your =", &state);
   count += test_html("    <p class style>ooooo", "     ooooo", &state);
   count += test_html("             ", "             ", &state);
   count += test_html("<html><center><table border=10 cellspacing=0 cellpadding=10 bordercolor=C0C0C0 width=600>", "   ", &state);
   count += test_html("<td bgcolor=FFFFFF align=center><font size=3 face=Dotum color=5F5F5F>", "  ", &state);
   count += test_html("<font size=5 color=0000FF><b>Viagra50/100mg - $1.85 |BUY NOW|</b></font><br>", "  Viagra50/100mg - $1.85 |BUY NOW|   ", &state);
   count += test_html("High Qua1ityMedications + Discount On All Reorders +<br>", "High Qua1ityMedications + Discount On All Reorders + ", &state);
   count += test_html("<a href=http://canadamedshealth.ru target=_blank><img src = http://aaa.fu/1.gif><font size=2 color=D90000><b>Free Shipping Options + Free Pills With Every Order = Best Deal Ever!<br>''''''''''click here''''''''''</b></font></a><br></font></td></tr></table></center></html>", "    Free Shipping Options + Free Pills With Every Order = Best Deal Ever! ''''''''''click here'''''''''", &state);
   count += test_html("http://bbb.fu/2.gif", "http://bbb.fu/2.gif", &state);

   count += test_html("<html>", " ", &state);
   count += test_html("    <style type=3D\"text/css\">", "     ", &state);
   count += test_html("        <span class=3D\"style3\">=E2=ED =E1=F1=FA=E9=E5 =E9=F9 =EE=E1=F6=F2=", "         =E2=ED =E1=F1=FA=E9=E5 =E9=F9 =EE=E1=F6=F2=", &state);
   count += test_html("=E9=ED =E1</span> <a href=3D\"http://www=2Eybay=2Eco=2Eil/\">=E0=E9=F0=E3=", "=E9=ED =E1   =E0=E9=F0=E3=", &state);
   count += test_html("=F7=F1 =F2=F1=F7=E9=ED</a><o:p></o:p></span></b></p>", "=F7=F1 =F2=F1=F7=E9=ED      ", &state);
   count += test_html("        <span dir=3D\"rtl\" lang=3D\"HE\"=20", "         ", &state);

   state.htmltag = 1;
   count += test_html("span></p>llll", " llll", &state);

   return count;
}

int test_dates(){
   int count=0;
   unsigned long ts;
   char datestr[SMALLBUFSIZE];
   struct __config cfg;

   cfg.tweak_sent_time_offset = 0;

   snprintf(datestr, sizeof(datestr)-2, "Date: Mon, 3 Feb 2014 13:16:09 +0100");
   ts = parse_date_header(datestr, &cfg); printf("%s => %ld\n", datestr, ts);

   snprintf(datestr, sizeof(datestr)-2, "Date: Sat, 4 Aug 07 13:36:52 GMT-0700");
   ts = parse_date_header(datestr, &cfg); printf("%s => %ld\n", datestr, ts);

   snprintf(datestr, sizeof(datestr)-2, "Date: 23 Sep 09 07:03 -0800");
   ts = parse_date_header(datestr, &cfg); printf("%s => %ld\n", datestr, ts);

   snprintf(datestr, sizeof(datestr)-2, "Date: 16 Dec 07 20:45:52");
   ts = parse_date_header(datestr, &cfg); printf("%s => %ld\n", datestr, ts);

   snprintf(datestr, sizeof(datestr)-2, "Date: 30.06.2005 17:47:42");
   ts = parse_date_header(datestr, &cfg); printf("%s => %ld\n", datestr, ts);

   snprintf(datestr, sizeof(datestr)-2, "Date: 03 Jun 06 05:59:00 +0100");
   ts = parse_date_header(datestr, &cfg); printf("%s => %ld\n", datestr, ts);

   snprintf(datestr, sizeof(datestr)-2, "Date: 03-Feb-2014 18:00:00");
   ts = parse_date_header(datestr, &cfg); printf("%s => %ld\n", datestr, ts);

   return count;
}


int main(int argc, char **argv){
   int n;

   n = test_urls();
   printf("testing fixURL(), errors: %d\n", n);


   n = test_translates();
   printf("testing translateLine(), errors: %d\n", n);


   n = test_htmls();
   printf("testing markHTML(), errors: %d\n", n);

   n = test_dates();
   printf("testing parse_date_header(), errors: %d\n", n);

   return 0;
}
