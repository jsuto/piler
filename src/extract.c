#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <ctype.h>
#include <signal.h>
#include <piler.h>

#ifdef HAVE_ZIP
   #include <zip.h>
#endif

#define die(e) do { syslog(LOG_INFO, "error: helper: %s", e); exit(EXIT_FAILURE); } while (0);


int remove_xml(char *src, char *dest, int destlen, int *html){
   int i=0;

   memset(dest, 0, destlen);

   for(; *src; src++){
      if(*src == '<'){ *html = 1; continue; }
      if(*src == '>'){ *html = 0; continue; }

      if(*html == 0){
         if(i < destlen) *(dest+i) = *src;
         i++;
      }
   }

   return i;
}


#ifdef HAVE_ZIP
int extract_opendocument(struct session_data *sdata, struct parser_state *state, char *filename, char *prefix){
   int errorp, i=0, len=0, html=0;
   unsigned int len2;
   char buf[4*MAXBUFSIZE], puf[4*MAXBUFSIZE];
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

               len2 = remove_xml(buf, puf, sizeof(puf), &html);

               if(len2 > 0 && state->bodylen < BIGBUFSIZE-len2-1){
                  memcpy(&(state->b_body[state->bodylen]), puf, len2);
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


int unzip_file(struct session_data *sdata, struct parser_state *state, char *filename, int *rec, struct config *cfg){
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

            if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_INFO, "%s: writing zip content to '%s'", sdata->ttmpfile, extracted_filename);

            fd = open(extracted_filename, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
            if(fd != -1){
               zf = zip_fopen_index(z, i, 0);
               if(zf){
                  while((len = zip_fread(zf, buf, sizeof(buf))) > 0){
                     if(write(fd, buf, len) == -1) syslog(LOG_PRIORITY, "ERROR: error writing to fd in %s", __func__);
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
         syslog(LOG_PRIORITY, "error: attachment ('%s') is in encrypted zip file", sb.name);
      }

      i++;
   }


   zip_close(z);

   return 0;
}

#endif


#ifdef HAVE_TNEF

int extract_tnef(struct session_data *sdata, struct parser_state *state, char *filename, struct config *cfg){
   int rc=0, n, rec=1;
   char tmpdir[BUFLEN], buf[SMALLBUFSIZE];
   struct dirent **namelist;

   memset(tmpdir, 0, sizeof(tmpdir));
   make_random_string(&tmpdir[0], sizeof(tmpdir)-3);

   memcpy(&tmpdir[sizeof(tmpdir)-3], ".d", 2);

   if(mkdir(tmpdir, 0700)) return rc;

   snprintf(buf, sizeof(buf)-1, "%s --unix-paths -C %s %s", HAVE_TNEF, tmpdir, filename);

   if(system(buf) == -1) syslog(LOG_INFO, "error: running %s", buf);

   n = scandir(tmpdir, &namelist, NULL, alphasort);
   if(n < 0) syslog(LOG_INFO, "error: reading %s", tmpdir);
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


void kill_helper(){
   syslog(LOG_PRIORITY, "error: helper is killed by alarm");
   die("timeout for helper!");
}


void extract_attachment_content(struct session_data *sdata, struct parser_state *state, char *filename, char *type, int *rec, struct config *cfg){
   int link[2];
   ssize_t n;
   pid_t pid;
   char outbuf[MAXBUFSIZE];

   if(strcmp(type, "other") == 0 || strcmp(type, "text") == 0) return;

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

#ifdef HAVE_TNEF
   if(strcmp(type, "tnef") == 0){
      extract_tnef(sdata, state, filename, cfg);
      return;
   }
#endif


   /*
    * http://stackoverflow.com/questions/7292642/grabbing-output-from-exec
    */

   if(pipe(link) == -1){
      syslog(LOG_PRIORITY, "%s: cannot open link", sdata->ttmpfile);
      return;
   }

   if((pid = fork()) == -1){
      syslog(LOG_PRIORITY, "%s: cannot fork", sdata->ttmpfile);
      close(link[0]);
      close(link[1]);
      return;
   }

   if(pid == 0){
      dup2(link[1], STDOUT_FILENO);
      close(link[0]);
      close(link[1]);

      alarm(cfg->helper_timeout);
      sig_catch(SIGALRM, kill_helper);

   #ifdef HAVE_PDFTOTEXT
      if(strcmp(type, "pdf") == 0) execl(HAVE_PDFTOTEXT, HAVE_PDFTOTEXT, "-enc", "UTF-8", filename, "-", (char *) 0);
   #endif

   #ifdef HAVE_CATDOC
      if(strcmp(type, "doc") == 0) execl(HAVE_CATDOC, HAVE_CATDOC, "-d", "utf-8", filename, (char *) 0);
   #endif

   #ifdef HAVE_CATPPT
      if(strcmp(type, "ppt") == 0) execl(HAVE_CATPPT, HAVE_CATPPT, "-d", "utf-8", filename, (char *) 0);
   #endif

   #ifdef HAVE_XLS2CSV
      if(strcmp(type, "xls") == 0) execl(HAVE_XLS2CSV, HAVE_XLS2CSV, "-d", "utf-8", filename, (char *) 0);
   #endif

   #ifdef HAVE_PPTHTML
       if(strcmp(type, "ppt") == 0) execl(HAVE_PPTHTML, HAVE_PPTHTML, filename, (char *) 0);
   #endif

   #ifdef HAVE_UNRTF
      if(strcmp(type, "rtf") == 0) execl(HAVE_UNRTF, HAVE_UNRTF, "--text", filename, (char *) 0);
   #endif

      die("execl");
   }
   else {
      close(link[1]);
      while((n = read(link[0], outbuf, sizeof(outbuf))) > 0){
         if(state->bodylen < BIGBUFSIZE-n-1){
            memcpy(&(state->b_body[state->bodylen]), outbuf, n);
            state->bodylen += n;
         }
      }
      close(link[0]);
      wait(NULL);
      return;
   }


}


