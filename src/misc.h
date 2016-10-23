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

int get_build();
void get_extractor_list();
void __fatal(char *s);
long tvdiff(struct timeval a, struct timeval b);
int searchStringInBuffer(char *s, int len1, char *what, int len2);
int search_char_backward(char *buf, int buflen, char c);
int countCharacterInBuffer(char *p, char c);
void replaceCharacterInBuffer(char *p, char from, char to);
char *split(char *str, int ch, char *buf, int buflen, int *result);
char *split_str(char *row, char *what, char *s, int size);
int trimBuffer(char *s);
int extractEmail(char *rawmail, char *email);
void make_random_string(char *buf, int buflen);
void create_id(char *id, unsigned char server_id);
int get_random_bytes(unsigned char *buf, int len, unsigned char server_id);
int readFromEntropyPool(int fd, void *_s, ssize_t n);
int recvtimeout(int s, char *buf, int len, int timeout);
int write1(int sd, void *buf, int buflen, int use_ssl, SSL *ssl);
int recvtimeoutssl(int s, char *buf, int len, int timeout, int use_ssl, SSL *ssl);
void close_connection(int sd, struct __data *data, int use_ssl);

void write_pid_file(char *pidfile);
int drop_privileges(struct passwd *pwd);

void init_session_data(struct session_data *sdata, struct __config *cfg);
int read_from_stdin(struct session_data *sdata);
void strtolower(char *s);

void *get_in_addr(struct sockaddr *sa);

int can_i_write_current_directory();

void move_email(struct smtp_session *session);

#ifndef _GNU_SOURCE
   char *strcasestr(const char *s, const char *find);
#endif

#endif /* _MISC_H */
