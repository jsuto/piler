/*
 * misc.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <syslog.h>
#include <unistd.h>
#include <ctype.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "misc.h"
#include "smtpcodes.h"
#include "errmsg.h"
#include "config.h"
#include "tai.h"


int get_build(){
   return BUILD;
}


void get_extractor_list(){
   printf("Extractors: ");

   #ifdef HAVE_PDFTOTEXT
      printf("%s ", HAVE_PDFTOTEXT);
   #endif

   #ifdef HAVE_CATDOC
      printf("%s ", HAVE_CATDOC);
   #endif

   #ifdef HAVE_CATPPT
      printf("%s ", HAVE_CATPPT);
   #endif

   #ifdef HAVE_XLS2CSV
      printf("%s ", HAVE_XLS2CSV);
   #endif

   #ifdef HAVE_PPTHTML
      printf("%s ", HAVE_PPTHTML);
   #endif

   #ifdef HAVE_UNRTF
      printf("%s ", HAVE_UNRTF);
   #endif

   #ifdef HAVE_TNEF
      printf("%s ", HAVE_TNEF);
   #endif

   printf("\n\n");
}


void __fatal(char *s){
   fprintf(stderr, "%s\n", s);
   exit(1);
}

/*
 * calculate the difference betwwen two timevals in [usec]
 */

long tvdiff(struct timeval a, struct timeval b){
   double res;

   res = (a.tv_sec * 1000000 + a.tv_usec) - (b.tv_sec * 1000000 + b.tv_usec);
   return (long) res;
}


/*
 * search something in a buffer
 */

int searchStringInBuffer(char *s, int len1, char *what, int len2){
   int i, k, r;

   for(i=0; i<len1; i++){
      r = 0;

      for(k=0; k<len2; k++){
         if(*(s+i+k) == *(what+k))
            r++;
      }

      if(r == len2)
         return i;
   }

   return 0;
}


int search_char_backward(char *buf, int buflen, char c){
   int n, m;

   m = buflen - 1 - 5;
   if(m < 0) m = 0;

   for(n=m; n<buflen; n++){
      if(*(buf + n) == c){
         return n;
      }
   }
   return -1;
}


/*
 * count a character in buffer
 */

int countCharacterInBuffer(char *p, char c){
   int i=0;

   for(; *p; p++){
      if(*p == c)
         i++;
   }

   return i;
}


void replaceCharacterInBuffer(char *p, char from, char to){
   int i, k=0;

   for(i=0; i<strlen(p); i++){
      if(p[i] == from){
         if(to > 0){
            p[k] = to;
            k++;
         }
      }
      else {
         p[k] = p[i];
         k++;
      }

   }

   p[k] = '\0';
}


/*
 * split a string by a character as delimiter
 */

char *split(char *str, int ch, char *buf, int buflen, int *result){
   char *p;

   *result = 0;

   if(str == NULL || buf == NULL || buflen < 2) return NULL;

   p = strchr(str, ch);
   if(p){
      *p = '\0';
   }

   snprintf(buf, buflen, "%s", str);

   if(p){
      *p = ch;
      *result = 1;
      p++;
   }

   return p;
}


/*
 * split a string by a string as delimiter
 */

char *split_str(char *row, char *what, char *s, int size){
   char *r;
   int len;

   memset(s, 0, size);

   if(row == NULL)
      return NULL;

   r = strstr(row, what);
   if(r == NULL){
      len = strlen(row);
      if(len > size)
         len = size;
   }
   else {
      len = strlen(row) - strlen(r);
      if(len > size)
         len = size;

      r += strlen(what);
   }

   if(s != NULL){
      strncpy(s, row, len);
      s[len] = '\0';
   }

   return r;
}


/*
 * trim trailing CR-LF
 */

int trimBuffer(char *s){
   int n=0;
   char *p;

   p = strrchr(s, '\n');
   if(p){
      *p = '\0';
      n++;
   }

   p = strrchr(s, '\r');
   if(p){
      *p = '\0';
      n++;
   }

   return n;
}


