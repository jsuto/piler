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


#define SKIPLIST "junk,trash,spam,draft,"
#define MBOX_ARGS 1024

extern char *optarg;
extern int optind;

int dryrun=0;
int import_from_gui=0;


void usage(){
   printf("\nusage: pilerimport\n\n");
   printf("    [-c <config file>]                Config file to use if not the default\n");
   printf("    -e <eml file>                     EML file to import\n");
   printf("    -m <mailbox file>                 Mbox file to import\n");
   printf("    -d <dir>                          Directory with EML files to import\n");
   printf("    -i <imap server>                  IMAP server to connect\n");
   printf("    -K <pop3 server>                  POP3 server to connect\n");
   printf("    -u <username>                     Username for imap/pop3 import\n");
   printf("    -p <password>                     Password for imap/pop3 import\n");
   printf("    -P <port>                         Port for imap/pop3 import (default: 143/110\n");
   printf("    -t <timeout>                      Timeout in sec for imap/pop3 import\n");
   printf("    -x <folder1,folder2,....folderN,> Comma separated list of imap folders to skip. Add the trailing comma!\n");
   printf("    -f <imap folder>                  IMAP folder name to import\n");
   printf("    -g <imap folder>                  Move email after import to this IMAP folder\n");
   printf("    -F <folder>                       Piler folder name to assign to this import\n");
   printf("    -R                                Assign IMAP folder names as Piler folder names\n");
   printf("    -b <batch limit>                  Import only this many emails\n");
   printf("    -s <start position>               Start importing POP3 emails from this position\n");
   printf("    -a <recipient>                    Add recipient to the To:/Cc: list\n");
   printf("    -D                                Dry-run, do not import anything\n");
   printf("    -o                                Only download emails for POP3/IMAP import\n");
   printf("    -r                                Remove imported emails\n");
   printf("    -z                                Reimport emails\n");
   printf("    -q                                Quiet mode\n");

   exit(0);
}


int main(int argc, char **argv){
   int i, c, rc=0, n_mbox=0, tot_msgs=0, port=143;
   char *configfile=CONFIG_FILE, *emlfile=NULL, *mboxdir=NULL, *mbox[MBOX_ARGS], *directory=NULL;
   char *imapserver=NULL, *pop3server=NULL, *username=NULL, *password=NULL, *skiplist=SKIPLIST, *folder=NULL, *folder_imap=NULL;
   struct session_data sdata;
   struct __config cfg;
   struct __data data;
   struct import import;

   for(i=0; i<MBOX_ARGS; i++) mbox[i] = NULL;

   srand(getpid());

   data.folder = 0;
   data.recursive_folder_names = 0;
   data.quiet = 0;


   import.import_job_id = import.total_messages = import.total_size = import.processed_messages = import.batch_processing_limit = 0;
   import.started = import.updated = import.finished = import.remove_after_import = import.reimport = 0;
   import.extra_recipient = import.move_folder = NULL;
   import.start_position = 1;
   import.download_only = 0;
   import.timeout = 30;

   data.import = &import;

   inithash(data.mydomains);
   initrules(data.archiving_rules);
   initrules(data.retention_rules);
   initrules(data.folder_rules);

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
            {"pop3server",   required_argument,  0,  'K' },
            {"username",     required_argument,  0,  'u' },
            {"password",     required_argument,  0,  'p' },
            {"port",         required_argument,  0,  'P' },
            {"skiplist",     required_argument,  0,  'x' },
            {"folder",       required_argument,  0,  'F' },
            {"folder_imap",  required_argument,  0,  'f' },
            {"add-recipient",required_argument,  0,  'a' },
            {"batch-limit",  required_argument,  0,  'b' },
            {"timeout",      required_argument,  0,  't' },
            {"start-position",required_argument,  0,  's' },
            {"quiet",        no_argument,        0,  'q' },
            {"recursive",    required_argument,  0,  'R' },
            {"remove-after-import",no_argument,  0,  'r' },
            {"move-folder",  required_argument,  0,  'g' },
            {"only-download",no_argument,        0,  'o' },
            {"reimport",     no_argument,        0,  'z' },
            {"gui-import",   no_argument,        0,  'G' },
            {"dry-run",      no_argument,        0,  'D' },
            {"help",         no_argument,        0,  'h' },
            {0,0,0,0}
         };

      int option_index = 0;

      c = getopt_long(argc, argv, "c:m:M:e:d:i:K:u:p:P:x:F:f:a:b:t:s:g:GDRrozqh?", long_options, &option_index);
