/*
 * sql.h, SJ
 */

#ifndef _SQL_H
 #define _SQL_H


int open_database(struct session_data *sdata, struct config *cfg);
int open_sphx(struct session_data *sdata, struct config *cfg);
void close_database(struct session_data *sdata);
void close_sphx(struct session_data *sdata);
int prepare_sql_statement(struct session_data *sdata, struct sql *sql, char *s);
int prepare_sphx_statement(struct session_data *sdata, struct sql *sql, char *s);
void p_query(struct session_data *sdata, char *s);
int p_exec_stmt(struct session_data *sdata, struct sql *sql);
int p_store_results(struct sql *sql);
int p_fetch_results(struct sql *sql);
void p_free_results(struct sql *sql);
void p_bind_init(struct sql *sql);
uint64 p_get_insert_id(struct sql *sql);
int p_get_affected_rows(struct sql *sql);
void close_prepared_statement(struct sql *sql);


#endif /* _PILER_H */
