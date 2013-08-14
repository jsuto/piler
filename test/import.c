/*
 * pilerimport.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>
#include <syslog.h>
#include <piler.h>


extern char *optarg;
extern int optind;


int connect_to_imap_server(int sd, int *seq, char *imapserver, char *username, char *password);
int list_folders(int sd, int *seq, char *folders, int foldersize);
int process_imap_folder(int sd, int *seq, char *folder, struct session_data *sdata, struct __data *data, struct __config *cfg);
int import_message2(char *filename, struct session_data *sdata, struct __data *data, struct __config *cfg);


int import_from_mailbox(char *mailbox, struct session_data *sdata, struct __data *data, struct __config *cfg){
   FILE *F, *f=NULL;
   int rc=ERR, tot_msgs=0, ret=OK;
   char buf[MAXBUFSIZE], fname[SMALLBUFSIZE];
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
            rc = import_message2(fname, sdata, data, cfg);
            printf("processed: %7d\r", tot_msgs); fflush(stdout);

            if(rc == ERR) ret = ERR;
            unlink(fname);
         }

         snprintf(fname, sizeof(fname)-1, "%ld-%d", t, tot_msgs);
         f = fopen(fname, "w+");
         continue;
      }

      if(f) fprintf(f, "%s", buf);
   }

   if(f){
      fclose(f);
      rc = import_message2(fname, sdata, data, cfg);
      printf("processed: %7d\r", tot_msgs); fflush(stdout);

      if(rc == ERR) ret = ERR;
      unlink(fname);
   }

   fclose(F);

   printf("\n");

   return ret;
}


int import_from_maildir(char *directory, struct session_data *sdata, struct __data *data, struct __config *cfg){
   DIR *dir;
   struct dirent *de;
   int rc=ERR, ret=OK, tot_msgs=0;
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
            rc = import_from_maildir(fname, sdata, data, cfg);
            if(rc == ERR) ret = ERR;
         }
         else {

            if(S_ISREG(st.st_mode)){
               rc = import_message2(fname, sdata, data, cfg);
               if(rc == OK) tot_msgs++;
               else ret = ERR;
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


int import_from_imap_server(char *imapserver, char *username, char *password, struct session_data *sdata, struct __data *data, struct __config *cfg){
   int rc=ERR, ret=OK, sd, seq=1;
   char *p, puf[MAXBUFSIZE];
   char folders[MAXBUFSIZE];
   
   if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
      printf("cannot create socket\n");
      return ERR;
   }

   if(connect_to_imap_server(sd, &seq, imapserver, username, password) == ERR){
      close(sd);
      return ERR;
   }


   list_folders(sd, &seq, &folders[0], sizeof(folders));


   p = &folders[0];
   do {
      memset(puf, 0, sizeof(puf));
      p = split(p, '\n', puf, sizeof(puf)-1);

      printf("processing folder: %s... ", puf);

      rc = process_imap_folder(sd, &seq, puf, sdata, data, cfg);
      if(rc == ERR) ret = ERR;

   } while(p);


   close(sd);

   return ret;
}


void usage(){
   printf("usage: pilerimport [-c <config file>] -e <eml file> | -m <mailbox file> | -d <directory> | -i <imap server> -u <imap username> -p <imap password>\n");
   exit(0);
}


int main(int argc, char **argv){
   int i, rc=0;
   char *configfile=CONFIG_FILE, *mailbox=NULL, *emlfile=NULL, *directory=NULL;
   char *imapserver=NULL, *username=NULL, *password=NULL;
   struct session_data sdata;
   struct __config cfg;
   struct __data data;


   while((i = getopt(argc, argv, "c:m:e:d:i:u:p:h?")) > 0){
       switch(i){

         case 'c' :
                    configfile = optarg;
                    break;

         case 'e' :
                    emlfile = optarg;
                    break;

         case 'd' :
                    directory = optarg;
                    break;

         case 'm' :
                    mailbox = optarg;
                    break;

         case 'i' :
                    imapserver = optarg;
                    break;

         case 'u' :
                    username = optarg;
                    break;

         case 'p' :
                    password = optarg;
                    break;

         case 'h' :
         case '?' :
                    usage();
                    break;


         default  : 
                    break;
       }
   }



   if(!mailbox && !emlfile && !directory && !imapserver) usage();


   cfg = read_config(configfile);

   setlocale(LC_CTYPE, cfg.locale);

   initrules(data.archiving_rules);
   initrules(data.retention_rules);

   if(emlfile) rc = import_message2(emlfile, &sdata, &data, &cfg);
   if(mailbox) rc = import_from_mailbox(mailbox, &sdata, &data, &cfg);
   if(directory) rc = import_from_maildir(directory, &sdata, &data, &cfg);
   if(imapserver && username && password) rc = import_from_imap_server(imapserver, username, password, &sdata, &data, &cfg);

   clearrules(data.archiving_rules);
   clearrules(data.retention_rules);

   return rc;
}


