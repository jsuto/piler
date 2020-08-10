/*
 * memc.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <errno.h>
#include <piler.h>


int __recvtimeout(int s, char *buf, int len, int timeout){
    fd_set fds;
    int n;
    struct timeval tv;

    memset(buf, 0, MAXBUFSIZE);

    FD_ZERO(&fds);
    FD_SET(s, &fds);

    tv.tv_sec = 0;
    tv.tv_usec = timeout;

    n = select(s+1, &fds, NULL, NULL, &tv);
    if (n == 0) return -2; // timeout!
    if (n == -1) return -1; // error

    return recv(s, buf, len, 0);
}


void memcached_init(struct memcached_server *ptr, char *server_ip, int server_port){

   ptr->snd_timeout= 0;
   ptr->rcv_timeout= 100000;

   ptr->send_size= -1;
   ptr->recv_size= -1;

   ptr->fd = -1;

   ptr->last_read_bytes = 0;

   snprintf(ptr->server_ip, IPLEN-1, "%s", server_ip);
   ptr->server_port = server_port;

   ptr->initialised = 0;
}


int set_socket_options(struct memcached_server *ptr){
   int error, flag=1, flags;
   struct timeval waittime;
   struct linger linger;


   if(ptr->snd_timeout){
      waittime.tv_sec = 0;
      waittime.tv_usec = ptr->snd_timeout;

      error = setsockopt(ptr->fd, SOL_SOCKET, SO_SNDTIMEO, &waittime, (socklen_t)sizeof(struct timeval));
      if(error) return MEMCACHED_FAILURE;
   }


   if(ptr->rcv_timeout){
       waittime.tv_sec = 0;
       waittime.tv_usec = ptr->rcv_timeout;

       error = setsockopt(ptr->fd, SOL_SOCKET, SO_RCVTIMEO, &waittime, (socklen_t)sizeof(struct timeval));
       if(error) return MEMCACHED_FAILURE;
   }


   if(ptr->flags.no_block){
      linger.l_onoff = 1;
      linger.l_linger = 0; /* By default on close() just drop the socket */

      error = setsockopt(ptr->fd, SOL_SOCKET, SO_LINGER, &linger, (socklen_t)sizeof(struct linger));
      if(error) return MEMCACHED_FAILURE;
   }


   if(ptr->flags.tcp_nodelay){
      error = setsockopt(ptr->fd, IPPROTO_TCP, TCP_NODELAY, &flag, (socklen_t)sizeof(int));
      if(error) return MEMCACHED_FAILURE;
   }


   if(ptr->flags.tcp_keepalive){
      error= setsockopt(ptr->fd, SOL_SOCKET, SO_KEEPALIVE, &flag, (socklen_t)sizeof(int));
      if(error) return MEMCACHED_FAILURE;
   }


   /*if(ptr->tcp_keepidle > 0){
      error = setsockopt(ptr->fd, IPPROTO_TCP, TCP_KEEPIDLE, &ptr->tcp_keepidle, (socklen_t)sizeof(int));
      if(error) return MEMCACHED_FAILURE;
   }*/


   if(ptr->send_size > 0){
      error = setsockopt(ptr->fd, SOL_SOCKET, SO_SNDBUF, &ptr->send_size, (socklen_t)sizeof(int));
      if(error) return MEMCACHED_FAILURE;
   }


   if(ptr->recv_size > 0){
      error = setsockopt(ptr->fd, SOL_SOCKET, SO_RCVBUF, &ptr->recv_size, (socklen_t)sizeof(int));
      if(error) return MEMCACHED_FAILURE;
   }


   /* always use nonblocking IO to avoid write deadlocks */

   flags = fcntl(ptr->fd, F_GETFL, 0);
   if(flags == -1) return MEMCACHED_FAILURE;


   if((flags & O_NONBLOCK) == 0){
      if(fcntl(ptr->fd, F_SETFL, flags | O_NONBLOCK) == -1) return MEMCACHED_FAILURE;
   }

   return MEMCACHED_SUCCESS;
}


