/*
 * common.c
 */

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <locale.h>
#include <stdbool.h>
#include <assert.h>
#include "../src/piler.h"


int setup_and_parse_message(struct session_data *sdata, struct parser_state *state, struct data *data, char *filename, struct config *cfg){
   struct stat st;

   if(stat(filename, &st) != 0){
      fprintf(stderr, "%s is not found, skipping\n", filename);
      return 1;
   }

   init_session_data(sdata, cfg);

   sdata->delivered = 0;
   sdata->tot_len = st.st_size;

   snprintf(sdata->ttmpfile, SMALLBUFSIZE-1, "%s", filename);
   snprintf(sdata->filename, SMALLBUFSIZE-1, "%s", filename);
   snprintf(sdata->tmpframe, SMALLBUFSIZE-1, "%s.m", filename);

   *state = parse_message(sdata, 1, data, cfg);
   post_parse(sdata, data, state, cfg);

   return 0;
}
