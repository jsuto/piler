/*
 * parser.h, SJ
 */

#ifndef _PARSER_H
 #define _PARSER_H

#include "cfg.h"
#include "config.h"
#include "defs.h"

struct parser_state parse_message(struct session_data *sdata, int take_into_pieces, struct __data *data, struct __config *cfg);
void post_parse(struct session_data *sdata, struct parser_state *state, struct __config *cfg);
int parse_line(char *buf, struct parser_state *state, struct session_data *sdata, int take_into_pieces, char *writebuffer, int writebuffersize, char *abuffer, int abuffersize, struct __data *data, struct __config *cfg);

void init_state(struct parser_state *state);
time_t parse_date_header(char *s, struct __config *cfg);
int isHexNumber(char *p);
int extract_boundary(char *p, struct parser_state *state);
void fixupEncodedHeaderLine(char *buf, int buflen);
void fixupSoftBreakInQuotedPritableLine(char *buf, struct parser_state *state);
void fixupBase64EncodedLine(char *buf, struct parser_state *state);
void markHTML(char *buf, struct parser_state *state);
int appendHTMLTag(char *buf, char *htmlbuf, int pos, struct parser_state *state);
void translateLine(unsigned char *p, struct parser_state *state);
void fix_email_address_for_sphinx(char *s);
void split_email_address(char *s);
int does_it_seem_like_an_email_address(char *email);
void reassembleToken(char *p);
void degenerateToken(unsigned char *p);
void fixURL(char *buf, int buflen);
void extractNameFromHeaderLine(char *s, char *name, char *resultbuf);
char *determine_attachment_type(char *filename, char *type);
char *get_attachment_extractor_by_filename(char *filename);
void parse_reference(struct parser_state *state, char *s);
int base64_decode_attachment_buffer(char *p, int plen, unsigned char *b, int blen);

#endif /* _PARSER_H */
