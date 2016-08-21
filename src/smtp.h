/*
 * smtp.h, SJ
 */

#ifndef _SMTP_H
 #define _SMTP_H

void process_command_ehlo_lhlo(struct session_data *sdata, struct __data *data, int *protocol_state, char *resp, int resplen, struct __config *cfg);
void process_command_starttls(struct session_data *sdata, struct __data *data, int *protocol_state, int *starttls, int new_sd, char *resp, int resplen, struct __config *cfg);
void process_command_mail_from(struct session_data *sdata, int *protocol_state, char *buf, char *resp, int resplen, struct __config *cfg);
void process_command_rcpt_to(struct session_data *sdata, int *protocol_state, char *buf, char *resp, int resplen);
void process_command_data(struct session_data *sdata, int *protocol_state, char *resp, int resplen);
void process_command_bdat(struct session_ctx *sctx, struct session_data *sdata, int *protocol_state, char *buf, char *resp, int resplen);
void process_command_quit(struct session_data *sdata, int *protocol_state, char *resp, int resplen, struct __config *cfg);
void process_command_reset(struct session_data *sdata, int *protocol_state, char *resp, int resplen, struct __config *cfg);

int read_bdat_data(struct session_ctx *sctx, struct session_data *sdata, int expected_bdat_len);
int extract_bdat_command(struct session_ctx *sctx, struct session_data *sdata, char *buf);

void send_buffered_response(struct session_data *sdata, struct __data *data, int starttls, int new_sd, char *resp, struct __config *cfg);

#endif /* _SMTP_H */
