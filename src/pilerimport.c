/*
 * pilerimport.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>
#include <syslog.h>
#include <piler.h>


extern char *optarg;
extern int optind;


int import_message(char *filename, struct session_data *sdata, struct __data *data, struct __config *cfg){
   int rc=ERR;
   char *rule;
   struct stat st;
   struct _state state;
   struct __counters counters;


   init_session_data(sdata);


   if(strcmp(filename, "-") == 0){

      if(read_from_stdin(sdata) == ERR){
         printf("error reading from stdin\n");
         return rc;
      }

      snprintf(sdata->filename, SMALLBUFSIZE-1, "%s", sdata->ttmpfile);

   }
   else {

      if(stat(filename, &st) != 0){
         printf("cannot read: %s\n", filename);
         return rc;
      }

      if(S_ISREG(st.st_mode) == 0){
         printf("%s is not a file\n", filename);
         return rc;
      }

      snprintf(sdata->filename, SMALLBUFSIZE-1, "%s", filename);

      sdata->tot_len = st.st_size;
   }


   
   sdata->sent = 0;

   state = parse_message(sdata, cfg);
   post_parse(sdata, &state, cfg);

   if(sdata->sent > sdata->now) sdata->sent = sdata->now;
   if(sdata->sent == -1) sdata->sent = 0;

   /* fat chances that you won't import emails before 1990.01.01 */

   if(sdata->sent > 631148400) sdata->retained = sdata->sent;

   rule = check_againt_ruleset(data->archiving_rules, &state, sdata->tot_len, sdata->spam_message);

   if(rule){
      printf("discarding %s by archiving policy: %s\n", filename, rule);
      rc = OK;
      goto ENDE;
   }

   make_digests(sdata, cfg);

   rc = process_message(sdata, &state, data, cfg);

ENDE:
   unlink(sdata->tmpframe);

   if(strcmp(filename, "-") == 0) unlink(sdata->ttmpfile);


   switch(rc) {
      case OK:
                        printf("imported: %s\n", filename);

                        bzero(&counters, sizeof(counters));
                        counters.c_size += sdata->tot_len;
                        update_counters(sdata, data, &counters, cfg);

                        break;

      case ERR_EXISTS:
                        printf("discarding duplicate message: %s\n", filename);
                        break;

      default:
                        printf("failed to import: %s (id: %s)\n", filename, sdata->ttmpfile);
                        break;
   } 

   return rc;
}


int import_from_mailbox(char *mailbox, struct session_data *sdata, struct __data *data, struct __config *cfg){
   FILE *F, *f=NULL;
   int rc=ERR, tot_msgs=0;
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
            rc = import_message(fname, sdata, data, cfg);
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
      rc = import_message(fname, sdata, data, cfg);
      unlink(fname);
   }

   fclose(F);


   return rc;
}


int import_from_maildir(char *directory, struct session_data *sdata, struct __data *data, struct __config *cfg){
   DIR *dir;
   struct dirent *de;
   int rc=ERR, tot_msgs=0;
   char fname[SMALLBUFSIZE];

   dir = opendir(directory);
   if(!dir){
      printf("cannot open directory: %s\n", directory);
      return rc;
   }

   while((de = readdir(dir))){
      if(strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) continue;

      snprintf(fname, sizeof(fname)-1, "%s/%s", directory, de->d_name);

      rc = import_message(fname, sdata, data, cfg);

      if(rc != ERR) tot_msgs++;

   }
   closedir(dir);

   return rc;
}


void usage(){
   printf("usage: pilerimport [-c <config file>] -e <eml file> | -m <mailbox file> | -d <directory>\n");
   exit(0);
}


int main(int argc, char **argv){
   int i, rc;
   char *configfile=CONFIG_FILE, *mailbox=NULL, *emlfile=NULL, *directory=NULL;
   struct session_data sdata;
   struct __config cfg;
   struct __data data;


   while((i = getopt(argc, argv, "c:m:e:d:h?")) > 0){
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

         case 'h' :
         case '?' :
                    usage();
                    break;


         default  : 
                    break;
       }
   }



   if(!mailbox && !emlfile && !directory) usage();


   cfg = read_config(configfile);

   if(read_key(&cfg)){
      printf("%s\n", ERR_READING_KEY);
      return 1;
   }

   mysql_init(&(sdata.mysql));
   mysql_options(&(sdata.mysql), MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&cfg.mysql_connect_timeout);
   if(mysql_real_connect(&(sdata.mysql), cfg.mysqlhost, cfg.mysqluser, cfg.mysqlpwd, cfg.mysqldb, cfg.mysqlport, cfg.mysqlsocket, 0) == 0){
      printf("cant connect to mysql server\n");
      return 0;
   }

   mysql_real_query(&(sdata.mysql), "SET NAMES utf8", strlen("SET NAMES utf8"));
   mysql_real_query(&(sdata.mysql), "SET CHARACTER SET utf8", strlen("SET CHARACTER SET utf8"));

   setlocale(LC_CTYPE, cfg.locale);

   data.archiving_rules = NULL;
   data.retention_rules = NULL;

   (void) openlog("pilerimport", LOG_PID, LOG_MAIL);

   load_rules(&sdata, &(data.archiving_rules), SQL_ARCHIVING_RULE_TABLE);
   load_rules(&sdata, &(data.retention_rules), SQL_RETENTION_RULE_TABLE);



   if(emlfile) rc = import_message(emlfile, &sdata, &data, &cfg);
   if(mailbox) rc = import_from_mailbox(mailbox, &sdata, &data, &cfg);
   if(directory) rc = import_from_maildir(directory, &sdata, &data, &cfg);



   free_rule(data.archiving_rules);
   free_rule(data.retention_rules);

   mysql_close(&(sdata.mysql));

   return 0;
}


