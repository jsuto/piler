#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <locale.h>
#include <getopt.h>
#include <stdbool.h>
#include <assert.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "../src/piler.h"


struct test_data_s_i {
   char s[SMALLBUFSIZE];
   int result;
};

struct test_data_s_uinti {
   char s[SMALLBUFSIZE];
   unsigned int result;
};

struct test_data_s_s {
   char s[SMALLBUFSIZE];
   char result[SMALLBUFSIZE];
};

struct test_data_s_s_i {
   char s1[SMALLBUFSIZE];
   char s2[SMALLBUFSIZE];
   int result;
};

struct digest_test {
   char s[SMALLBUFSIZE];
   char *digest1;
   char *digest2;
};


#define ASSERT(expr, value) if (!(expr)) { printf("assert failed: '%s'\n", value); abort(); } else { printf("."); }
#define TEST_HEADER() printf("%s() ", __func__);
#define TEST_FOOTER() printf(" OK\n");

int setup_and_parse_message(struct session_data *sdata, struct parser_state *state, struct data *data, char *filename, struct config *cfg);
