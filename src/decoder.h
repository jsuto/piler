/*
 * decoder.h, SJ
 */

#ifndef _DECODER_H
 #define _DECODER_H

void sanitiseBase64(char *s);
int decodeBase64(char *p);
void decodeUTF8(char *p);
void decodeQP(char *p);
void decodeHTML(char *p);
void decodeURL(char *p);

#endif /* _DECODER_H */
