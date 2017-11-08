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
#include <fcntl.h>
#include <locale.h>
#include <getopt.h>
#include <stdbool.h>
#include <assert.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "../src/piler.h"


#define ASSERT(expr, value) if (!(expr)) { printf("assert failed: '%s'\n", value); abort(); } else { printf("."); }
#define TEST_HEADER() printf("%s() ", __func__);
#define TEST_FOOTER() printf(" OK\n");

