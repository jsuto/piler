/*
 * pilerimport.c, SJ
 */

#define _FILE_OFFSET_BITS 64

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
#include <getopt.h>
#include <syslog.h>
#include <piler.h>


#define SKIPLIST "junk,trash,spam,draft"
#define MBOX_ARGS 1024

extern char *optarg;
extern int optind;

int quiet=0;

int connect_to_imap_server(int sd, int *seq, char *imapserver, char *username, char *password);
int list_folders(int sd, int *seq, char *folders, int foldersize);
int process_imap_folder(int sd, int *seq, char *folder, struct session_data *sdata, struct __data *data, struct __config *cfg);


int parse_it(char *filename, struct session_data *sdata, struct __data *data, struct __config *cfg){
   int i, rc=ERR, fd;
   struct stat st;
   struct _state state;


   init_session_data(sdata, cfg);

      if(stat(filename, &st) != 0){
         printf("cannot stat() %s\n", filename);
         return rc;
      }

      if(S_ISREG(st.st_mode) == 0){
         printf("%s is not a file\n", filename);
         return rc;
      }

      fd = open(filename, O_RDONLY);
      if(fd == -1){
         printf("cannot open %s\n", filename);
         return rc;
      }
      close(fd);

      snprintf(sdata->filename, SMALLBUFSIZE-1, "%s", filename);

      sdata->tot_len = st.st_size;


   
   sdata->sent = 0;

   state = parse_message(sdata, 1, data, cfg);
   post_parse(sdata, &state, cfg);

   unlink(sdata->tmpframe);

   for(i=1; i<=state.n_attachments; i++) unlink(state.attachments[i].internalname);

   return 0;
}


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
            rc = parse_it(fname, sdata, data, cfg);
            if(rc == ERR) ret = ERR;
            unlink(fname);

            if(quiet == 0) printf("processed: %7d\r", tot_msgs); fflush(stdout);
         }

         snprintf(fname, sizeof(fname)-1, "%ld-%d", t, tot_msgs);
         f = fopen(fname, "w+");
         continue;
      }

      if(f) fprintf(f, "%s", buf);
   }

   if(f){
      fclose(f);
      rc = parse_it(fname, sdata, data, cfg);
      if(rc == ERR) ret = ERR;
      unlink(fname);

      if(quiet == 0) printf("processed: %7d\r", ++tot_msgs); fflush(stdout);
   }

   fclose(F);

   return ret;
}


