/*
 * piler.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <locale.h>
#include <errno.h>
#include <piler.h>

#define PROGNAME "piler"

extern char *optarg;
extern int optind;

int quit = 0;
int received_sighup = 0;
char *configfile = CONFIG_FILE;
struct config cfg;
struct data data;
struct passwd *pwd;

struct child children[MAXCHILDREN];


void p_clean_exit();


void usage(){
   printf("\nusage: piler\n\n");
   printf("    -c <config file>                  Config file to use if not the default\n");
   printf("    -d                                Fork to the background\n");
   printf("    -v                                Return the version\n");
   printf("    -V                                Return the version and some build parameters\n");

   exit(0);
}


void takesig(int sig){
   int i, status;
   pid_t pid;

   switch(sig){
        case SIGHUP:
                initialise_configuration();
                if(read_key(&cfg)) fatal(ERR_READING_KEY);
                kill_children(SIGHUP);
                break;

        case SIGTERM:
                quit = 1;
                p_clean_exit();
                break;

        case SIGCHLD:
                while((pid = waitpid (-1, &status, WNOHANG)) > 0){

                   //syslog(LOG_PRIORITY, "child (pid: %d) has died", pid);

                   if(quit == 0){
                      i = search_slot_by_pid(pid);
                      if(i >= 0){
                         children[i].serial = i;
                         children[i].status = READY;
                         children[i].pid = child_make(&children[i]);
                      }
                      else syslog(LOG_PRIORITY, "error: couldn't find slot for pid %d", pid);

                   }
                }
                break;
   }

   return;
}


void child_sighup_handler(int sig){
   if(sig == SIGHUP){
      received_sighup = 1;
   }
}


int perform_checks(char *filename, struct session_data *sdata, struct data *data, struct parser_state *parser_state, struct config *cfg){

   if(cfg->security_header[0] && parser_state->found_security_header == 0){
      syslog(LOG_PRIORITY, "%s: discarding: missing security header", filename);
      return ERR_DISCARDED;
   }

   char *arule = check_against_ruleset(data->archiving_rules, parser_state, sdata->tot_len, sdata->spam_message);

   if(arule){
      syslog(LOG_PRIORITY, "%s: discarding: archiving policy: *%s*", filename, arule);
      return ERR_DISCARDED;
   }


   if(cfg->archive_only_mydomains == 1 && sdata->internal_sender == 0 && sdata->internal_recipient == 0){
      syslog(LOG_PRIORITY, "%s: discarding: not on mydomains", filename);
      return ERR_DISCARDED;
   }

   make_digests(sdata, cfg);

   // A normal header is much bigger than 10 bytes. We get here for header-only
   // messages without a Message-ID: line. I believe that no such message is valid, and
   // it's a reasonable to discard it, and not allowing it to fill up the error directory.

   if(sdata->hdr_len < 10){
      syslog(LOG_PRIORITY, "%s: discarding: a header-only message without a Message-ID line", filename);
      return ERR_DISCARDED;
   }

   int rc = process_message(sdata, parser_state, data, cfg);
   unlink(parser_state->message_id_hash);

   return rc;
}


int process_email(char *filename, struct session_data *sdata, struct data *data, int size, struct config *cfg){
   char tmpbuf[SMALLBUFSIZE];
   char *status=S_STATUS_UNDEF;
   char *p;
   struct timezone tz;
   struct timeval tv1, tv2;
   struct parser_state parser_state;
   struct counters counters;

   gettimeofday(&tv1, &tz);

   bzero(&counters, sizeof(counters));

#ifdef HAVE_ANTIVIRUS
   if(do_av_check(filename, cfg) == AVIR_VIRUS){
      unlink(filename);
      return OK;
   }
#endif

   init_session_data(sdata, cfg);

   sdata->tot_len = size;

   snprintf(sdata->filename, SMALLBUFSIZE-1, "%s", filename);

   parser_state = parse_message(sdata, 1, data, cfg);

   post_parse(sdata, &parser_state, cfg);

   if(cfg->syslog_recipients == 1){
      char *rcpt = parser_state.b_to;
      do {
         rcpt = split_str(rcpt, " ", tmpbuf, sizeof(tmpbuf)-1);

         if(does_it_seem_like_an_email_address(tmpbuf) == 1){
            syslog(LOG_PRIORITY, "%s: rcpt=%s", sdata->ttmpfile, tmpbuf);
         }
      } while(rcpt);
   }

   int rc = perform_checks(filename, sdata, data, &parser_state, cfg);
   unlink(sdata->tmpframe);

   remove_stripped_attachments(&parser_state);

   if(rc == OK){
      status = S_STATUS_STORED;
      counters.c_rcvd = 1;
      counters.c_size += sdata->tot_len;
      counters.c_stored_size = sdata->stored_len;
   }
   else if(rc == ERR_EXISTS){
      status = S_STATUS_DUPLICATE;
      counters.c_duplicate = 1;
      syslog(LOG_PRIORITY, "%s: discarding: duplicate message, id: %llu, message-id: %s", filename, sdata->duplicate_id, parser_state.message_id);
   }
   else if(rc == ERR_DISCARDED){
      status = S_STATUS_DISCARDED;
      counters.c_ignore = 1;
   }
   else {
      status = S_STATUS_ERROR;

      // move the file from piler/tmp/[0-xxx] dir to piler/error directory
      p = strchr(filename, '/');
      if(p)
         p++;
      else
         p = filename;

      snprintf(tmpbuf, sizeof(tmpbuf)-1, "%s/%s", ERROR_DIR, p);
      if(rename(filename, tmpbuf) == 0)
         syslog(LOG_PRIORITY, "%s: moved to %s", filename, tmpbuf);
      else
         syslog(LOG_PRIORITY, "%s: failed to moved to %s", filename, tmpbuf);
   }

   if(rc != ERR) unlink(filename);

   update_counters(sdata, data, &counters, cfg);

   gettimeofday(&tv2, &tz);

   syslog(LOG_PRIORITY, "%s: %s, size=%d/%d, attachments=%d, reference=%s, "
                        "message-id=%s, retention=%d, folder=%d, delay=%.4f, status=%s",
                             filename, sdata->ttmpfile, sdata->tot_len, sdata->stored_len,
                             parser_state.n_attachments, parser_state.reference, parser_state.message_id,
                             parser_state.retention, data->folder, tvdiff(tv2,tv1)/1000000.0, status);

   return rc;
}


int process_dir(char *directory, struct session_data *sdata, struct data *data, struct config *cfg){
   DIR *dir;
   struct dirent *de;
   int tot_msgs=0;
   char fname[SMALLBUFSIZE];
   struct stat st;

   dir = opendir(directory);
   if(!dir){
      syslog(LOG_PRIORITY, "cannot open directory: %s", directory);
      return tot_msgs;
   }

   while((de = readdir(dir))){
      if(strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) continue;

      snprintf(fname, sizeof(fname)-1, "%s/%s", directory, de->d_name);

      if(stat(fname, &st) == 0){
         if(S_ISREG(st.st_mode) && process_email(fname, sdata, data, st.st_size, cfg) != ERR){
            tot_msgs++;
         }
      }
      else {
         syslog(LOG_PRIORITY, "ERROR: cannot stat: %s", fname);
      }
   }

   closedir(dir);

   return tot_msgs;
}


void child_main(struct child *ptr){
   struct session_data sdata;
   char dir[TINYBUFSIZE];

   /* open directory, then process its files, then sleep 1 sec, and repeat */

   ptr->messages = 0;

   snprintf(dir, sizeof(dir)-1, "%d", ptr->serial);

   if(cfg.verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "child (pid: %d, serial: %d) started main() working on '%s'", getpid(), ptr->serial, dir);

   while(1){
      if(received_sighup == 1){
         if(cfg.verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "child (pid: %d) caught HUP signal", getpid());
         break;
      }

      sig_block(SIGHUP);

      int sphxopen = 0;
      if(cfg.rtindex && open_sphx(&sdata, &cfg) == OK){
         sphxopen = 1;
      }

      if((cfg.rtindex == 0 || sphxopen == 1) && open_database(&sdata, &cfg) == OK){
         ptr->messages += process_dir(dir, &sdata, &data, &cfg);
         close_database(&sdata);

         if(cfg.rtindex){
            close_sphx(&sdata);
         }

         sleep(1);
      }
      else {
         sleep(10);
      }

      sig_unblock(SIGHUP);

      // Let the child quit after processing max_requests_per_child messages

      if(cfg.max_requests_per_child > 0 && ptr->messages >= cfg.max_requests_per_child){
         if(cfg.verbosity >= _LOG_DEBUG)
            syslog(LOG_PRIORITY, "child (pid: %d, serial: %d) served enough: %d", getpid(), ptr->messages, ptr->serial);
         break;
      }
   }

