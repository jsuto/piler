/*
 * misc.h, SJ
 */

#ifndef _MISC_H
 #define _MISC_H

#include <sys/socket.h>
#include <openssl/ssl.h>
#include <sys/time.h>
#include <pwd.h>
#include <cfg.h>
#include "defs.h"

#define CHK_NULL(x, errmsg) if ((x)==NULL) { printf("error: %s\n", errmsg); return ERR; }
#define CHK_SSL(err, msg) if ((err)==-1) { printf("ssl error: %s\n", msg); return ERR; }

void get_extractor_list();
void __fatal(char *s);
long tvdiff(struct timeval a, struct timeval b);
char *split(char *str, int ch, char *buf, int buflen, int *result);
char *split_str(char *row, char *what, char *s, int size);
int trimBuffer(char *s);
int extractEmail(char *rawmail, char *email);
int extract_verp_address(char *email);
void make_random_string(unsigned char *buf, int buflen);
void create_id(char *id, unsigned char server_id);
int get_random_bytes(unsigned char *buf, int len, unsigned char server_id);
int readFromEntropyPool(int fd, void *_s, ssize_t n);
int recvtimeout(int s, char *buf, int len, int timeout);
int write1(struct net *net, void *buf, int buflen);
int recvtimeoutssl(struct net *net, char *buf, int len);
void close_connection(struct net *net);

void write_pid_file(char *pidfile);
int drop_privileges(struct passwd *pwd);

void init_session_data(struct session_data *sdata, struct config *cfg);
int read_from_stdin(struct session_data *sdata);
void strtolower(char *s);

int make_socket_non_blocking(int fd);
int create_and_bind(char *listen_addr, int listen_port);

int can_i_write_directory(char *dir);

void move_email(struct smtp_session *session);
int read_one_line(char *s, int slen, int c, char *buf, int buflen, int *rc, int *nullbyte);

int init_ssl_to_server(struct data *data);

#ifndef _GNU_SOURCE
   char *strcasestr(const char *s, const char *find);
#endif

int append_string_to_buffer(char **buffer, char *str);
int get_size_from_smtp_mail_from(char *s);

#endif /* _MISC_H */
