/*
 * errmsg.h, SJ
 */

#ifndef _ERRMSG_H
 #define _ERRMSG_H

#define ERR_CANNOT_READ_FROM_POOL "ERR: cannot read from pool"
#define ERR_SIGACTION "sigaction failed"
#define ERR_OPEN_SOCKET "ERR: cannot open socket"
#define ERR_SET_SOCK_OPT "ERR: cannot set socket option"
#define ERR_BIND_TO_PORT "ERR: cannot bind to port"
#define ERR_LISTEN "ERR: cannot listen"
#define ERR_SETUID "ERR: setuid()"
#define ERR_SETGID "ERR: setgid()"
#define ERR_SELECT "ERR: select()"
#define ERR_CHDIR "ERR: chdir() to working directory failed"
#define ERR_OPEN_TMP_FILE "ERR: opening a tempfile"
#define ERR_TIMED_OUT "ERR: timed out"
#define ERR_FORK_FAILED "ERR: cannot fork()"
#define ERR_OPEN_DEDUP_FILE "ERR: cannot open dedup file"

#define ERR_MYSQL_CONNECT "Cannot connect to mysql server"
#define ERR_PSQL_CONNECT "Cannot connect to PSql server"
#define ERR_SQLITE3_OPEN "Cannot open sqlite3 database"
#define ERR_SQL_DATA "No valid data from sql table"

#define ERR_NON_EXISTENT_USER "ERR: non existent user in config file, see the 'username' variable"

#define ERR_READING_KEY "ERR: reading cipher key"

#define ERR_LICENCE "ERR: licence validation error"
#define ERR_LICENCE_IP "ERR: licenced IP-address doesn't match with listen_addr parameter"

#endif /* _ERRMSG_H */
