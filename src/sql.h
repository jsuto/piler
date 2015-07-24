/*
 * sql.h, SJ
 */

#ifndef _SQL_H
 #define _SQL_H


int open_database(struct session_data *sdata, struct __config *cfg);
void close_database(struct session_data *sdata);
int prepare_sql_statement(struct session_data *sdata, MYSQL_STMT **stmt, char *s, struct __config *cfg);
void p_query(struct session_data *sdata, char *s, struct __config *cfg);
int p_exec_query(struct session_data *sdata, MYSQL_STMT *stmt, struct __data *data);
int p_store_results(struct session_data *sdata, MYSQL_STMT *stmt, struct __data *data);
int p_fetch_results(MYSQL_STMT *stmt);
void p_free_results(MYSQL_STMT *stmt);
void p_bind_init(struct __data *data);
uint64 p_get_insert_id(MYSQL_STMT *stmt);
int p_get_affected_rows(MYSQL_STMT *stmt);
void close_prepared_statement(MYSQL_STMT *stmt);


#endif /* _PILER_H */

