/*
 * import_mailbox.c
 */

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
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>
#include <getopt.h>
#include <syslog.h>
#include <piler.h>


int import_from_mailbox(char *mailbox, struct session_data *sdata, struct data *data, struct config *cfg){
   FILE *F, *f=NULL;
   int rc=ERR, tot_msgs=0, ret=OK;
   char buf[MAXBUFSIZE];
   time_t t;


   F = fopen(mailbox, "r");
   if(!F){
      printf("cannot open mailbox: %s\n", mailbox);
      return rc;
   }

   t = time(NULL);

   while(fgets(buf, sizeof(buf)-1, F)){

      if(buf[0] == 'F' && buf[1] == 'r' && buf[2] == 'o' && buf[3] == 'm' && buf[4] == ' '){
         tot_msgs++;
         if(f){
            fclose(f);
            f = NULL;
            rc = import_message(sdata, data, cfg);
            if(rc == ERR){
               printf("error importing: '%s'\n", data->import->filename);
               ret = ERR;
            }
            else unlink(data->import->filename);

            if(data->quiet == 0){ printf("processed: %7d\r", data->import->tot_msgs); fflush(stdout); }
         }

         snprintf(data->import->filename, sizeof(data->import->filename)-1, "%ld-%d", t, data->import->tot_msgs);
         f = fopen(data->import->filename, "w+");
         continue;
      }

      if(f) fprintf(f, "%s", buf);
   }

   if(f){
      fclose(f);
      rc = import_message(sdata, data, cfg);
      if(rc == ERR){
         printf("ERROR: error importing: '%s'\n", data->import->filename);
         ret = ERR;
      }
      else unlink(data->import->filename);

      if(data->quiet == 0){ printf("processed: %7d\r", data->import->tot_msgs); fflush(stdout); }
   }

   fclose(F);

   return ret;
}


int import_mbox_from_dir(char *directory, struct session_data *sdata, struct data *data, struct config *cfg){
   DIR *dir;
   struct dirent *de;
   int rc=ERR, ret=OK, i=0;
   int folder;
   char fname[SMALLBUFSIZE];
   struct stat st;

   dir = opendir(directory);
   if(!dir){
      printf("cannot open directory: %s\n", directory);
      return ERR;
   }


   while((de = readdir(dir))){
      if(strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) continue;

      snprintf(fname, sizeof(fname)-1, "%s/%s", directory, de->d_name);

      if(stat(fname, &st) == 0){
         if(S_ISDIR(st.st_mode)){
            folder = data->folder;
            rc = import_mbox_from_dir(fname, sdata, data, cfg);
            data->folder = folder;
            if(rc == ERR) ret = ERR;
         }
         else {

            if(S_ISREG(st.st_mode)){
               if(i == 0 && data->recursive_folder_names == 1){
                  folder = get_folder_id(sdata, data, fname, data->folder);
                  if(folder == ERR_FOLDER){
                     folder = add_new_folder(sdata, data, fname, data->folder);

                     if(folder == ERR_FOLDER){
                        printf("error: cannot get/add folder '%s' to parent id: %d\n", fname, data->folder);
                        return ERR;
                     }
                     else {
                        data->folder = folder;
                     }
                  }

               }

               rc = import_from_mailbox(fname, sdata, data, cfg);
               if(rc == OK) (data->import->tot_msgs)++;
               else ret = ERR;

               i++;
            }
            else {
               printf("%s is not a file\n", fname);
            }

         }
      }
      else {
         printf("cannot stat() %s\n", fname);
      }

   }
   closedir(dir);

   return ret;
}


