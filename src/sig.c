/*
 * sig.c
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <piler.h>


void sig_block(int sig){
  sigset_t ss;
  sigemptyset(&ss);
  sigaddset(&ss,sig);
  sigprocmask(SIG_BLOCK, &ss, (sigset_t *) 0);
}

void sig_unblock(int sig){
  sigset_t ss;
  sigemptyset(&ss);
  sigaddset(&ss,sig);
  sigprocmask(SIG_UNBLOCK, &ss, (sigset_t *) 0);
}

void sig_catch(int sig, void (*f)()){
  struct sigaction sa;
  sa.sa_handler = f;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  sigaction(sig, &sa, (struct sigaction *) 0);
}

void sig_uncatch(int sig){
   void (*sig_defaulthandler)() = SIG_DFL;
   sig_catch(sig, sig_defaulthandler);
}

void sig_pause(){
  sigset_t ss;
  sigemptyset(&ss);
  sigsuspend(&ss);
}

int wait_nohang(int *wstat){
   return waitpid(-1, wstat, WNOHANG);
}

signal_func *set_signal_handler(int signo, signal_func * func){
   struct sigaction act, oact;

   act.sa_handler = func;
   sigemptyset (&act.sa_mask);
   act.sa_flags = 0;

   if(sigaction(signo, &act, &oact) < 0) return SIG_ERR;

   return oact.sa_handler;
}

