/*
 * smtp.h, SJ
 */

#ifndef _SMTP_H
 #define _SMTP_H

#include <piler.h>

void process_smtp_command(struct smtp_session *session, char *buf, struct config *cfg);
void process_data(struct smtp_session *session, char *readbuf, int readlen);

void send_smtp_response(struct smtp_session *session, char *buf);
void process_command_helo(struct smtp_session *session, char *buf, int buflen);
void process_command_ehlo_lhlo(struct smtp_session *session, char *buf, int buflen);
void process_command_quit(struct smtp_session *session, char *buf, int buflen);
void process_command_reset(struct smtp_session *session);
void process_command_mail_from(struct smtp_session *session, char *buf);
void process_command_rcpt_to(struct smtp_session *session, char *buf);
void process_command_data(struct smtp_session *session, struct config *cfg);
void process_command_period(struct smtp_session *session);
void process_command_starttls(struct smtp_session *session);

void reset_bdat_counters(struct smtp_session *session);
void get_bdat_size_to_read(struct smtp_session *session, char *buf);
void process_bdat(struct smtp_session *session, char *readbuf, int readlen, struct config *cfg);

#endif