#ifdef HAVE_MEMCACHED
   memcached_shutdown(&(data.memc));
#endif

   if(cfg.verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "child decides to exit (pid: %d)", getpid());

   exit(0);
}


pid_t child_make(struct child *ptr){
   pid_t pid;

   if((pid = fork()) > 0) return pid;

   if(pid == -1) return -1;

   if(cfg.verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "forked a child (pid: %d)", getpid());

   /* reset signals */

   set_signal_handler(SIGCHLD, SIG_DFL);
   set_signal_handler(SIGTERM, SIG_DFL);
   set_signal_handler(SIGHUP, child_sighup_handler);

   child_main(ptr);

   return -1;
}



int child_pool_create(){
   int i;

   for(i=0; i<MAXCHILDREN; i++){
      children[i].pid = 0;
      children[i].messages = 0;
      children[i].status = UNDEF;
      children[i].serial = -1;
   }

   for(i=0; i<cfg.number_of_worker_processes; i++){
      children[i].status = READY;
      children[i].serial = i;
      children[i].pid = child_make(&children[i]);

      if(children[i].pid == -1){
         syslog(LOG_PRIORITY, "error: failed to fork a child");
         p_clean_exit();
      }
   }

   return 0;
}


int search_slot_by_pid(pid_t pid){
   int i;

   for(i=0; i<MAXCHILDREN; i++){
      if(children[i].pid == pid) return i;
   }

   return -1;
}


