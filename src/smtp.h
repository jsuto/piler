/*
 * smtp.h, SJ
 */

#ifndef _SMTP_H
 #define _SMTP_H

void process_command_ehlo_lhlo(struct session_ctx *sctx, int *protocol_state, char *resp, int resplen);
void process_command_starttls(struct session_ctx *sctx, int *protocol_state, int *starttls, char *resp, int resplen);
void process_command_mail_from(struct session_ctx *sctx, int *protocol_state, char *buf, char *resp, int resplen);
void process_command_rcpt_to(struct session_ctx *sctx, int *protocol_state, char *buf, char *resp, int resplen);
void process_command_data(struct session_ctx *sctx, int *protocol_state, char *resp, int resplen);
void process_command_bdat(struct session_ctx *sctx, int *protocol_state, char *buf, char *resp, int resplen);
void process_command_quit(struct session_ctx *sctx, int *protocol_state, char *resp, int resplen);
void process_command_reset(struct session_ctx *sctx, int *protocol_state, char *resp, int resplen);

int read_bdat_data(struct session_ctx *sctx, int expected_bdat_len);
int extract_bdat_command(struct session_ctx *sctx, char *buf);

void send_buffered_response(struct session_ctx *sctx, int starttls, char *resp);

#endif /* _SMTP_H */
