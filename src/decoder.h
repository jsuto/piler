/*
 * decoder.h, SJ
 */

#ifndef _DECODER_H
 #define _DECODER_H

int decodeBase64(char *p);
int decode_base64_to_buffer(char *p, int plen, unsigned char *b, int blen);
void decodeQP(char *p);
void decodeHTML(char *p, int utf8);
void decodeURL(char *p);
int utf8_encode(char *inbuf, int inbuflen, char *outbuf, int outbuflen, char *encoding);

#endif /* _DECODER_H */