void kill_children(int sig){
   int i;

   for(i=0; i<MAXCHILDREN; i++){
      if(children[i].status != UNDEF && children[i].pid > 1){
         if(cfg.verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "sending signal to child (pid: %d)", children[i].pid);
         kill(children[i].pid, sig);
      }
   }
}


void p_clean_exit(){
   kill_children(SIGTERM);

   clearrules(data.archiving_rules);
   clearrules(data.retention_rules);
   clearrules(data.folder_rules);

   clearhash(data.mydomains);

   syslog(LOG_PRIORITY, "%s has been terminated", PROGNAME);

   unlink(cfg.pidfile);

   if(data.dedup != MAP_FAILED) munmap(data.dedup, MAXCHILDREN*DIGEST_LENGTH*2);

   exit(1);
}


void fatal(char *s){
   syslog(LOG_PRIORITY, "%s\n", s);
   p_clean_exit();
}


void initialise_configuration(){
   struct session_data sdata;

   cfg = read_config(configfile);

   if(cfg.number_of_worker_processes < 2) cfg.number_of_worker_processes = 2;
   if(cfg.number_of_worker_processes > MAXCHILDREN) cfg.number_of_worker_processes = MAXCHILDREN;

   if(strlen(cfg.username) > 1){
      pwd = getpwnam(cfg.username);
      if(!pwd) fatal(ERR_NON_EXISTENT_USER);
   }


   if(chdir(cfg.workdir)){
      syslog(LOG_PRIORITY, "workdir: *%s*", cfg.workdir);
      fatal(ERR_CHDIR);
   }

   setlocale(LC_MESSAGES, cfg.locale);
   setlocale(LC_CTYPE, cfg.locale);


   clearrules(data.archiving_rules);
   clearrules(data.retention_rules);
   clearrules(data.folder_rules);

   clearhash(data.mydomains);

   data.folder = 0;
   data.recursive_folder_names = 0;

   inithash(data.mydomains);
   initrules(data.archiving_rules);
   initrules(data.retention_rules);
   initrules(data.folder_rules);

   if(open_database(&sdata, &cfg) == ERR){
      syslog(LOG_PRIORITY, "cannot connect to mysql server");
      return;
   }

   load_rules(&sdata, data.archiving_rules, SQL_ARCHIVING_RULE_TABLE);
   load_rules(&sdata, data.retention_rules, SQL_RETENTION_RULE_TABLE);
   load_rules(&sdata, data.folder_rules, SQL_FOLDER_RULE_TABLE);

   load_mydomains(&sdata, &data, &cfg);

   if(cfg.server_id > 0) insert_offset(&sdata, cfg.server_id);

   close_database(&sdata);


   syslog(LOG_PRIORITY, "reloaded config: %s", configfile);

#ifdef HAVE_MEMCACHED
   memcached_init(&(data.memc), cfg.memcached_servers, 11211);
#endif
}


