/*
 * import.h, SJ
 */

#ifndef _IMPORT_H
   #define _IMPORT_H


int import_message(struct session_data *sdata, struct data *data, struct config *cfg);
int update_import_table(struct session_data *sdata, struct data *data);

int import_from_maildir(struct session_data *sdata, struct data *data, char *directory, struct config *cfg);
int import_from_mailbox(char *mailbox, struct session_data *sdata, struct data *data, struct config *cfg);
int import_mbox_from_dir(char *directory, struct session_data *sdata, struct data *data, struct config *cfg);
void import_from_pop3_server(struct session_data *sdata, struct data *data, struct config *cfg);
int import_from_imap_server(struct session_data *sdata, struct data *data, struct config *cfg);

int connect_to_pop3_server(struct data *data);
void process_pop3_emails(struct session_data *sdata, struct data *data, struct config *cfg);

int connect_to_imap_server(struct data *data);
int list_folders(struct data *data);
int process_imap_folder(char *folder, struct session_data *sdata, struct data *data, struct config *cfg);
void send_imap_close(struct data *data);

void import_from_pilerexport(struct session_data *sdata, struct data *data, struct config *cfg);

#endif /* _IMPORT_H */
