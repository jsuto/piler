/*
 * parser.h, SJ
 */

#ifndef _PARSER_H
 #define _PARSER_H

#include "cfg.h"
#include "config.h"
#include "defs.h"

struct _state parseMessage(struct session_data *sdata, struct __config *cfg);
struct _state parseBuffer(struct session_data *sdata, struct __config *cfg);
int parseLine(char *buf, struct _state *state, struct session_data *sdata, struct __config *cfg);

void initState(struct _state *state);
void freeState(struct _state *state);
int extract_boundary(char *p, struct _state *state);
int extractNameFromHeaderLine(char *s, char *name, char *resultbuf);
int attachment_by_type(struct _state *state, char *type);
void fixupEncodedHeaderLine(char *buf);
void fixupSoftBreakInQuotedPritableLine(char *buf, struct _state *state);
void fixupBase64EncodedLine(char *buf, struct _state *state);
void markHTML(char *buf, struct _state *state);
int appendHTMLTag(char *buf, char *htmlbuf, int pos, struct _state *state);
void fixupHTML(char *buf, struct _state *state, struct __config *cfg);
int isSkipHTMLTag(char *s);
void translateLine(unsigned char *p, struct _state *state);
void reassembleToken(char *p);
void degenerateToken(unsigned char *p);
int countInvalidJunkLines(char *p);
int countInvalidJunkCharacters(char *p, int replace_junk);
int countNonPrintableCharacters(char *p);
int isHexNumber(char *p);
void fixURL(char *url);
void fixFQDN(char *fqdn);
void getTLDFromName(char *name);
int isItemOnList(char *item, char *list, char *extralist);
unsigned long parse_date_header(char *s);

#endif /* _PARSER_H */
