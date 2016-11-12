/*
 * piler.h, SJ
 */

#ifndef _PILER_H
 #define _PILER_H

#include <misc.h>
#include <parser.h>
#include <errmsg.h>
#include <smtpcodes.h>
#include <decoder.h>
#include <hash.h>
#include <rules.h>
#include <defs.h>
#include <tai.h>
#include <sig.h>
#include <av.h>
#include <rules.h>
#include <sql.h>
#include <import.h>
#include <smtp.h>
#include <config.h>
#include <unistd.h>

#ifdef HAVE_MEMCACHED
   #include "memc.h"
#endif

int read_key(struct __config *cfg);
void insert_offset(struct session_data *sdata, int server_id);

void tear_down_client(int n);

int do_av_check(struct session_data *sdata, char *virusinfo, struct __data *data, struct __config *cfg);

int make_digests(struct session_data *sdata, struct __config *cfg);
void digest_file(char *filename, char *digest);
void digest_string(char *s, char *digest);

void remove_stripped_attachments(struct parser_state *state);
int process_message(struct session_data *sdata, struct parser_state *state, struct __data *data, struct __config *cfg);
int reimport_message(struct session_data *sdata, struct parser_state *state, struct __data *data, struct __config *cfg);
int store_file(struct session_data *sdata, char *filename, int len, struct __config *cfg);
int remove_stored_message_files(struct session_data *sdata, struct parser_state *state, struct __config *cfg);
int store_attachments(struct session_data *sdata, struct parser_state *state, struct __data *data, struct __config *cfg);
int query_attachments(struct session_data *sdata, struct __data *data, struct ptr_array *ptr_arr);

struct __config read_config(char *configfile);

void check_and_create_directories(struct __config *cfg, uid_t uid, gid_t gid);

void update_counters(struct session_data *sdata, struct __data *data, struct counters *counters, struct __config *cfg);

int retrieve_email_from_archive(struct session_data *sdata, struct __data *data, FILE *dest, struct __config *cfg);
int file_from_archive_to_network(char *filename, int sd, int tls_enable, struct __data *data, struct __config *cfg);

int get_folder_id(struct session_data *sdata, struct __data *data, char *foldername, int parent_id);
int add_new_folder(struct session_data *sdata, struct __data *data, char *foldername, int parent_id);

int store_index_data(struct session_data *sdata, struct parser_state *state, struct __data *data, uint64 id, struct __config *cfg);

void extract_attachment_content(struct session_data *sdata, struct parser_state *state, char *filename, char *type, int *rec, struct __config *cfg);

int retrieve_file_from_archive(char *filename, int mode, char **buffer, FILE *dest, struct __config *cfg);

void load_mydomains(struct session_data *sdata, struct __data *data, struct __config *cfg);
int is_email_address_on_my_domains(char *email, struct __data *data);

int start_new_session(struct smtp_session **sessions, int socket, int *num_connections, struct __config *cfg);
void tear_down_session(struct smtp_session **sessions, int slot, int *num_connections);
struct smtp_session *get_session_by_socket(struct smtp_session **sessions, int max_connections, int socket);
void handle_data(struct smtp_session *session, char *readbuf, int readlen);
void free_smtp_session(struct smtp_session *session);

void child_sighup_handler(int sig);
void child_main(struct child *ptr);
pid_t child_make(struct child *ptr);
int search_slot_by_pid(pid_t pid);
void kill_children(int sig);
void p_clean_exit();
void fatal(char *s);
void initialise_configuration();

#endif /* _PILER_H */