int extract_verp_address(char *email){
   char *p, *p1, *p2;
   char puf[SMALLBUFSIZE];

   // a VERP address is like archive+user=domain.com@myarchive.local

   if(!email) return 0;

   if(strlen(email) < 5) return 0;

   p1 = strchr(email, '+');
   if(p1){
      p2 = strchr(p1, '@');
      if(p2 && p2 > p1 + 2){
         if(strchr(p1+1, '=')){
            memset(puf, 0, sizeof(puf));

            memcpy(&puf[0], p1+1, p2-p1-1);
            p = strchr(puf, '=');
            if(p) *p = '@';
            strcpy(email, puf);
         }
      }

   }

   return 0;
}


int extractEmail(char *rawmail, char *email){
   char *p;

   memset(email, 0, SMALLBUFSIZE);

   p = strchr(rawmail, '<');
   if(p){
      snprintf(email, SMALLBUFSIZE-1, "%s", p+1);
      p = strchr(email, '>');
      if(p){
         *p = '\0';
         extract_verp_address(email);
         return 1;
      }
   }

   return 0;
}


/*
 * Generate a random string from /dev/urandom or
 * using the rand() function if not possible
 */

void make_random_string(char *buf, int buflen){
   int i, len, fd;
   int urandom=0;
   static char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
   unsigned char s[QUEUE_ID_LEN];

   len = strlen(alphanum);

   fd = open(RANDOM_POOL, O_RDONLY);
   if(fd != -1){
      if(readFromEntropyPool(fd, s, sizeof(s)) == sizeof(s)){
         for(i=0; i<QUEUE_ID_LEN; i++){
            *(buf+i) = alphanum[s[i] % len];
         }

         urandom = 1;
      }
      close(fd);
   }

   if(urandom == 1) return;

   for(i=0; i<buflen; i++){
      *(buf+i) = alphanum[rand() % len];
   }

}


void create_id(char *id, unsigned char server_id){
   int i;
   unsigned char buf[RND_STR_LEN/2];

   memset(id, 0, SMALLBUFSIZE);

   get_random_bytes(buf, RND_STR_LEN/2, server_id);

   for(i=0; i < RND_STR_LEN/2; i++){
      sprintf(id, "%02x", buf[i]);
      id += 2;
   }

}


/*
 * reading from pool
 */

int get_random_bytes(unsigned char *buf, int len, unsigned char server_id){
   int fd, ret=0;
   struct taia now;
   char nowpack[TAIA_PACK];

   /* the first 12 bytes are the taia timestamp */

   taia_now(&now);
   taia_pack(nowpack, &now);

   memcpy(buf, nowpack, 12);

   fd = open(RANDOM_POOL, O_RDONLY);
   if(fd == -1) return ret;

   *(buf + 12) = server_id;

   if(readFromEntropyPool(fd, buf+12+1, len-12-1) != len-12-1){
      syslog(LOG_PRIORITY, "%s: %s", ERR_CANNOT_READ_FROM_POOL, RANDOM_POOL);
   }
   
   close(fd);
   return ret;
}


/*
 * read random data from entropy pool
 */

int readFromEntropyPool(int fd, void *_s, ssize_t n){
   char *s = _s;
   ssize_t res, pos = 0;

   while(n > pos){
      res = read(fd, s + pos, n - pos);
      switch(res){
         case  -1: continue;
         case   0: return res;
         default : pos += res;
      }
   }
   return pos;
}


/*
 * recv() with timeout
 */

int recvtimeout(int s, char *buf, int len, int timeout){
    fd_set fds;
    int n;
    struct timeval tv;

    memset(buf, 0, len);

    FD_ZERO(&fds);
    FD_SET(s, &fds);

    tv.tv_sec = timeout;
    tv.tv_usec = TIMEOUT_USEC;

    n = select(s+1, &fds, NULL, NULL, &tv);
    if (n == 0) return -2; // timeout!
    if (n == -1) return -1; // error

    return recv(s, buf, len, 0);
}


int write1(struct net *net, void *buf, int buflen){
   int n;

   if(net->use_ssl == 1)
      n = SSL_write(net->ssl, buf, buflen);
   else
      n = send(net->socket, buf, buflen, 0);

   return n;
}