int main(int argc, char **argv){
   int i, daemonise=0;
   struct stat st;


   while((i = getopt(argc, argv, "c:dvVh")) > 0){
      switch(i){

        case 'c' :
                   configfile = optarg;
                   break;

        case 'd' :
                   daemonise = 1;
                   break;

        case 'v' :
                   printf("%s\n", VERSION);
                   return 0;

        case 'V' :
                   printf("%s %s, Janos SUTO <sj@acts.hu>\n\n%s\nMySQL client library version: %s\n", PROGNAME, VERSION, CONFIGURE_PARAMS, mysql_get_client_info());
                   get_extractor_list();
                   return 0;

        case 'h' :
        default  :
                   usage();
      }
   }

   (void) openlog(PROGNAME, LOG_PID, LOG_MAIL);

   data.folder = 0;
   data.recursive_folder_names = 0;
   inithash(data.mydomains);
   initrules(data.archiving_rules);
   initrules(data.retention_rules);
   initrules(data.folder_rules);
   data.dedup = MAP_FAILED;
   data.import = NULL;

   initialise_configuration();

   set_signal_handler (SIGPIPE, SIG_IGN);


   if(read_key(&cfg)) fatal(ERR_READING_KEY);


   if(drop_privileges(pwd)) fatal(ERR_SETUID);

   check_and_create_directories(&cfg);

   if(stat(cfg.pidfile, &st) == 0) fatal(ERR_PID_FILE_EXISTS);

   if(cfg.mmap_dedup_test == 1){
      int dedupfd = open(MESSAGE_ID_DEDUP_FILE, O_RDWR);
      if(dedupfd == -1) fatal(ERR_OPEN_DEDUP_FILE);

      data.dedup = mmap(NULL, MAXCHILDREN*DIGEST_LENGTH*2, PROT_READ|PROT_WRITE, MAP_SHARED, dedupfd, 0);
      close(dedupfd);

      if(data.dedup == MAP_FAILED) syslog(LOG_INFO, "cannot mmap() %s, errno=%d", MESSAGE_ID_DEDUP_FILE, errno);
   }

   syslog(LOG_PRIORITY, "%s %s starting", PROGNAME, VERSION);

#if HAVE_DAEMON == 1
   if(daemonise == 1 && daemon(1, 0) == -1) fatal(ERR_DAEMON);
#endif

   write_pid_file(cfg.pidfile);

   child_pool_create();

   set_signal_handler(SIGCHLD, takesig);
   set_signal_handler(SIGTERM, takesig);
   set_signal_handler(SIGHUP, takesig);

   for(;;){ sleep(1); }

   p_clean_exit();

   return 0;
}
