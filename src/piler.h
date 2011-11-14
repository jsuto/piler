/*
 * piler.h, SJ
 */

#ifndef _PILER_H
 #define _PILER_H

#include <misc.h>
#include <list.h>
#include <parser.h>
#include <errmsg.h>
#include <smtpcodes.h>
#include <session.h>
#include <decoder.h>
#include <boundary.h>
#include <defs.h>
#include <sig.h>
#include <av.h>
#include <config.h>
#include <unistd.h>

#ifdef HAVE_MEMCACHED
   #include "memc.h"
#endif

int do_av_check(struct session_data *sdata, char *rcpttoemail, char *fromemail, char *virusinfo, struct __data *data, struct __config *cfg);

int make_header_digest(struct session_data *sdata, struct _state *state);
int make_body_digest(struct session_data *sdata);

int processMessage(struct session_data *sdata, struct _state *sstate, struct __config *cfg);
int store_message(struct session_data *sdata, struct _state *state, int stored, struct __config *cfg);

struct __config read_config(char *configfile);

void check_and_create_directories(struct __config *cfg, uid_t uid, gid_t gid);

void updateCounters(struct session_data *sdata, struct __data *data, struct __counters *counters, struct __config *cfg);

#endif /* _PILER_H */