#else
      c = getopt(argc, argv, "c:m:M:e:d:i:K:u:p:P:x:F:f:a:b:t:s:g:GDRrozqh?");
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

         case 'K' :
                    pop3server = optarg;
                    if(port == 143) port = 110;
                    break;

         case 'u' :
                    username = optarg;
                    break;

         case 'p' :
                    password = optarg;
                    break;

         case 'P' :
                    port = atoi(optarg);
                    break;

         case 'x' :
                    skiplist = optarg;
                    break;

         case 'F' :
                    folder = optarg;
                    break;

         case 'f' :
                    folder_imap = optarg;
                    break;

         case 'R' :
                    data.recursive_folder_names = 1;
                    break;

         case 'r' :
                    data.import->remove_after_import = 1;
                    break;

         case 'g' :
                    data.import->move_folder = optarg;
                    break;

         case 'o' :
                    data.import->download_only = 1;
                    dryrun = 1;
                    break;

         case 'b' :
                    data.import->batch_processing_limit = atoi(optarg);
                    break;

         case 't' :
                    data.import->timeout = atoi(optarg);
                    break;

         case 's' :
                    if(atoi(optarg) < 1){
                       printf("invalid start position: %s\n", optarg);
                       return -1;
                    }

                    data.import->start_position = atoi(optarg);
                    break;

         case 'z' :
                    data.import->reimport = 1;
                    break;

         case 'a' :
                    data.import->extra_recipient = optarg;
                    break;

         case 'G' :
                    import_from_gui = 1;
                    break;

         case 'D' :
                    dryrun = 1;
                    break;

         case 'q' :
                    data.quiet = 1;
                    break;

         case 'h' :
         case '?' :
                    usage();
                    break;


         default  : 
                    break;
       }
   }



   if(!mbox[0] && !mboxdir && !emlfile && !directory && !imapserver && !pop3server && import_from_gui == 0) usage();

   if(!can_i_write_current_directory()) __fatal("cannot write current directory!");

   cfg = read_config(configfile);

   if((data.recursive_folder_names == 1 || folder) && cfg.enable_folders == 0){
      printf("please set enable_folders=1 in piler.conf to use the folder options\n");
      return ERR;
   }

   /* make sure we don't discard messages without a valid Message-Id when importing manually */
   cfg.archive_emails_not_having_message_id = 1;

   if(read_key(&cfg)){
      printf("%s\n", ERR_READING_KEY);
      return ERR;
   }

   /* enable using the extra email address */
   if(data.import->extra_recipient) cfg.process_rcpt_to_addresses = 1;

   if(open_database(&sdata, &cfg) == ERR) return 0;


   setlocale(LC_CTYPE, cfg.locale);

   (void) openlog("pilerimport", LOG_PID, LOG_MAIL);

#ifdef HAVE_MEMCACHED
   memcached_init(&(data.memc), cfg.memcached_servers, 11211);
#endif

   if(folder){
      data.folder = get_folder_id(&sdata, &data, folder, 0);

      if(data.folder == ERR_FOLDER){
         data.folder = add_new_folder(&sdata, &data, folder, 0);
      }

      if(data.folder == ERR_FOLDER){
         printf("error: cannot get/add folder '%s'\n", folder);
         close_database(&sdata);
         return 0;
      }

   }

   load_rules(&sdata, &data, data.archiving_rules, SQL_ARCHIVING_RULE_TABLE);
   load_rules(&sdata, &data, data.retention_rules, SQL_RETENTION_RULE_TABLE);
   load_rules(&sdata, &data, data.folder_rules, SQL_FOLDER_RULE_TABLE);

   load_mydomains(&sdata, &data, &cfg);

   if(emlfile) rc = import_message(emlfile, &sdata, &data, &cfg);

   if(mbox[0]){
      for(i=0; i<n_mbox; i++){
         rc = import_from_mailbox(mbox[i], &sdata, &data, &cfg);
      }
   }
   if(mboxdir) rc = import_mbox_from_dir(mboxdir, &sdata, &data, &tot_msgs, &cfg);
   if(directory) rc = import_from_maildir(directory, &sdata, &data, &tot_msgs, &cfg);
   if(imapserver && username && password) rc = import_from_imap_server(imapserver, username, password, port, &sdata, &data, folder_imap, skiplist, dryrun, &cfg);
   if(pop3server && username && password) rc = import_from_pop3_server(pop3server, username, password, port, &sdata, &data, dryrun, &cfg);
   if(import_from_gui == 1) rc = read_gui_import_data(&sdata, &data, folder_imap, skiplist, dryrun, &cfg);

   clearrules(data.archiving_rules);
   clearrules(data.retention_rules);
   clearrules(data.folder_rules);

   clearhash(data.mydomains);

   close_database(&sdata);

   if(data.quiet == 0) printf("\n");

   return rc;
}


