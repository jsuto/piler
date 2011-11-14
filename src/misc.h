/*
 * misc.h, SJ
 */

#ifndef _MISC_H
 #define _MISC_H

#include <sys/time.h>
#include <pwd.h>
#include <cfg.h>
#include "defs.h"

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
int isValidClapfID(char *p);

int isDottedIPv4Address(char *s);
int isEmailAddressOnList(char *list, char *tmpfile, char *email, struct __config *cfg);

void write_pid_file(char *pidfile);
int drop_privileges(struct passwd *pwd);


#ifndef _GNU_SOURCE
   char *strcasestr(const char *s, const char *find);
#endif

#endif /* _MISC_H */
