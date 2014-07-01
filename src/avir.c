/*
 * avir.c, SJ
 */

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <piler.h>


int do_av_check(struct session_data *sdata, char *virusinfo, struct __data *data, struct __config *cfg){
   int rav = AVIR_OK;
   char avengine[SMALLBUFSIZE];

   if(sdata->need_scan == 0) return rav;

   memset(avengine, 0, SMALLBUFSIZE);

#ifdef HAVE_LIBCLAMAV
   const char *virname;
   unsigned int options=0;

   options = CL_SCAN_STDOPT | CL_SCAN_ARCHIVE | CL_SCAN_MAIL | CL_SCAN_OLE2;

   if(cfg->use_libclamav_block_max_feature == 1) options |= CL_SCAN_BLOCKMAX;

   if(cfg->clamav_block_encrypted_archives == 1) options |= CL_SCAN_BLOCKENCRYPTED;

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: trying to pass to libclamav", sdata->ttmpfile);

   if(cl_scanfile(sdata->ttmpfile, &virname, NULL, data->engine, options) == CL_VIRUS){
      memset(virusinfo, 0, SMALLBUFSIZE);
      strncpy(virusinfo, virname, SMALLBUFSIZE-1);
      rav = AVIR_VIRUS;
      snprintf(avengine, SMALLBUFSIZE-1, "libClamAV");
   }

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: virus info: '%s'", sdata->ttmpfile, virname);
#endif

#ifdef HAVE_CLAMD
   if(strlen(cfg->clamd_addr) > 3 && cfg->clamd_port > 0){
      if(clamd_net_scan(sdata->ttmpfile, avengine, virusinfo, cfg) == AV_VIRUS) rav = AVIR_VIRUS;
   } else {
      if(clamd_scan(sdata->ttmpfile, avengine, virusinfo, cfg) == AV_VIRUS) rav = AVIR_VIRUS;
   }
#endif

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: done virus scanning", sdata->ttmpfile);

   return rav;
}

