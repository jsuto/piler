#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <piler.h>


void init_smtp_acl(struct smtp_acl *smtp_acl[]){
   smtp_acl[0] = NULL;
}


void clear_smtp_acl(struct smtp_acl *smtp_acl[]){
   struct smtp_acl *q;

   q = smtp_acl[0];

   while(q){
      struct smtp_acl *p = q;
      q = q->r;

      free(p);
   }

   smtp_acl[0] = NULL;
}


int add_smtp_acl(struct smtp_acl *smtp_acl[], char *network_str, struct smtp_acl *acl){
   struct smtp_acl *q, *p=NULL, *node;

   if((node = malloc(sizeof(struct smtp_acl))) == NULL) return 0;

   memset(node, 0, sizeof(struct smtp_acl));

   node->low = acl->low;
   node->high = acl->high;
   node->prefix = acl->prefix;
   node->rejected = acl->rejected;
   snprintf(node->network_str, sizeof(node->network_str)-1, "%s", network_str);

   node->r = NULL;

   q = smtp_acl[0];

   while(q){
      p = q;
      q = q->r;
   }

   if(!p){
      smtp_acl[0] = node;
   } else {
      p->r = node;
   }

   return 1;
}


int is_valid_line(char *line){
   // Currently we support ipv4 stuff only, ie. valid characters are: 0-9./
   // and a line should look like "1.2.3.4/24 permit" or similar (without quotes)

   if(!strchr(line, '.') || !strchr(line, '/') || (!strchr(line, ' ') && !strchr(line, '\t')) ){
      return 0;
   }

   if(!strstr(line, "permit") && !strstr(line, "reject")){
      return 0;
   }

   // ascii values:
   //   46: .
   //   47: /
   //   48-57: 0-9
   //   97-122: a-z
   //
   for(; *line; line++){
      if(isalnum(*line) == 0 && isblank(*line) == 0 && *line != 46 && *line != 47) return 0;
   }

   return 1;
}


int a_to_hl(char *ipstr, in_addr_t *addr){
  struct in_addr in;

  if(inet_aton(ipstr, &in) == 1){
     *addr = ntohl(in.s_addr);
     return 1;
  }

  syslog(LOG_PRIORITY, "invalid ipv4 address string: *%s*", ipstr);

  return 0;
}


in_addr_t netmask(int prefix){
   if(prefix == 0)
      return( ~((in_addr_t) -1) );
   else
      return ~((1 << (32 - prefix)) - 1);
}


in_addr_t network(in_addr_t addr, int prefix){
   return addr & netmask(prefix);
}


in_addr_t broadcast(in_addr_t addr, int prefix){
   return addr | ~netmask(prefix);
}


int str_to_net_range(char *network_addr_prefix, struct smtp_acl *smtp_acl){
   in_addr_t net = 0;
   int prefix = 0;

   smtp_acl->low = 0;
   smtp_acl->high = 0;

   // By default we permit unless you specify "reject" (without quotes)
   // To be on the safer side we permit even if you misspell the word "reject"

   smtp_acl->rejected = 0;

   if(strcasestr(network_addr_prefix, "reject")){
      smtp_acl->rejected = 1;
   }

   char *p = strchr(network_addr_prefix, '/');
   if(!p) return 0;

   if(strlen(network_addr_prefix) > sizeof(smtp_acl->network_str)){
      syslog(LOG_PRIORITY, "line *%s* is longer than %ld bytes, discarded", network_addr_prefix, sizeof(smtp_acl->network_str));
      return 0;
   }

   char buf[SMALLBUFSIZE];
   snprintf(buf, sizeof(buf)-1, "%s", network_addr_prefix);

   *p = '\0';
   p++;

   // Even though the remaining part of the acl line continues with some number
   // then whitespace characters and the permit/reject action atoi() can still
   // figure out the numeric part properly, that's why I'm lazy here.
   prefix = atoi(p);

   // The prefix string (p) must start with a digit and the prefix integer must be in 0..32 range
   if(*p < 48 || *p > 57 || prefix < 0 || prefix > 32){
      syslog(LOG_PRIORITY, "error: invalid prefix: %s", p);
      return 0;
   }

   if(a_to_hl(network_addr_prefix, &net)){
      smtp_acl->low = network(net, prefix);
      smtp_acl->high = broadcast(net, prefix);
      smtp_acl->prefix = prefix;

      syslog(LOG_PRIORITY, "info: parsed acl *%s*", buf);

      return 1;
   }

   return 0;
}


void load_smtp_acl(struct smtp_acl *smtp_acl[]){
   int count=0;

   clear_smtp_acl(smtp_acl);
   init_smtp_acl(smtp_acl);

   FILE *f = fopen(SMTP_ACL_FILE, "r");
   if(!f){
      syslog(LOG_PRIORITY, "info: cannot open %s, piler-smtp accepts smtp connections from everywhere", SMTP_ACL_FILE);
      return;
   }

   char line[SMALLBUFSIZE];
   struct smtp_acl acl;

   while(fgets(line, sizeof(line)-1, f)){
      // Skip comments
      if(line[0] == ';' || line[0] == '#') continue;

      trimBuffer(line);

      // Skip empty line
      if(line[0] == 0) continue;

      char line2[SMALLBUFSIZE];
      int rc = 0;
      snprintf(line2, sizeof(line2)-1, "%s", line);

      if(is_valid_line(line) == 1 && str_to_net_range(line, &acl) == 1){
         add_smtp_acl(smtp_acl, line, &acl);
         count++;
         rc = 1;
      }

      if(!rc) syslog(LOG_PRIORITY, "error: failed to parse line: *%s*", line2);
   }

   fclose(f);

   // If we have entries on the smtp acl list, then add 127.0.0.1/8
   // to let the GUI health page connect to the piler-smtp daemon
   if(count){
      snprintf(line, sizeof(line)-1, "127.0.0.1/8 permit");

      if(str_to_net_range(line, &acl) == 1){
         add_smtp_acl(smtp_acl, line, &acl);
      }
   }
}


int is_blocked_by_pilerscreen(struct smtp_acl *smtp_acl[], char *ipaddr){
   struct smtp_acl *q=smtp_acl[0];
   in_addr_t addr = 0;

   // Empty acl, let it pass
   if(!q) return 0;

   if(a_to_hl(ipaddr, &addr) == 0){
      syslog(LOG_PRIORITY, "error: invalid smtp client address: *%s*", ipaddr);
      return 1;
   }

   while(q){
      if(addr >= q->low && addr <= q->high){
         if(q->rejected) syslog(LOG_PRIORITY, "denied connection from %s, acl: %s/%d reject", ipaddr, q->network_str, q->prefix);
         return q->rejected;
      }

      q = q->r;
   }

   syslog(LOG_PRIORITY, "denied connection from %s by implicit default deny", ipaddr);

   return 1;
}
