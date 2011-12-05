/*
 * parser.h, SJ
 */

#ifndef _PARSER_H
 #define _PARSER_H

#include "cfg.h"
#include "config.h"
#include "defs.h"

struct _state parse_message(struct session_data *sdata, struct __config *cfg);
int parse_line(char *buf, struct _state *state, struct session_data *sdata, struct __config *cfg);

void init_state(struct _state *state);
unsigned long parse_date_header(char *s);
int isHexNumber(char *p);
int extract_boundary(char *p, struct _state *state);
void fixupEncodedHeaderLine(char *buf);
void fixupSoftBreakInQuotedPritableLine(char *buf, struct _state *state);
void fixupBase64EncodedLine(char *buf, struct _state *state);
void markHTML(char *buf, struct _state *state);
int appendHTMLTag(char *buf, char *htmlbuf, int pos, struct _state *state);
void translateLine(unsigned char *p, struct _state *state);
void fix_email_address_for_sphinx(char *s);
void reassembleToken(char *p);
void degenerateToken(unsigned char *p);
void fixURL(char *url);
int extractNameFromHeaderLine(char *s, char *name, char *resultbuf);

#endif /* _PARSER_H */
