/*
 * import.h, SJ
 */

#ifndef _IMPORT_H
   #define _IMPORT_H


int import_message(char *filename, struct session_data *sdata, struct __data *data, struct __config *cfg);
void update_import_job_stat(struct session_data *sdata, struct __data *data);

int read_gui_import_data(struct session_data *sdata, struct __data *data, char *folder_imap, char *skiplist, int dryrun, struct __config *cfg);

int import_from_maildir(char *directory, struct session_data *sdata, struct __data *data, int *tot_msgs, struct __config *cfg);
int import_from_mailbox(char *mailbox, struct session_data *sdata, struct __data *data, struct __config *cfg);
int import_mbox_from_dir(char *directory, struct session_data *sdata, struct __data *data, int *tot_msgs, struct __config *cfg);
int import_from_pop3_server(char *server, char *username, char *password, int port, struct session_data *sdata, struct __data *data, int dryrun, struct __config *cfg);
int import_from_imap_server(char *server, char *username, char *password, int port, struct session_data *sdata, struct __data *data, char *folder_imap, char *skiplist, int dryrun, struct __config *cfg);

int connect_to_pop3_server(int sd, char *username, char *password, struct __data *data, int use_ssl);
int process_pop3_emails(int sd, struct session_data *sdata, struct __data *data, int use_ssl, int dryrun, struct __config *cfg);

int connect_to_imap_server(int sd, int *seq, char *username, char *password, struct __data *data, int use_ssl);
int list_folders(int sd, int *seq, int use_ssl, struct __data *data);
int process_imap_folder(int sd, int *seq, char *folder, struct session_data *sdata, struct __data *data, int use_ssl, int dryrun, struct __config *cfg);
void send_imap_close(int sd, int *seq, struct __data *data, int use_ssl);

#endif /* _IMPORT_H */

