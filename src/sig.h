/*
 * sig.h, SJ
 */

#ifndef _SIG_H
 #define _SIG_H

void sig_block(int sig);
void sig_unblock(int sig);
void sig_catch(int sig, void (*f)());
void sig_uncatch(int sig);
void sig_pause();
int wait_nohang(int *wstat);

#endif
