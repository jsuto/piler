#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <piler.h>

#ifdef HAVE_ZIP
   #include <zip.h>
#endif


void remove_xml(char *buf, int *html){
   int i=0;
   char *p;

   p = buf;

   for(; *p; p++){
      if(*p == '<'){ *html = 1; }

      if(*html == 0){
         *(buf+i) = *p;
         i++;
      }

      if(*p == '>'){
         *html = 0;

         if(i > 2 && *(buf+i-1) != ' '){
            *(buf+i) = ' '; i++;
         }

      }

   }

   *(buf+i) = '\0';
}


#ifdef HAVE_ZIP
int extract_opendocument(struct session_data *sdata, struct _state *state, char *filename, char *prefix){
   int errorp, i=0, len=0, html=0;
   int len2;
   char buf[MAXBUFSIZE];
   struct zip *z;
   struct zip_stat sb;
   struct zip_file *zf;

   z = zip_open(filename, ZIP_CHECKCONS, &errorp);
   if(!z){
      syslog(LOG_INFO, "%s: error: corrupt zip file=%s, error code=%d", sdata->ttmpfile, filename, errorp);
      return 1;
   }

   memset(buf, 0, sizeof(buf));

   while(zip_stat_index(z, i, 0, &sb) == 0){
      if(ZIP_EM_NONE == sb.encryption_method && strncmp(sb.name, prefix, strlen(prefix)) == 0 && (int)sb.size > 0){

         zf = zip_fopen_index(z, i, 0);
         if(zf){
            while((len = zip_fread(zf, buf, sizeof(buf)-2)) > 0){

               remove_xml(buf, &html);
               len2 = strlen(buf);

               if(len2 > 0 && state->bodylen < BIGBUFSIZE-len2-1){
                  memcpy(&(state->b_body[state->bodylen]), buf, len2);
                  state->bodylen += len2;
               }

               memset(buf, 0, sizeof(buf));
            }
            zip_fclose(zf);
         }
         else syslog(LOG_PRIORITY, "%s: cannot extract '%s' from '%s'", sdata->ttmpfile, sb.name, filename);

         if(state->bodylen > BIGBUFSIZE-1024) break;
      }

      i++;
   }


   zip_close(z);

   return 0;
}


