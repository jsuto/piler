/*
 * parser.h, SJ
 */

#ifndef _PARSER_H
 #define _PARSER_H

#include "cfg.h"
#include "config.h"
#include "defs.h"

struct _state parse_message(struct session_data *sdata, int take_into_pieces, struct __config *cfg);
void post_parse(struct session_data *sdata, struct _state *state, struct __config *cfg);
int parse_line(char *buf, struct _state *state, struct session_data *sdata, int take_into_pieces, char *writebuffer, int writebuffersize, char *abuffer, int abuffersize, struct __config *cfg);

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
void split_email_address(char *s);
int does_it_seem_like_an_email_address(char *email);
void reassembleToken(char *p);
void degenerateToken(unsigned char *p);
void fixURL(char *url);
int extractNameFromHeaderLine(char *s, char *name, char *resultbuf);
char *determine_attachment_type(char *filename, char *type);
void parse_reference(struct _state *state, char *s);
int base64_decode_attachment_buffer(char *p, int plen, unsigned char *b, int blen);

#endif /* _PARSER_H */
