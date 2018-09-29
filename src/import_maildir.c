/*
 * import_maildir.c
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


int import_from_maildir(struct session_data *sdata, struct data *data, char *directory, struct config *cfg){
   DIR *dir;
   struct dirent *de;
   int rc=ERR, ret=OK, i=0;
   int folder;
   char *p, subdir[SMALLBUFSIZE];
   struct stat st;

   dir = opendir(directory);
   if(!dir){
      printf("cannot open directory: %s\n", directory);
      return ERR;
   }


   while((de = readdir(dir))){
      if(strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) continue;

      snprintf(data->import->filename, SMALLBUFSIZE-1, "%s/%s", directory, de->d_name);

      if(stat(data->import->filename, &st) == 0){
         if(S_ISDIR(st.st_mode)){
            folder = data->folder;
            snprintf(subdir, sizeof(subdir)-1, "%s", data->import->filename);
            rc = import_from_maildir(sdata, data, subdir, cfg);
            data->folder = folder;
            if(rc == ERR) ret = ERR;
         }
         else {

            if(S_ISREG(st.st_mode)){
               if(i == 0 && data->recursive_folder_names == 1){
                  p = strrchr(directory, '/');
                  if(p) p++;
                  else {
                     printf("ERROR: invalid directory name: '%s'\n", directory);
                     return ERR;
                  }

                  folder = get_folder_id(sdata, p, data->folder);
                  if(folder == ERR_FOLDER){
                     folder = add_new_folder(sdata, p, data->folder);

                     if(folder == ERR_FOLDER){
                        printf("error: cannot get/add folder '%s' to parent id: %d\n", p, data->folder);
                        return ERR;
                     }
                     else {
                        data->folder = folder;
                     }
                  }

               }

               rc = import_message(sdata, data, cfg);

               if(rc == OK) (data->import->tot_msgs)++;
               else if(rc == ERR){
                  printf("ERROR: error importing: '%s'\n", data->import->filename);
                  ret = ERR;
               }
 
               if(data->import->remove_after_import == 1 && rc != ERR) unlink(data->import->filename);

               i++;

               if(data->quiet == 0){ printf("processed: %7d\r", data->import->tot_msgs); fflush(stdout); }
            }
            else {
               printf("%s is not a file\n", data->import->filename);
            }

         }
      }
      else {
         printf("cannot stat() %s\n", data->import->filename);
      }

   }
   closedir(dir);

   return ret;
}