int ssl_want_retry(SSL *ssl, int ret, int timeout){
   int i;
   fd_set fds;
   struct timeval tv;
   int sock;

   // something went wrong.  I'll retry, die quietly, or complain
   i = SSL_get_error(ssl, ret);
   if(i == SSL_ERROR_NONE)
      return 1;
 
   tv.tv_sec = timeout/1000;
   tv.tv_usec = 0;
   FD_ZERO(&fds);
 
   switch(i){
      case SSL_ERROR_WANT_READ: // pause until the socket is readable
          sock = SSL_get_rfd(ssl);
          FD_SET(sock, &fds);
          i = select(sock+1, &fds, 0, 0, &tv);
          break;
 
      case SSL_ERROR_WANT_WRITE: // pause until the socket is writeable
         sock = SSL_get_wfd(ssl);
         FD_SET(sock, &fds);
         i = select(sock+1, 0, &fds, 0, &tv);
         break;
 
      case SSL_ERROR_ZERO_RETURN: // the sock closed, just return quietly
         i = 0;
         break;
 
      default: // ERROR - unexpected error code
         i = -1;
         break;
   };

   return i;
}


int ssl_read_timeout(SSL *ssl, void *buf, int len, int timeout){
   int i;

   while(1){
      i = SSL_read(ssl, (char*)buf, len);
      if(i > 0) break;
      i = ssl_want_retry(ssl, i, timeout);
      if(i <= 0) break;
   }

   return i;
}


int recvtimeoutssl(struct net *net, char *buf, int len){

    memset(buf, 0, len);

    if(net->use_ssl == 1){
       return ssl_read_timeout(net->ssl, buf, len-1, net->timeout);
    }
    else {
       return recvtimeout(net->socket, buf, len-1, net->timeout);
    }
}


void close_connection(struct net *net){
   close(net->socket);

   if(net->use_ssl == 1){
      SSL_shutdown(net->ssl);
      SSL_free(net->ssl);
      SSL_CTX_free(net->ctx);
      ERR_free_strings();
   }
}


void write_pid_file(char *pidfile){
   FILE *f;

   f = fopen(pidfile, "w");
   if(f){
      fprintf(f, "%d", (int)getpid());
      fclose(f);
   }
   else syslog(LOG_PRIORITY, "cannot write pidfile: %s", pidfile);
}


int drop_privileges(struct passwd *pwd){

   if(pwd->pw_uid > 0 && pwd->pw_gid > 0){

      if(getgid() != pwd->pw_gid){
         if(setgid(pwd->pw_gid)) return -1;
      }

      if(getuid() != pwd->pw_uid){
         if(setuid(pwd->pw_uid)) return -1;
      }

   }

   return 0;
}


void init_session_data(struct session_data *sdata, struct config *cfg){
   int i;


   sdata->fd = -1;

   create_id(&(sdata->ttmpfile[0]), cfg->server_id);
   unlink(sdata->ttmpfile);

   snprintf(sdata->filename, SMALLBUFSIZE-1, "%s", sdata->ttmpfile);

   snprintf(sdata->tmpframe, SMALLBUFSIZE-1, "%s.m", sdata->ttmpfile);
   unlink(sdata->tmpframe);

   memset(sdata->mailfrom, 0, SMALLBUFSIZE);
   snprintf(sdata->client_addr, SMALLBUFSIZE-1, "null");

   memset(sdata->attachments, 0, SMALLBUFSIZE);

   memset(sdata->fromemail, 0, SMALLBUFSIZE);

   sdata->duplicate_id = 0;

   sdata->restored_copy = 0;

   sdata->internal_sender = sdata->internal_recipient = sdata->external_recipient = 0;
   sdata->direction = 0;

   sdata->hdr_len = 0;
   sdata->tot_len = 0;
   sdata->stored_len = 0;
   sdata->num_of_rcpt_to = 0;

   sdata->ms_journal = 0;
   sdata->journal_envelope_length = 0;
   sdata->journal_bottom_length = 0;

   sdata->tre = '-';

   sdata->rav = AVIR_OK;

   sdata->spam_message = 0;

   sdata->customer_id = 0;

   sdata->__acquire = sdata->__parsed = sdata->__av = sdata->__store = sdata->__compress = sdata->__encrypt = 0;

   sdata->import = 0;

   for(i=0; i<MAX_RCPT_TO; i++) memset(sdata->rcptto[i], 0, SMALLBUFSIZE);

   time(&(sdata->now));
   sdata->delivered = sdata->retained = sdata->now;
   sdata->sent = 0;

   sdata->sql_errno = 0;

#ifdef HAVE_TWEAK_SENT_TIME
   sdata->sent += cfg->tweak_sent_time_offset;
#endif
}