int import_mbox_from_dir(char *directory, struct session_data *sdata, struct __data *data, int *tot_msgs, struct __config *cfg){
   DIR *dir;
   struct dirent *de;
   int rc=ERR, ret=OK;
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
            rc = import_mbox_from_dir(fname, sdata, data, tot_msgs, cfg);
            if(rc == ERR) ret = ERR;
         }
         else {

            if(S_ISREG(st.st_mode)){
               rc = import_from_mailbox(fname, sdata, data, cfg);
               if(rc == OK) (*tot_msgs)++;
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


int import_from_maildir(char *directory, struct session_data *sdata, struct __data *data, int *tot_msgs, struct __config *cfg){
   DIR *dir;
   struct dirent *de;
   int rc=ERR, ret=OK;
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
            rc = import_from_maildir(fname, sdata, data, tot_msgs, cfg);
            if(rc == ERR) ret = ERR;
         }
         else {

            if(S_ISREG(st.st_mode)){
               rc = parse_it(fname, sdata, data, cfg);
               if(rc == OK) (*tot_msgs)++;
               else ret = ERR;

               if(quiet == 0) printf("processed: %7d\r", *tot_msgs); fflush(stdout);
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


int import_from_imap_server(char *imapserver, char *username, char *password, struct session_data *sdata, struct __data *data, char *skiplist, struct __config *cfg){
   int rc=ERR, ret=OK, sd, seq=1, skipmatch;
   char *p, puf[SMALLBUFSIZE];
   char *q, muf[SMALLBUFSIZE];
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

      if(strlen(puf) < 1) continue;

      skipmatch = 0;

      if(skiplist && strlen(skiplist) > 0){
         q = skiplist;
         do {
            memset(muf, 0, sizeof(muf));
            q = split(q, ',', muf, sizeof(muf)-1);
            if(strncasecmp(puf, muf, strlen(muf)) == 0){
               skipmatch = 1;
               break;
            }
         } while(q);
      }

      if(skipmatch == 1){
         if(quiet == 0) printf("SKIPPING FOLDER: %s\n", puf);
         continue;
      }

      if(quiet == 0) printf("processing folder: %s... ", puf);

      rc = process_imap_folder(sd, &seq, puf, sdata, data, cfg);
      if(rc == ERR) ret = ERR;

   } while(p);


   close(sd);

   return ret;
}


void usage(){
   printf("usage: pilerimport [-c <config file>] -e <eml file> | -m <mailbox file> | -M <mailbox directory> | -d <directory> | -i <imap server> -u <imap username> -p <imap password>\n");
   exit(0);
}


int main(int argc, char **argv){
   int i, c, rc=0, n_mbox=0, tot_msgs=0;
   char *configfile=CONFIG_FILE, *emlfile=NULL, *mboxdir=NULL, *mbox[MBOX_ARGS], *directory=NULL;
   char *imapserver=NULL, *username=NULL, *password=NULL, *skiplist=SKIPLIST;
   struct session_data sdata;
   struct __config cfg;
   struct __data data;

   for(i=0; i<MBOX_ARGS; i++) mbox[i] = NULL;


   data.folder = 0;
   initrules(data.archiving_rules);
   initrules(data.retention_rules);


   while(1){

#ifdef _GNU_SOURCE
      static struct option long_options[] =
         {
            {"config",       required_argument,  0,  'c' },
            {"eml",          required_argument,  0,  'e' },
            {"dir",          required_argument,  0,  'd' },
            {"mbox",         required_argument,  0,  'm' },
            {"mboxdir",      required_argument,  0,  'M' },
            {"imapserver",   required_argument,  0,  'i' },
            {"username",     required_argument,  0,  'u' },
            {"password",     required_argument,  0,  'p' },
            {"skiplist",     required_argument,  0,  'x' },
            {"folder",       required_argument,  0,  'F' },
            {"help",         no_argument,        0,  'h' },
            {0,0,0,0}
         };

      int option_index = 0;

      c = getopt_long(argc, argv, "c:m:M:e:d:i:u:p:x:F:h?", long_options, &option_index);
#else
      c = getopt(argc, argv, "c:m:M:e:d:i:u:p:x:F:h?");
#endif

      if(c == -1) break;

      switch(c){

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
                    if(n_mbox < MBOX_ARGS){
                       mbox[n_mbox++] = optarg;
                    } else {
                       printf("too many -m <mailbox> arguments: %s\n", optarg);
                    }

                    break;

         case 'M' :
                    mboxdir = optarg;
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

         case 'x' :
                    skiplist = optarg;
                    break;

         case 'F' :
                    //folder = optarg;
                    break;

         case 'h' :
         case '?' :
                    usage();
                    break;


         default  : 
                    break;
       }
   }



   if(!mbox[0] && !mboxdir && !emlfile && !directory && !imapserver) usage();


   cfg = read_config(configfile);

   if(read_key(&cfg)){
      printf("%s\n", ERR_READING_KEY);
      return ERR;
   }

   mysql_init(&(sdata.mysql));
   mysql_options(&(sdata.mysql), MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&cfg.mysql_connect_timeout);
   if(mysql_real_connect(&(sdata.mysql), cfg.mysqlhost, cfg.mysqluser, cfg.mysqlpwd, cfg.mysqldb, cfg.mysqlport, cfg.mysqlsocket, 0) == 0){
      printf("cant connect to mysql server\n");
      return ERR;
   }

   mysql_real_query(&(sdata.mysql), "SET NAMES utf8", strlen("SET NAMES utf8"));
   mysql_real_query(&(sdata.mysql), "SET CHARACTER SET utf8", strlen("SET CHARACTER SET utf8"));

   setlocale(LC_CTYPE, cfg.locale);

   (void) openlog("pilerimport", LOG_PID, LOG_MAIL);

   load_rules(&sdata, &data, data.archiving_rules, SQL_ARCHIVING_RULE_TABLE);
   load_rules(&sdata, &data, data.retention_rules, SQL_RETENTION_RULE_TABLE);

   load_mydomains(&sdata, &data, &cfg);

   if(emlfile) rc = import_message(emlfile, &sdata, &data, &cfg);
   if(mbox[0]){
      for(i=0; i<n_mbox; i++){
         rc = import_from_mailbox(mbox[i], &sdata, &data, &cfg);
      }
   }
   if(mboxdir) rc = import_mbox_from_dir(mboxdir, &sdata, &data, &tot_msgs, &cfg);
   if(directory) rc = import_from_maildir(directory, &sdata, &data, &tot_msgs, &cfg);
   if(imapserver && username && password) rc = import_from_imap_server(imapserver, username, password, &sdata, &data, skiplist, &cfg);


   if(quiet == 0) printf("\n");

   clearrules(data.archiving_rules);
   clearrules(data.retention_rules);

   mysql_close(&(sdata.mysql));

   return rc;
}


