/*
 * memc.h, SJ
 */

#ifndef _MEMC_H
 #define _MEMC_H

void memcached_init(struct memcached_server *ptr, char *server_ip, int server_port);
int set_socket_options(struct memcached_server *ptr);
int memcached_connect(struct memcached_server *ptr);
int memcached_shutdown(struct memcached_server *ptr);
int memcached_add(struct memcached_server *ptr, char *cmd, char *key, char *value, unsigned int valuelen, unsigned int flags, unsigned long expiry);
int memcached_increment(struct memcached_server *ptr, char *key, unsigned long long value, unsigned long long *result);
char *memcached_get(struct memcached_server *ptr, char *key, unsigned int *len, unsigned int *flags);
int memcached_mget(struct memcached_server *ptr, char *key);
char *memcached_fetch_result(struct memcached_server *ptr, char *key, char *value, unsigned int *flags);

#endif /* _MEMC_H */
