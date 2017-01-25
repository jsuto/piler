/*
 * av.h, SJ
 */

#ifndef _AV_H
 #define _AV_H

#include "defs.h"
#include "cfg.h"


#define AV_OK 200
#define AV_VIRUS 403
#define AV_ERROR 501


// clamd stuff

#define CLAMD_RESP_CLEAN "OK"
#define CLAMD_RESP_INFECTED "FOUND"
#define CLAMD_RESP_ERROR "ERROR"

int clamd_scan(char *tmpfile, struct __config *cfg);

#endif /* _AV_H */
