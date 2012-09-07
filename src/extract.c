#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <piler.h>


void extract_pdf(struct session_data *sdata, struct _state *state, char *filename, struct __config *cfg){
   int len;
   char buf[MAXBUFSIZE];
   FILE *f;

   snprintf(buf, sizeof(buf)-1, "%s -enc UTF-8 %s -", HAVE_PDFTOTEXT, filename);

   f = popen(buf, "r");
   if(f){
      while(fgets(buf, sizeof(buf)-1, f)){
         len = strlen(buf);

         if(state->bodylen < BIGBUFSIZE-len-1){
            memcpy(&(state->b_body[state->bodylen]), buf, len);
            state->bodylen += len;
         }
         else break;
      }

      fclose(f);
   }
   else syslog(LOG_PRIORITY, "%s: popen(): %s", sdata->ttmpfile, buf);

}


