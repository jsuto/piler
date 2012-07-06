/*
 * misc.h, SJ
 */

#ifndef _MISC_H
 #define _MISC_H

#include <sys/time.h>
#include <pwd.h>
#include <cfg.h>
#include "defs.h"

int get_build();
void __fatal(char *s);
long tvdiff(struct timeval a, struct timeval b);
int searchStringInBuffer(char *s, int len1, char *what, int len2);
int countCharacterInBuffer(char *p, char c);
void replaceCharacterInBuffer(char *p, char from, char to);
char *split(char *row, int ch, char *s, int size);
char *split_str(char *row, char *what, char *s, int size);
void trimBuffer(char *s);
int extractEmail(char *rawmail, char *email);
void create_id(char *id);
int get_random_bytes(unsigned char *buf, int len);
int readFromEntropyPool(int fd, void *_s, size_t n);
int recvtimeout(int s, char *buf, int len, int timeout);

void write_pid_file(char *pidfile);
int drop_privileges(struct passwd *pwd);

int is_email_address_on_my_domains(char *email, struct __config *cfg);
void init_session_data(struct session_data *sdata);
int read_from_stdin(struct session_data *sdata);

#ifndef _GNU_SOURCE
   char *strcasestr(const char *s, const char *find);
#endif

#endif /* _MISC_H */