int unzip_file(struct session_data *sdata, struct _state *state, char *filename, int *rec, struct __config *cfg){
   int errorp, i=0, len=0, fd;
   char *p, extracted_filename[SMALLBUFSIZE], buf[MAXBUFSIZE];
   struct zip *z;
   struct zip_stat sb;
   struct zip_file *zf;

   (*rec)++;

   z = zip_open(filename, ZIP_CHECKCONS, &errorp);
   if(!z){
      syslog(LOG_INFO, "%s: error: corrupt zip file=%s, error code=%d", sdata->ttmpfile, filename, errorp);
      return 1;
   }

   while(zip_stat_index(z, i, 0, &sb) == 0){
      //printf("processing file inside the zip: %s, index: %d, size: %d\n", sb.name, sb.index, (int)sb.size);

      if(ZIP_EM_NONE == sb.encryption_method) {

         p = strrchr(sb.name, '.');

         if((int)sb.size > 0 && p && strcmp(get_attachment_extractor_by_filename((char*)sb.name), "other")){

            snprintf(extracted_filename, sizeof(extracted_filename)-1, "%s-%d-%d%s", sdata->ttmpfile, *rec, i, p);

            fd = open(extracted_filename, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
            if(fd != -1){
               zf = zip_fopen_index(z, i, 0);
               if(zf){
                  while((len = zip_fread(zf, buf, sizeof(buf))) > 0){
                     write(fd, buf, len);
                  }
                  zip_fclose(zf);
               }
               else syslog(LOG_PRIORITY, "%s: cannot extract '%s' from '%s'", sdata->ttmpfile, sb.name, extracted_filename);

               close(fd);

               extract_attachment_content(sdata, state, extracted_filename, get_attachment_extractor_by_filename(extracted_filename), rec, cfg);

               unlink(extracted_filename);

            }
            else {
               syslog(LOG_PRIORITY, "%s: cannot open '%s'", sdata->ttmpfile, extracted_filename);
            }
         }

      }
      else {
         syslog(LOG_PRIORITY, "ERR: attachment ('%s') is in encrypted zip file", sb.name);
      }

      i++;
   }


   zip_close(z);

   return 0;
}

#endif


#ifdef HAVE_TNEF

int extract_tnef(struct session_data *sdata, struct _state *state, char *filename, struct __config *cfg){
   int rc=0, n, rec=1;
   char tmpdir[BUFLEN], buf[SMALLBUFSIZE];
   struct dirent **namelist;

   memset(tmpdir, 0, sizeof(tmpdir));
   make_random_string(&tmpdir[0], sizeof(tmpdir)-3);

   memcpy(&tmpdir[sizeof(tmpdir)-3], ".d", 2);

   if(mkdir(tmpdir, 0700)) return rc;

   snprintf(buf, sizeof(buf)-1, "%s --unix-paths -C %s %s", HAVE_TNEF, tmpdir, filename);

   system(buf);

   n = scandir(tmpdir, &namelist, NULL, alphasort);
   if(n < 0) syslog(LOG_INFO, "error reading %s", tmpdir);
   else {
      while(n--){
         if(strcmp(namelist[n]->d_name, ".") && strcmp(namelist[n]->d_name, "..")){
            snprintf(buf, sizeof(buf)-1, "%s/%s", tmpdir, namelist[n]->d_name);

            extract_attachment_content(sdata, state, buf, get_attachment_extractor_by_filename(buf), &rec, cfg);

            unlink(buf);
         }

         free(namelist[n]);
      }
      free(namelist);
   }

   rmdir(tmpdir);

   return rc;
}

#endif

void read_content_with_popen(struct session_data *sdata, struct _state *state, char *cmd, struct __config *cfg){
   int len;
   char buf[MAXBUFSIZE];
   FILE *f;

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_INFO, "running command: '%s'", cmd);

   f = popen(cmd, "r");
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


void extract_attachment_content(struct session_data *sdata, struct _state *state, char *filename, char *type, int *rec, struct __config *cfg){
   char cmd[SMALLBUFSIZE], timeout[SMALLBUFSIZE];

   if(strcmp(type, "other") == 0) return;

   memset(cmd, 0, sizeof(cmd));
   memset(timeout, 0, sizeof(timeout));

#ifdef TIMEOUT_BINARY
   if(cfg->helper_timeout > 0) snprintf(timeout, sizeof(timeout)-1, "%s %d ", TIMEOUT_BINARY, cfg->helper_timeout);
#endif

#ifdef HAVE_PDFTOTEXT
   if(strcmp(type, "pdf") == 0) snprintf(cmd, sizeof(cmd)-1, "%s%s -enc UTF-8 %s -", timeout, HAVE_PDFTOTEXT, filename);
#endif

#ifdef HAVE_CATDOC
   if(strcmp(type, "doc") == 0) snprintf(cmd, sizeof(cmd)-1, "%s%s -d utf-8 %s", timeout, HAVE_CATDOC, filename);
#endif

#ifdef HAVE_CATPPT
   if(strcmp(type, "ppt") == 0) snprintf(cmd, sizeof(cmd)-1, "%s%s -d utf-8 %s", timeout, HAVE_CATPPT, filename);
#endif

#ifdef HAVE_XLS2CSV
   if(strcmp(type, "xls") == 0) snprintf(cmd, sizeof(cmd)-1, "%s%s -d utf-8 %s", timeout, HAVE_XLS2CSV, filename);
#endif

#ifdef HAVE_PPTHTML
   if(strcmp(type, "ppt") == 0) snprintf(cmd, sizeof(cmd)-1, "%s%s %s", timeout, HAVE_PPTHTML, filename);
#endif

#ifdef HAVE_UNRTF
   if(strcmp(type, "rtf") == 0) snprintf(cmd, sizeof(cmd)-1, "%s%s --text %s", timeout, HAVE_UNRTF, filename);
#endif

#ifdef HAVE_TNEF
   if(strcmp(type, "tnef") == 0){
      extract_tnef(sdata, state, filename, cfg);
      return;
   }
#endif

   if(strlen(cmd) > 12){
      read_content_with_popen(sdata, state, cmd, cfg);
      return;
   }


#ifdef HAVE_ZIP

   if(strcmp(type, "odf") == 0){
      extract_opendocument(sdata, state, filename, "content.xml");
      return;
   }

   if(strcmp(type, "docx") == 0){
      extract_opendocument(sdata, state, filename, "word/document.xml");
      return;
   }

   if(strcmp(type, "xlsx") == 0){
      extract_opendocument(sdata, state, filename, "xl/sharedStrings.xml");
      return;
   }

   if(strcmp(type, "pptx") == 0){
      extract_opendocument(sdata, state, filename, "ppt/slides/slide");
      return;
   }

   if(strcmp(type, "zip") == 0){
      if(*rec < MAX_ZIP_RECURSION_LEVEL){
         unzip_file(sdata, state, filename, rec, cfg);
      }
      else {
         syslog(LOG_PRIORITY, "%s: multiple recursion level zip attachment, skipping %s", sdata->ttmpfile, filename);
      }
   }
#endif

}


