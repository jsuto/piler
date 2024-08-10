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


void usage(){
   printf("\nusage: pilerimport\n\n");
   printf("    [-c <config file>]                Config file to use if not the default\n");
   printf("    -e <eml file>                     EML file to import\n");
   printf("    -m <mailbox file>                 Mbox file to import\n");
   printf("    -d <dir>                          Directory with EML files to import\n");
   printf("    -i <imap server>                  IMAP server to connect\n");
   printf("    -K <pop3 server>                  POP3 server to connect\n");
   printf("    -u <username>                     Username for imap/pop3 import; overrides PILER_USERNAME from the environment\n");
   printf("    -p <password>                     Password for imap/pop3 import; overrides PILER_PASSWORD from the environment\n");
   printf("    -P <port>                         Port for imap/pop3 import (default: 143/110\n");
   printf("    -t <timeout>                      Timeout in sec for imap/pop3 import\n");
   printf("    -x <folder1,folder2,....folderN,> Comma separated list of imap folders to skip. Add the trailing comma!\n");
   printf("    -f <imap folder>                  IMAP folder name to import\n");
   printf("    -g <imap folder>                  Move email after import to this IMAP folder\n");
   printf("    -F <folder>                       Piler folder name to assign to this import\n");
   printf("    -R                                Assign IMAP folder names as Piler folder names\n");
   printf("    -b <batch limit>                  Import only this many emails\n");
   printf("    -s <start position>               Start importing POP3 emails from this position\n");
   printf("    -j <failed folder>                Move failed to import emails to this folder\n");
   printf("    -a <recipient>                    Add recipient to the To:/Cc: list\n");
   printf("    -T <id>                           Update import table at id=<id>\n");
   printf("    -Z <ms>                           Delay Z milliseconds in between emails being imported\n");
   printf("    -z <integer>                      Wait whenever the load average is above the number\n");
   printf("    -D                                Dry-run, do not import anything\n");
   printf("    -y                                Read pilerexport data from stdin\n");
   printf("    -o                                Only download emails for POP3/IMAP import\n");
   printf("    -r                                Remove imported emails\n");
   printf("    -q                                Quiet mode\n");
   printf("    -A <timestamp>                    Import emails sent after this timestamp\n");
   printf("    -B <timestamp>                    Import emails sent before this timestamp\n");

   exit(0);
}


int main(int argc, char **argv){
   int i, n_mbox=0, read_from_pilerexport=0;
   char *configfile=CONFIG_FILE, *mbox[MBOX_ARGS], *directory=NULL;
   char puf[SMALLBUFSIZE], *imapserver=NULL, *pop3server=NULL;
   struct session_data sdata;
   struct config cfg;
   struct data data;
   struct import import;
   struct net net;
   struct counters counters;

   bzero(&counters, sizeof(counters));

   for(i=0; i<MBOX_ARGS; i++) mbox[i] = NULL;

   srand(getpid());

   data.folder = 0;
   data.recursive_folder_names = 0;
   data.quiet = 0;


   import.import_job_id = import.total_messages = import.total_size = import.processed_messages = import.batch_processing_limit = 0;
   import.started = import.updated = import.finished = import.remove_after_import = 0;
   import.extra_recipient = import.move_folder = import.failed_folder = NULL;
   import.start_position = 1;
   import.download_only = 0;
   import.dryrun = 0;
   import.port = 143;
   import.server = NULL;
   import.username = getenv("PILER_USERNAME");
   import.password = getenv("PILER_PASSWORD");
   import.database = NULL;
   import.skiplist = SKIPLIST;
   import.folder_imap = NULL;
   memset(import.filename, 0, SMALLBUFSIZE);
   import.mboxdir = NULL;
   import.tot_msgs = 0;
   import.table_id = 0;
   import.folder = NULL;
   import.delay = 0;
   import.la_limit = 0;
   import.after = 0;
   import.before = 0;

   data.import = &import;

   net.socket = -1;
   net.timeout = 30;

   data.net = &net;

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
            {"table-id",     required_argument,  0,  'T' },
            {"delay",        required_argument,  0,  'Z' },
            {"loadaverage",  required_argument,  0,  'z' },
            {"quiet",        no_argument,        0,  'q' },
            {"recursive",    no_argument,        0,  'R' },
            {"remove-after-import",no_argument,  0,  'r' },
            {"failed-folder",  required_argument,  0,  'j' },
            {"move-folder",  required_argument,  0,  'g' },
            {"after",        required_argument,  0,  'A' },
            {"before",       required_argument,  0,  'B' },
            {"only-download",no_argument,        0,  'o' },
            {"read-from-export",no_argument,     0,  'y' },
            {"dry-run",      no_argument,        0,  'D' },
            {"help",         no_argument,        0,  'h' },
            {0,0,0,0}
         };

      int option_index = 0;

      int c = getopt_long(argc, argv, "c:m:M:e:d:i:K:u:p:P:x:F:f:a:b:t:s:g:j:T:Z:z:A:B:yDRroqh?", long_options, &option_index);
#else
      int c = getopt(argc, argv, "c:m:M:e:d:i:K:u:p:P:x:F:f:a:b:t:s:g:j:T:Z:z:A:B:yDRroqh?");
