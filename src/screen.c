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
   // Skip comments
   if(line[0] == ';' || line[0] == '#') return 0;

   trimBuffer(line);

   // Skip empty line
   if(line[0] == 0) return 0;

   // Currently we support ipv4 stuff only, ie. valid characters are: 0-9./
   // and a line should look like "1.2.3.4/24 permit" or similar (without quotes)

   if(!strchr(line, '.') || !strchr(line, '/') || (!strchr(line, ' ') && !strchr(line, '\t')) ){
      return -1;
   }

   // ascii values:
   //   46: .
   //   47: /
   //   48-57: 0-9
   //   65-90: A-Z
   //   97-122: a-z
   for(; *line; line++){
      if(isalnum(*line) == 0 && isblank(*line) == 0 && *line != 46 && *line != 47) return -1;
   }

   return 1;
}


int a_to_hl(char *ipstr, in_addr_t *addr){
  struct in_addr in;

  if(inet_aton(ipstr, &in) == 1){
     *addr = ntohl(in.s_addr);
     return 1;
  }

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
   prefix = atoi(++p);

   if(a_to_hl(network_addr_prefix, &net)){
      smtp_acl->low = network(net, prefix);
      smtp_acl->high = broadcast(net, prefix);
      smtp_acl->prefix = prefix;

      syslog(LOG_PRIORITY, "info: parsed acl *%s* to low: %u, high: %u, prefix: %d, reject: %d", buf, smtp_acl->low, smtp_acl->high, smtp_acl->prefix, smtp_acl->rejected);

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
      int rc = is_valid_line(line);
      if(rc < 0){
         syslog(LOG_PRIORITY, "warn: invalid network range: *%s*", line);
      }

      if(rc == 1 && str_to_net_range(line, &acl) == 1){
         add_smtp_acl(smtp_acl, line, &acl);
         count++;
      }
   }

   fclose(f);

   // If we have entries on the smtp acl list, then add 127.0.0.1/8
   if(count){
      snprintf(line, sizeof(line)-1, "127.0.0.1/8 permit");

      if(str_to_net_range(line, &acl) == 1){
         add_smtp_acl(smtp_acl, line, &acl);
      }
   }
}


int is_blocked_by_pilerscreen(struct smtp_acl *smtp_acl[], char *ipaddr, struct config *cfg){
   struct smtp_acl *q;
   in_addr_t addr = 0;

   if(a_to_hl(ipaddr, &addr) == 0){
      syslog(LOG_PRIORITY, "error: invalid smtp client address: *%s*", ipaddr);
      return 1;
   }

   q = smtp_acl[0];

   // Empty network ranges list
   if(!q){
      syslog(LOG_PRIORITY, "info: empty network ranges list, pass");
      return 0;
   }

   while(q){
      if(addr >= q->low && addr <= q->high){
         if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "info: smtp client %s is on %s/%d rejected: %d", ipaddr, q->network_str, q->prefix, q->rejected);
         return q->rejected;
      }

      q = q->r;
   }

   return 1;
}