int memcached_connect(struct memcached_server *ptr){
   struct in_addr addr;

   if(ptr->last_read_bytes > 0) return MEMCACHED_SUCCESS;

   if(ptr->initialised == 0){

      ptr->addr.sin_family = AF_INET;
      ptr->addr.sin_port = htons(ptr->server_port);

      if(inet_aton(ptr->server_ip, &addr) == 0) return MEMCACHED_FAILURE;

      ptr->addr.sin_addr.s_addr = addr.s_addr;
      bzero(&(ptr->addr.sin_zero), 8);

      ptr->initialised = 1;
   }


   if((ptr->fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){ return MEMCACHED_FAILURE; }


   if(set_socket_options(ptr) != MEMCACHED_SUCCESS) return MEMCACHED_FAILURE;

   if(connect(ptr->fd, (struct sockaddr *)&ptr->addr, sizeof(struct sockaddr)) == -1){

      if(errno == EINPROGRESS || /* nonblocking mode - first return, */
            errno == EALREADY) /* nonblocking mode - subsequent returns */
      {

         struct pollfd fds[1];
         fds[0].fd = ptr->fd;
         fds[0].events = POLLOUT;
         int error = poll(fds, 1, 1000);

          if (error != 1 || fds[0].revents & POLLERR)
          {
            if (fds[0].revents & POLLERR)
            {
              int err;
              socklen_t len = sizeof (err);
              (void)getsockopt(ptr->fd, SOL_SOCKET, SO_ERROR, &err, &len);
              //ptr->cached_errno= (err == 0) ? errno : err;
            }

            (void)close(ptr->fd);
            ptr->fd= -1;

            return MEMCACHED_FAILURE;
          }
          return MEMCACHED_SUCCESS;

        }
        else if (errno == EISCONN) /* we are connected :-) */
        {
           return MEMCACHED_SUCCESS;
        }
        else if (errno != EINTR)
        {
          (void)close(ptr->fd);
          ptr->fd= -1;

        }

      return MEMCACHED_FAILURE;
   }

   return MEMCACHED_SUCCESS;
}


int memcached_shutdown(struct memcached_server *ptr){
   if(ptr->fd != -1){
      close(ptr->fd);
      ptr->fd = -1;
   }

   return MEMCACHED_SUCCESS;
}


int memcached_add(struct memcached_server *ptr, char *cmd, char *key, char *value, unsigned int valuelen, unsigned int flags, unsigned long expiry){
   int len=0;

   if(memcached_connect(ptr) != MEMCACHED_SUCCESS) return MEMCACHED_FAILURE;

   // cmd could be either 'add' or 'set'
   snprintf(ptr->buf, MAXBUFSIZE-1, "%s %s %u %lu %u \r\n", cmd, key, flags, expiry, valuelen);
   len = strlen(ptr->buf);

   strncat(ptr->buf, value, MAXBUFSIZE-strlen(ptr->buf)-1);
   strncat(ptr->buf, "\r\n", MAXBUFSIZE-strlen(ptr->buf)-1);

   len += valuelen + 2;

   send(ptr->fd, ptr->buf, len, 0);

   ptr->last_read_bytes = __recvtimeout(ptr->fd, ptr->buf, MAXBUFSIZE, ptr->rcv_timeout);

   if(strcmp("STORED\r\n", ptr->buf)) return MEMCACHED_FAILURE;

   return MEMCACHED_SUCCESS;
}


int memcached_increment(struct memcached_server *ptr, char *key, unsigned long long value, unsigned long long *result){
   char *p;

   if(memcached_connect(ptr) != MEMCACHED_SUCCESS) return MEMCACHED_FAILURE;

   snprintf(ptr->buf, MAXBUFSIZE, "incr %s %llu\r\n", key, value);
   send(ptr->fd, ptr->buf, strlen(ptr->buf), 0);

   ptr->last_read_bytes = __recvtimeout(ptr->fd, ptr->buf, MAXBUFSIZE, ptr->rcv_timeout);

   if(!strcmp("NOT_FOUND\r\n", ptr->buf)) return MEMCACHED_FAILURE;

   p = strchr(ptr->buf, '\r');
   if(p){
      *p = '\0';
      *result = strtoul(ptr->buf, NULL, 10);
   }

   return MEMCACHED_SUCCESS;
}


int memcached_mget(struct memcached_server *ptr, char *key){

   if(memcached_connect(ptr) != MEMCACHED_SUCCESS) return MEMCACHED_FAILURE;

   snprintf(ptr->buf, MAXBUFSIZE, "get %s \r\n", key);
   send(ptr->fd, ptr->buf, strlen(ptr->buf), 0);

   ptr->last_read_bytes = __recvtimeout(ptr->fd, ptr->buf, MAXBUFSIZE, ptr->rcv_timeout);

   if(ptr->last_read_bytes <= 0){
      memcached_shutdown(ptr);
      return MEMCACHED_FAILURE;
   }

   ptr->result = ptr->buf;

   return MEMCACHED_SUCCESS;
}


char *memcached_fetch_result(struct memcached_server *ptr, char *key, char *value, unsigned int *flags){
   char *p, *q;
   int len=0;

   if(ptr->last_read_bytes < 10) return NULL;

   if(strncmp("VALUE ", ptr->result, 6)) return NULL;

   /* first read, eg. 'VALUE aaaa 0 4' */

   p = strchr(ptr->result, '\r');
   if(!p) return NULL;

   *p = '\0';
   p = ptr->result + strlen(ptr->result) + 2;

   q = strrchr(ptr->result + 6, ' ');
   if(!q) return NULL;

   len = atoi(q+1);

   *q = '\0';

   q = strchr(ptr->result + 6, ' ');
   if(!q) return NULL;

   *q = '\0';
   *flags = atoi(q+1);

   snprintf(key, MAX_MEMCACHED_KEY_LEN-1, "%s", ptr->result + 6);

   /* now read 'len' bytes */

   q = p + len;
   *q = '\0';

   snprintf(value, MAXBUFSIZE-1, "%s", p);
   p = q + 2;

   ptr->result = p;

   return p;
}