int read_from_stdin(struct session_data *sdata){
   int fd, rc=ERR, n;
   char buf[MAXBUFSIZE];

   fd = open(sdata->ttmpfile, O_CREAT|O_EXCL|O_RDWR|O_TRUNC, S_IRUSR|S_IWUSR);
   if(fd == -1){
      syslog(LOG_PRIORITY, "%s: cannot open ttmpfile", sdata->ttmpfile);
      return rc;
   }

   while((n = read(0, buf, sizeof(buf))) > 0){
      sdata->tot_len += write(fd, buf, n);
   }

   if(fsync(fd)){
      syslog(LOG_PRIORITY, "%s: fsync() error", sdata->ttmpfile);
   }
   else rc = OK;

   close(fd);

   return rc;
}


void strtolower(char *s){
   for(; *s; s++){
      if(*s >= 65 && *s <= 90) *s = tolower(*s);
   }
}


void *get_in_addr(struct sockaddr *sa){
   if(sa->sa_family == AF_INET) return &(((struct sockaddr_in*)sa)->sin_addr);
   return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int make_socket_non_blocking(int fd){
   int flags, s;

   flags = fcntl(fd, F_GETFL, 0);
   if(flags == -1){
      return -1;
   }

   flags |= O_NONBLOCK;

   s = fcntl(fd, F_SETFL, flags);
   if(s == -1){
      return -1;
   }

   return 0;
}


int create_and_bind(char *listen_addr, int listen_port){
   struct addrinfo hints;
   struct addrinfo *result, *rp;
   char port_string[8];
   int rc, fd;

   memset(&hints, 0, sizeof (struct addrinfo));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_PASSIVE;

   snprintf(port_string, sizeof(port_string)-1, "%d", listen_port);

   rc = getaddrinfo(listen_addr, port_string, &hints, &result);
   if(rc != 0){
      syslog(LOG_PRIORITY, "getaddrinfo for '%s': %s", listen_addr, gai_strerror(rc));
      return -1;
   }

   for(rp = result; rp != NULL; rp = rp->ai_next){
      fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
      if(fd == -1)
         continue;

      rc = bind(fd, rp->ai_addr, rp->ai_addrlen);
      if(rc == 0){
          break;
      }

      close(fd);
   }

   if(rp == NULL){
      syslog(LOG_PRIORITY, "cannot bind to port: %s:%d", listen_addr, listen_port);
      return -1;
   }

   freeaddrinfo(result);

   return fd;
}


int can_i_write_directory(char *dir){
   int fd;
   char filename[SMALLBUFSIZE];

   if(dir)
      snprintf(filename, sizeof(filename)-1, "%s/__piler_%d", dir, getpid());
   else
      snprintf(filename, sizeof(filename)-1, "__piler_%d", getpid());

   fd = open(filename, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP);
   if(fd == -1){
      return 0;
   }

   close(fd);
   unlink(filename);

   return 1;
}


void move_email(struct smtp_session *session){
   char buf[SMALLBUFSIZE];

   snprintf(buf, sizeof(buf)-1, "%d/%s", session->ttmpfile[0] % session->cfg->number_of_worker_processes, session->ttmpfile);

   if(rename(session->ttmpfile, buf)){
      syslog(LOG_PRIORITY, "ERROR: couldn't rename %s to %s", session->ttmpfile, buf);
   }
}


#ifndef _GNU_SOURCE
char *strcasestr(const char *s, const char *find){
   char c, sc;
   size_t len;

   if((c = *find++) != 0){
      c = tolower((unsigned char)c);
      len = strlen(find);
      do {
         do {
            if((sc = *s++) == 0)
               return (NULL);
         } while((char)tolower((unsigned char)sc) != c);
      } while (strncasecmp(s, find, len) != 0);
      s--;
   }

   return((char*)s);
}
#endif
