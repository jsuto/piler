/*
 * smtpcodes.h, SJ
 */

// SMTP states

#define SMTP_STATE_INIT 0
#define SMTP_STATE_HELO 1
#define SMTP_STATE_MAIL_FROM 2
#define SMTP_STATE_RCPT_TO 3
#define SMTP_STATE_DATA 4
#define SMTP_STATE_PERIOD 5
#define SMTP_STATE_QUIT 6
#define SMTP_STATE_FINISHED 7
#define SMTP_STATE_BDAT 8

// SMTP commands

#define SMTP_CMD_HELO "HELO"
#define SMTP_CMD_EHLO "EHLO"
#define SMTP_CMD_HELP "HELP"
#define SMTP_CMD_MAIL_FROM "MAIL FROM:"
#define SMTP_CMD_RCPT_TO "RCPT TO:"
#define SMTP_CMD_DATA "DATA"
#define SMTP_CMD_BDAT "BDAT"
#define SMTP_CMD_PERIOD "\x0d\x0a\x2e\x0d\x0a"
#define SMTP_CMD_QUIT "QUIT"
#define SMTP_CMD_RESET "RSET"
#define SMTP_CMD_NOOP "NOOP"
#define SMTP_CMD_XFORWARD "XFORWARD"
#define SMTP_CMD_XCLIENT "XCLIENT"
#define SMTP_CMD_STARTTLS "STARTTLS"

// SMTP responses

#define SMTP_RESP_221_PILER_SMTP_OK "221 piler-smtp is OK\r\n"
#define SMTP_RESP_220_BANNER "220 %s ESMTP\r\n"
#define SMTP_RESP_220_READY_TO_START_TLS "220 Ready to start TLS\r\n"
#define SMTP_RESP_221_GOODBYE "221 %s Goodbye\r\n"
#define SMTP_RESP_250_OK "250 Ok\r\n"
#define SMTP_RESP_250_EXTENSIONS "250-%s\r\n250-SIZE %d\r\n%s250 8BITMIME\r\n"

#define SMTP_EXTENSION_STARTTLS "250-STARTTLS\r\n"
#define SMTP_EXTENSION_CHUNKING "250-CHUNKING\r\n"

#define SMTP_RESP_250_BDAT "250 octets received\r\n"

#define SMTP_RESP_354_DATA_OK "354 Send mail data; end it with <CRLF>.<CRLF>\r\n"

#define SMTP_RESP_421_ERR "421 %s Error: timed out\r\n"
#define SMTP_RESP_421_ERR_TMP "421 %s service not available\r\n"
#define SMTP_RESP_421_ERR_WRITE_FAILED "421 writing queue file failed\r\n"
#define SMTP_RESP_421_ERR_ALL_PORTS_ARE_BUSY "421 All server ports are busy\r\n"
#define SMTP_RESP_451_ERR_TOO_MANY_REQUESTS "451 Too many requests, try again later\r\n"
#define SMTP_RESP_451_ERR "451 Error in processing, try again later\r\n"
#define SMTP_RESP_454_ERR_TLS_TEMP_ERROR "454 TLS not available currently\r\n"

#define SMTP_RESP_502_ERR "502 Command not implemented\r\n"
#define SMTP_RESP_503_ERR "503 Bad command sequence\r\n"
#define SMTP_RESP_550_ERR_INVALID_RECIPIENT "550 Invalid recipient\r\n"
#define SMTP_RESP_550_ERR_YOU_ARE_BANNED_BY_LOCAL_POLICY "550 You are banned by local policy\r\n"
#define SMTP_RESP_550_ERR "550 Service currently unavailable\r\n"
#define SMTP_RESP_552_ERR_TOO_BIG_EMAIL "552 Too big email\r\n"


// LMTP commands

#define LMTP_CMD_LHLO "LHLO"

// LMTP responses

#define LMTP_RESP_220_BANNER "220 %s LMTP\r\n"
