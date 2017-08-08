/*
 * avir.c, SJ
 */

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <piler.h>


int do_av_check(char *filename, struct config *cfg){
   int rav = AVIR_OK;

   if(clamd_scan(filename, cfg) == AV_VIRUS) rav = AVIR_VIRUS;

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: done virus scanning", filename);

   return rav;
}
