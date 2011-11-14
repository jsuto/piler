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

int clamd_scan(char *tmpfile, char *engine, char *avinfo, struct __config *cfg);
int clamd_net_scan(char *tmpfile, char *engine, char *avinfo, struct __config *cfg);

// Dr.Web stuff

#define DRWEB_RESP_VIRUS 0x20
#define DRWEB_VIRUS_HAS_FOUND_MESSAGE "Virus has been found in message. See drwebd.log for details"

int drweb_scan(char *tmpfile, char *engine, char *avinfo, struct __config *cfg);


// avast! stuff

#define AVAST_READY "220"
#define AVAST_CMD_QUIT "QUIT\r\n"

#define AVAST_RESP_OK "200"
#define AVAST_RESP_ENGINE_ERROR "451"
#define AVAST_RESP_SYNTAX_ERROR "501"

#define AVAST_RESP_CLEAN "[+]"
#define AVAST_RESP_INFECTED "[L]"

int avast_scan(char *tmpfile, char *engine, char *avinfo, struct __config *cfg);
int avast_cmd_scan(char *tmpfile, char *engine, char *avinfo, struct __config *cfg);

// Kaspersky stuff

#define KAV_CMD_QUIT "QUIT\r\n"

#define KAV_READY "201 "
#define KAV_RESP_CLEAN "220 File is clean"
#define KAV_RESP_INFECTED "230 File is infected"
#define KAV_RESP_INFECTED_NAME "322-"
#define KAV_RESP_NOT_FOUND "525 File not found"

int kav_scan(char *tmpfile, char *engine, char *avinfo, struct __config *cfg);

// avg stuff

#define AVG_READY "220"
#define AVG_CMD_QUIT "QUIT\r\n"

#define AVG_RESP_OK "200"
#define AVG_RESP_VIRUS "403"
#define AVG_RESP_NOT_FOUND "404"
#define AVG_RESP_ERROR "501"

#define AVG_NOT_FOUND 404

int avg_scan(char *tmpdir, char *tmpfile, char *engine, char *avinfo, struct __config *cfg);


int moveMessageToQuarantine(struct session_data *sdata, struct __config *cfg);
void sendNotificationToPostmaster(struct session_data *sdata, char *rcpttoemail, char *fromemail, char *virusinfo, char *avengine, struct __config *cfg);

#endif /* _AV_H */