#endif

      if(c == -1) break;

      switch(c){

         case 'c' :
                    configfile = optarg;
                    break;

         case 'e' :
                    snprintf(data.import->filename, SMALLBUFSIZE-1, "%s", optarg);
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
                    data.import->mboxdir = optarg;
                    break;

         case 'i' :
                    imapserver = optarg;
                    data.import->server = optarg;
                    break;

         case 'K' :
                    pop3server = optarg;
                    data.import->server = optarg;
                    if(data.import->port == 143) data.import->port = 110;
                    break;

         case 'u' :
                    data.import->username = optarg;
                    break;

         case 'p' :
                    data.import->password = optarg;
                    break;

         case 'P' :
                    data.import->port = atoi(optarg);
                    break;

         case 'x' :
                    data.import->skiplist = optarg;
                    break;

         case 'F' :
                    data.import->folder = optarg;
                    break;

         case 'f' :
                    data.import->folder_imap = optarg;
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

         case 'j' :
                    data.import->failed_folder = optarg;
                    break;

         case 'o' :
                    data.import->download_only = 1;
                    data.import->dryrun = 1;
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

         case 'a' :
                    snprintf(puf, sizeof(puf)-1, "%s ", optarg);
                    data.import->extra_recipient = puf;
                    break;

         case 'T' :
                    if(atoi(optarg) < 1){
                       printf("invalid import table id: %s\n", optarg);
                       return -1;
                    }

                    data.import->table_id = atoi(optarg);
                    break;

         case 'Z' :
                    if(atoi(optarg) < 1){
                       printf("invalid delay value: %s\n", optarg);
                       return -1;
                    }

                    data.import->delay = atoi(optarg);
                    break;
         case 'z' :
                    if (atoi(optarg) < 1) {
                       fprintf(stderr, "invalid load average value: %s\n", optarg);
                       return -1;
                    }

                    data.import->la_limit = atoi(optarg);
                    break;
         case 'y' :
                    read_from_pilerexport = 1;
                    break;

         case 'D' :
                    data.import->dryrun = 1;
                    break;

         case 'q' :
                    data.quiet = 1;
                    break;

         case 'A' : data.import->after = atol(optarg);
                    break;

         case 'B' : data.import->before = atol(optarg);
                    break;

         case 'h' :
         case '?' :
                    usage();
                    break;


         default  :
                    break;
       }
   }


   if(!mbox[0] && !data.import->mboxdir && !data.import->filename[0] && !directory && !imapserver && !pop3server && !read_from_pilerexport) usage();

   if(data.import->failed_folder && !can_i_write_directory(data.import->failed_folder)){
      printf("cannot write failed directory '%s'\n", data.import->failed_folder);
      return ERR;
   }

   if(!can_i_write_directory(NULL)) __fatal("cannot write current directory!");

   cfg = read_config(configfile);

   memset(cfg.security_header, 0, MAXVAL);

   if((data.recursive_folder_names == 1 || data.import->folder) && cfg.enable_folders == 0){
      printf("please set enable_folders=1 in piler.conf to use the folder options\n");
      return ERR;
   }

   /* make sure we don't discard messages without a valid Message-Id when importing manually */
   cfg.archive_emails_not_having_message_id = 1;

   /* The mmap_dedup_test feature is expected to work with the piler daemon only */
   cfg.mmap_dedup_test = 0;

   if(read_key(&cfg)){
      printf("%s\n", ERR_READING_KEY);
      return ERR;
   }

   /* enable using the extra email address */
   if(data.import->extra_recipient) cfg.process_rcpt_to_addresses = 1;

   if(open_database(&sdata, &cfg) == ERR) return 0;

   if(cfg.rtindex && open_sphx(&sdata, &cfg) == ERR) return 0;

   setlocale(LC_CTYPE, cfg.locale);

   (void) openlog("pilerimport", LOG_PID, LOG_MAIL);

#ifdef HAVE_MEMCACHED
   memcached_init(&(data.memc), cfg.memcached_servers, 11211);
#endif

   if(data.import->folder){
      data.folder = get_folder_id(&sdata, data.import->folder, 0);

      if(data.folder == ERR_FOLDER){
         data.folder = add_new_folder(&sdata, data.import->folder, 0);
      }

      if(data.folder == ERR_FOLDER){
         printf("ERROR: cannot get/add folder '%s'\n", data.import->folder);
         close_database(&sdata);
         return 0;
      }

   }

   load_rules(&sdata, data.archiving_rules, SQL_ARCHIVING_RULE_TABLE);
   load_rules(&sdata, data.retention_rules, SQL_RETENTION_RULE_TABLE);
   load_rules(&sdata, data.folder_rules, SQL_FOLDER_RULE_TABLE);

   load_mydomains(&sdata, &data, &cfg);

   if(data.import->filename[0] != '\0') import_message(&sdata, &data, &counters, &cfg);

   if(mbox[0]){
      for(i=0; i<n_mbox; i++){
         import_from_mailbox(mbox[i], &sdata, &data, &counters, &cfg);
      }
   }
   if(data.import->mboxdir) import_mbox_from_dir(data.import->mboxdir, &sdata, &data, &counters, &cfg);
   if(directory) import_from_maildir(&sdata, &data, directory, &counters, &cfg);
   if(imapserver) import_from_imap_server(&sdata, &data, &counters, &cfg);
   if(pop3server) import_from_pop3_server(&sdata, &data, &counters, &cfg);
   if(read_from_pilerexport) import_from_pilerexport(&sdata, &data, &counters, &cfg);

   clearrules(data.archiving_rules);
   clearrules(data.retention_rules);
   clearrules(data.folder_rules);

   clearhash(data.mydomains);

   update_counters(&sdata, &data, &counters, &cfg);

   syslog(LOG_PRIORITY, "server=%s, user=%s, directory=%s, imported=%lld, duplicated=%lld, discarded=%lld", data.import->server, data.import->username, directory, counters.c_rcvd, counters.c_duplicate, counters.c_ignore);

   close_database(&sdata);

   if(cfg.rtindex) close_sphx(&sdata);

   if(data.quiet == 0) printf("\n");

   return 0;
}
