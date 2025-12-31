/*
 * import_imap.c
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
#include <curl/curl.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>
#include <getopt.h>
#include <syslog.h>
#include <piler.h>

#define SSL_SETUP(curl, data)  \
   if(!strncmp(data->import->server, "imaps://", 8)) {    \
      curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);  \
      if(data->import->verifyssl == 0) {                \
         curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);    \
         curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);    \
      }                                   \
   }

void free_folder_list(struct FolderList *list){
   if(list){
      for(size_t i=0; i<list->count; i++){
         free(list->folders[i]);
      }

      if(list->folders && list->count){
         free(list->folders);
      }

      list->folders = NULL;
      list->count = 0;
   }
}

static size_t callback(void *contents, size_t size, size_t nmemb, void *userp){
   size_t realsize = size * nmemb;
   struct MemoryStruct *mem = (struct MemoryStruct *)userp;

   if(mem->size < mem->written_size + realsize + 1){
      int howmuch = realsize < BIGBUFSIZE ? BIGBUFSIZE : realsize;
      mem->memory = realloc(mem->memory, mem->size + howmuch + 1);
      if(mem->memory == NULL) {
         fprintf(stderr, "realloc() error: %s:%d\n", __func__, __LINE__);
         return 0;
      }
      mem->size += howmuch;
   }

   memcpy(&(mem->memory[mem->written_size]), contents, realsize);
   mem->written_size += realsize;
   mem->memory[mem->written_size] = 0;

   return realsize;
}


struct FolderList* list_folders_curl(CURL *curl, struct data *data){
   CURLcode res;
   struct MemoryStruct chunk;

   struct FolderList *result = malloc(sizeof(struct FolderList));
   if(!result){
      fprintf(stderr, "malloc() error: %s:%d\n", __func__, __LINE__);
      return NULL;
   }

   result->count = 0;

   chunk.memory = malloc(1);
   chunk.written_size = 0;
   chunk.size = 0;

   curl_easy_reset(curl);
   curl_easy_setopt(curl, CURLOPT_URL, data->import->server);
   curl_easy_setopt(curl, CURLOPT_USERNAME, data->import->username);
   curl_easy_setopt(curl, CURLOPT_PASSWORD, data->import->password);
   curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
   curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

   SSL_SETUP(curl, data);

   res = curl_easy_perform(curl);
   if(res != CURLE_OK){
      fprintf(stderr, "Curl perform failed: %s\n", curl_easy_strerror(res));
      goto cleanup3;
   }

   int rc;
   char buf[MAXBUFSIZE];
   char *p = chunk.memory;

   if(data->import->folder_imap){
      char tmpbuf[SMALLBUFSIZE];

      snprintf(tmpbuf, sizeof(tmpbuf)-1, "* LIST (\\HasNoChildren) \".\" \"%s\"\r\n", data->import->folder_imap);
      p = tmpbuf;
   }

   do {
      memset(buf, 0, sizeof(buf));
      p = split(p, '\n', buf, sizeof(buf)-1, &rc);

      if(*buf == '\0') continue;

      int len = strlen(buf);
      if(len < 5) continue;

      // Folder listings look like below
      // * LIST (\HasNoChildren) "." "fol der"
      // * LIST (\HasNoChildren) "." INBOX

      char *q = strchr(buf, '"');
      if(!q) continue;
      q = strchr(q+1, '"');
      if(!q) continue;

      if(result->count == 0){
         result->folders = malloc(sizeof(char*));
      } else {
         result->folders = realloc(result->folders, (result->count + 1) * sizeof(char*));
      }

      if(!result->folders){
         free(result);
         fprintf(stderr, "malloc() error: %s:%d\n", __func__, __LINE__);
         return NULL;
      }

      size_t name_len = strlen(q+2);
      char *folder_name = malloc(name_len + 3); // +3 for potential quotes and null terminator
      if(!folder_name){
         free_folder_list(result);
         fprintf(stderr, "malloc() error: %s:%d\n", __func__, __LINE__);
         return NULL;
      }

      snprintf(folder_name, name_len, "%s", q+2);
      result->folders[result->count++] = folder_name;
   } while(p);

cleanup3:
   free(chunk.memory);
   return result;
}


int examine_imap_folder(CURL *curl, struct data *data, const char *folder){
    CURLcode res;
    struct MemoryStruct chunk;
    long n_messages = 0;

    chunk.memory = malloc(1);
    chunk.written_size = 0;
    chunk.size = 0;

    curl_easy_reset(curl);
    curl_easy_setopt(curl, CURLOPT_URL, data->import->server);
    curl_easy_setopt(curl, CURLOPT_USERNAME, data->import->username);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, data->import->password);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    SSL_SETUP(curl, data);

    char command[SMALLBUFSIZE];
    snprintf(command, sizeof(command), "EXAMINE %s", folder);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, command);

    res = curl_easy_perform(curl);
    if(res != CURLE_OK){
       fprintf(stderr, "Curl perform failed: %s\n", curl_easy_strerror(res));
       goto cleanup2;
    }

    int result;
    char buf[MAXBUFSIZE];
    char *p = chunk.memory;

    do {
       memset(buf, 0, sizeof(buf));
       p = split(p, '\n', buf, sizeof(buf)-1, &result);

       if(*buf == '\0') continue;

       char *q = strcasestr(buf, " exists");
       if(q){
          *q = '\0';
          q = strrchr(buf, ' ');
          if(q) {
             q++;
             n_messages = atol(q);
          }
          break;
       }

    } while(p);

cleanup2:
   free(chunk.memory);
   return n_messages;
}

static CURLcode send_imap_command(CURL *curl, struct data *data, const char *url, struct MemoryStruct *chunk, const char *command){
   curl_easy_reset(curl);
   curl_easy_setopt(curl, CURLOPT_URL, url);
   curl_easy_setopt(curl, CURLOPT_USERNAME, data->import->username);
   curl_easy_setopt(curl, CURLOPT_PASSWORD, data->import->password);
   curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
   curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)chunk);
   curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, command);

   SSL_SETUP(curl, data);

   return curl_easy_perform(curl);
}

int delete_emails_from_imap(CURL *curl, struct data *data, const char *url, int *seq_nums, int count){
   CURLcode res;
   struct MemoryStruct chunk;
   char command[MAXBUFSIZE];
   int offset = 0;
   int i = 0;

   if(count <= 0) return 0;

   chunk.memory = malloc(1);
   chunk.written_size = 0;
   chunk.size = 0;

   // Build and send STORE commands in batches to avoid buffer overflow
   // Reserve space for "STORE " prefix (6) and " +FLAGS (\Deleted)" suffix (18) plus safety margin
   const int max_offset = sizeof(command) - 32;

   offset = snprintf(command, sizeof(command), "STORE ");

   for(i = 0; i < count; i++){
      int needed = snprintf(NULL, 0, "%s%d", (offset > 6) ? "," : "", seq_nums[i]);

      if(offset + needed >= max_offset){
         // Buffer full, send current batch
         snprintf(command + offset, sizeof(command) - offset, " +FLAGS (\\Deleted)");

         res = send_imap_command(curl, data, url, &chunk, command);
         if(res != CURLE_OK){
            fprintf(stderr, "Failed to mark messages as deleted: %s\n", curl_easy_strerror(res));
            free(chunk.memory);
            return -1;
         }

         // Start new batch
         offset = snprintf(command, sizeof(command), "STORE ");
      }

      // Add sequence number to current batch
      if(offset > 6) offset += snprintf(command + offset, sizeof(command) - offset, ",");
      offset += snprintf(command + offset, sizeof(command) - offset, "%d", seq_nums[i]);
   }

   // Send remaining batch
   if(offset > 6){
      snprintf(command + offset, sizeof(command) - offset, " +FLAGS (\\Deleted)");

      res = send_imap_command(curl, data, url, &chunk, command);
      if(res != CURLE_OK){
         fprintf(stderr, "Failed to mark messages as deleted: %s\n", curl_easy_strerror(res));
         free(chunk.memory);
         return -1;
      }
   }

   // Now expunge to permanently remove the deleted messages
   res = send_imap_command(curl, data, url, &chunk, "EXPUNGE");
   if(res != CURLE_OK){
      fprintf(stderr, "Failed to expunge deleted messages: %s\n", curl_easy_strerror(res));
      free(chunk.memory);
      return -1;
   }

   if(data->quiet == 0) printf("Deleted %d messages from server\n", count);

   free(chunk.memory);
   return 0;
}

int download_email(CURL *curl, struct data *data, const char *url, int seq_num, const char *output_path){
   char seq_url[SMALLBUFSIZE];
   CURLcode res;

   // Prepare URL for specific email
   snprintf(seq_url, sizeof(seq_url), "%s/;UID=%d", url, seq_num);

   struct MemoryStruct fetch_size, message;

   fetch_size.memory = malloc(1);
   fetch_size.written_size = 0;
   fetch_size.size = 0;

   message.memory = malloc(1);
   message.written_size = 0;
   message.size = 0;

   curl_easy_reset(curl);
   curl_easy_setopt(curl, CURLOPT_URL, seq_url);
   curl_easy_setopt(curl, CURLOPT_USERNAME, data->import->username);
   curl_easy_setopt(curl, CURLOPT_PASSWORD, data->import->password);
   curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
   curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&fetch_size);
   curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void *)&message);

   SSL_SETUP(curl, data);

   // Fetch command to get entire email body
   char command[SMALLBUFSIZE];
   snprintf(command, sizeof(command), "FETCH %d BODY[]", seq_num);
   curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, command);

   res = curl_easy_perform(curl);
   if(res != CURLE_OK){
      fprintf(stderr, "Curl perform failed: %s\n", curl_easy_strerror(res));
      goto cleanup;
   }

   char *body_start = memmem(message.memory, message.written_size, fetch_size.memory, fetch_size.written_size);
   if(body_start){
      body_start += fetch_size.written_size;

      // Parse message size from the 'fetch_size': * 1 FETCH (UID 1 BODY[] {19427}

      ssize_t message_size = 0;
      char *s = strrchr(fetch_size.memory, '{');
      if(s){
         s++;
         char *p = strchr(s, '}');
         if(!p) goto cleanup;

         *p = '\0';
         message_size = strtoull(s, NULL, 10);

         int fd = open(output_path, O_CREAT|O_EXCL|O_WRONLY, S_IRUSR|S_IWUSR|S_IRGRP);
         if(fd != -1){
            ssize_t n = write(fd, body_start, message_size);
            if(n != message_size) fprintf(stderr, "Failed to write %ld bytes\n", message_size);
            fsync(fd);
            close(fd);
         } else {
            fprintf(stderr, "Failed to open %s\n", output_path);
         }
      }
   } else {
      fprintf(stderr, "Cannot find %s in the message\n", fetch_size.memory);
   }

cleanup:
   free(message.memory);
   free(fetch_size.memory);

   return (res == CURLE_OK) ? 0 : -1;
}

int import_from_imap_server(struct session_data *sdata, struct data *data, struct counters *counters, struct config *cfg){
   CURL *curl;
   CURLcode global_init;

   global_init = curl_global_init(CURL_GLOBAL_DEFAULT);
   if(global_init != CURLE_OK){
      fprintf(stderr, "Curl global init failed\n");
      return 1;
   }

   curl = curl_easy_init();
   if(!curl){
      fprintf(stderr, "Curl init failed\n");
      curl_global_cleanup();
      return 1;
   }

   int check_skip_list = 0;
   if(data->import->skiplist && strlen(data->import->skiplist) > 0) check_skip_list = 1;

   struct FolderList *folders = list_folders_curl(curl, data);
   if(folders){
      uint64 total_counter = 0;

      for(size_t i=0; i<folders->count; i++){

         if(check_skip_list){
            char puf[SMALLBUFSIZE];
            snprintf(puf, sizeof(puf)-1, "%s,", folders->folders[i]);
            if(strstr(data->import->skiplist, puf)){
               if(data->quiet == 0) printf("SKIPPING FOLDER: %s\n", folders->folders[i]);
               continue;
            }
         }

         int n = examine_imap_folder(curl, data, folders->folders[i]);
         if(data->quiet == 0) printf("Folder %s has %d messages\n", folders->folders[i], n);
         if(n > 0){
            char url[SMALLBUFSIZE];
            char *q = folders->folders[i];
            if(*q == '"'){
               q++;
               *(q + strlen(q)-1) = '\0';
            }
            char *folder_name = url_encode(q);
            if(!folder_name) continue;
            snprintf(url, sizeof(url), "%s/%s", data->import->server, folder_name);
            free(folder_name);

            // Track downloaded message sequence numbers for deletion
            int *seq_nums_to_delete = NULL;
            int delete_count = 0;
            if(data->import->remove_after_import == 1){
               seq_nums_to_delete = malloc(n * sizeof(int));
               if(!seq_nums_to_delete){
                  fprintf(stderr, "Failed to allocate memory for deletion tracking\n");
               }
            }

            for(int j=1; j<=n; j++) {
               char output_path[SMALLBUFSIZE];
               total_counter++;
               snprintf(output_path, sizeof(output_path), "%llu.eml", total_counter);

               if(download_email(curl, data, url, j, output_path) == 0) {
                  // Track successfully downloaded message for deletion
                  if(seq_nums_to_delete){
                     seq_nums_to_delete[delete_count++] = j;
                  }
               } else {
                  fprintf(stderr, "Failed to download email %d from %s\n", j, folders->folders[i]);
               }

               // Import every downloaded 1000 messages
               if(total_counter % 1000 == 0){
                  import_from_maildir(sdata, data, ".", counters, cfg);
               }
            }

            // Import the rest
            import_from_maildir(sdata, data, ".", counters, cfg);

            // Delete successfully imported messages from server if requested
            if(data->import->remove_after_import == 1 && seq_nums_to_delete && delete_count > 0){
               if(delete_emails_from_imap(curl, data, url, seq_nums_to_delete, delete_count) != 0){
                  fprintf(stderr, "Failed to delete messages from %s\n", folders->folders[i]);
               }
            }

            if(seq_nums_to_delete){
               free(seq_nums_to_delete);
            }
         }
      }

      free_folder_list(folders);
      if(folders){
         free(folders);
      }

   } else {
      fprintf(stderr, "No imap folders found\n");
   }

   curl_easy_cleanup(curl);
   curl_global_cleanup();

   data->import->status = 2;

   return 0;
}
