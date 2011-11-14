/*
 * memcached.c, SJ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <syslog.h>
#include <piler.h>


int getUserdataFromMemcached(struct session_data *sdata, struct __data *data, char *email, struct __config *cfg){
   unsigned int len=0;
   uint32_t flags = 0;
   char key[SMALLBUFSIZE], *s=NULL, *p;

   //if(data->memc.initialised == 0) return 0;

   snprintf(key, SMALLBUFSIZE-1, "%s:%s", MEMCACHED_CLAPF_PREFIX, email);

   s = memcached_get(&(data->memc), key, &len, &flags);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: memcached user query=%s, data=%s (%d)", sdata->ttmpfile, key, s, len);

   if(len > 0){
      /* 1000:8:sj:acts.hu:1 */

      if(len == 1 && s[0] == 'U'){
         if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: %s is unknown", sdata->ttmpfile, email);
         return 1;
      }

      p = strchr(s, ':');
      if(p){ *p = '\0'; sdata->uid = atol(s); s = p+1; }

      p = strchr(s, ':');
      if(p){ *p = '\0'; sdata->gid = atol(s); s = p+1; }

      p = strchr(s, ':');
      if(p){ *p = '\0'; snprintf(sdata->name, SMALLBUFSIZE-1, "%s", s); s = p+1; }

      p = strchr(s, ':');
      if(p){ *p = '\0'; snprintf(sdata->domain, SMALLBUFSIZE-1, "%s", s); s = p+1; }

      sdata->policy_group = atoi(s);

      if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: memcached parsed user data: uid: %ld, gid: %ld, name: %s, domain: %s, policy group: %d", sdata->ttmpfile, sdata->uid, sdata->gid, sdata->name, sdata->domain, sdata->policy_group);

      return 1;
   }

   return 0;
}


int putUserdataToMemcached(struct session_data *sdata, struct __data *data, char *email, struct __config *cfg){
   uint32_t flags = 0;
   char key[SMALLBUFSIZE], value[SMALLBUFSIZE];

   snprintf(key, SMALLBUFSIZE-1, "%s:%s", MEMCACHED_CLAPF_PREFIX, email);
   if(sdata->uid == 0)
      strcpy(value, "U");
   else
      snprintf(value, SMALLBUFSIZE-1, "%ld:%ld:%s:%s:%d", sdata->uid, sdata->gid, sdata->name, sdata->domain, sdata->policy_group);

   if(memcached_add(&(data->memc), key, strlen(key), value, strlen(value), cfg->memcached_ttl, flags) == MEMCACHED_SUCCESS) return 1;

   return 0;
}


int getPolicyFromMemcached(struct session_data *sdata, struct __data *data, struct __config *cfg, struct __config *my_cfg){
   unsigned int len=0;
   uint32_t flags = 0;
   char key[SMALLBUFSIZE], *s=NULL, *p;

   if(sdata->policy_group <= 0) return 0;

   snprintf(key, SMALLBUFSIZE-1, "%s:%d", MEMCACHED_CLAPF_PREFIX, sdata->policy_group);

   s = memcached_get(&(data->memc), key, &len, &flags);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: memcached policy query=%s, data=%s (%d)", sdata->ttmpfile, key, s, len);

   if(len > 0){

      p = strchr(s, ':'); if(p){ *p = '\0'; my_cfg->deliver_infected_email = atoi(s); s = p+1; }
      p = strchr(s, ':'); if(p){ *p = '\0'; my_cfg->silently_discard_infected_email = atoi(s); s = p+1; }
      p = strchr(s, ':'); if(p){ *p = '\0'; my_cfg->use_antispam = atoi(s); s = p+1; }
      p = strchr(s, ':'); if(p){ *p = '\0'; snprintf(my_cfg->spam_subject_prefix, MAXVAL-1, "%s", s); s = p+1; }
      p = strchr(s, ':'); if(p){ *p = '\0'; my_cfg->enable_auto_white_list = atoi(s); s = p+1; }
      p = strchr(s, ':'); if(p){ *p = '\0'; my_cfg->max_message_size_to_filter = atoi(s); s = p+1; }
      p = strchr(s, ':'); if(p){ *p = '\0'; snprintf(my_cfg->rbl_domain, MAXVAL-1, "%s", s); s = p+1; }
      p = strchr(s, ':'); if(p){ *p = '\0'; snprintf(my_cfg->surbl_domain, MAXVAL-1, "%s", s); s = p+1; }
      p = strchr(s, ':'); if(p){ *p = '\0'; my_cfg->spam_overall_limit = atof(s); s = p+1; }
      p = strchr(s, ':'); if(p){ *p = '\0'; my_cfg->spaminess_oblivion_limit = atof(s); s = p+1; }
      p = strchr(s, ':'); if(p){ *p = '\0'; my_cfg->replace_junk_characters = atoi(s); s = p+1; }
      p = strchr(s, ':'); if(p){ *p = '\0'; my_cfg->invalid_junk_limit = atoi(s); s = p+1; }
      p = strchr(s, ':'); if(p){ *p = '\0'; my_cfg->invalid_junk_line = atoi(s); s = p+1; }
      p = strchr(s, ':'); if(p){ *p = '\0'; my_cfg->penalize_images = atoi(s); s = p+1; }
      p = strchr(s, ':'); if(p){ *p = '\0'; my_cfg->penalize_embed_images = atoi(s); s = p+1; }
      p = strchr(s, ':'); if(p){ *p = '\0'; my_cfg->penalize_octet_stream = atoi(s); s = p+1; }
      p = strchr(s, ':'); if(p){ *p = '\0'; my_cfg->training_mode = atoi(s); s = p+1; }
      p = strchr(s, ':'); if(p){ *p = '\0'; my_cfg->initial_1000_learning = atoi(s); s = p+1; }
      p = strchr(s, ':'); if(p){ *p = '\0'; my_cfg->store_metadata = atoi(s); s = p+1; }
      p = strchr(s, ':'); if(p){ *p = '\0'; my_cfg->store_only_spam = atoi(s); s = p+1; }
      p = strchr(s, ':'); if(p){ *p = '\0'; my_cfg->message_from_a_zombie = atoi(s); }

      if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: memcached parsed policy data: spam limit: %.4f, oblivion: %.4f, subject prefix: *%s*, rbl: *%s*, training mode: %d, meta: %d",
                         sdata->ttmpfile, my_cfg->spam_overall_limit, my_cfg->spaminess_oblivion_limit, my_cfg->spam_subject_prefix, my_cfg->rbl_domain, my_cfg->training_mode, my_cfg->store_metadata);

      return 1;
   }

   return 0;
}


int putPolicyToMemcached(struct session_data *sdata, struct __data *data, struct __config *my_cfg){
   uint32_t flags = 0;
   char key[SMALLBUFSIZE], value[SMALLBUFSIZE];

   if(sdata->policy_group <= 0) return 0;

   snprintf(key, SMALLBUFSIZE-1, "%s:%d", MEMCACHED_CLAPF_PREFIX, sdata->policy_group);

   snprintf(value, SMALLBUFSIZE-1, "%d:%d:%d:%s:%d:%ld:%s:%s:%.4f:%.4f:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
                   my_cfg->deliver_infected_email,
                   my_cfg->silently_discard_infected_email,
                   my_cfg->use_antispam,
                   my_cfg->spam_subject_prefix,
                   my_cfg->enable_auto_white_list,
                   my_cfg->max_message_size_to_filter,
                   my_cfg->rbl_domain,
                   my_cfg->surbl_domain,
                   my_cfg->spam_overall_limit,
                   my_cfg->spaminess_oblivion_limit,
                   my_cfg->replace_junk_characters,
                   my_cfg->invalid_junk_limit,
                   my_cfg->invalid_junk_line,
                   my_cfg->penalize_images,
                   my_cfg->penalize_embed_images,
                   my_cfg->penalize_octet_stream,
                   my_cfg->training_mode,
                   my_cfg->initial_1000_learning,
                   my_cfg->store_metadata,
                   my_cfg->store_only_spam,
                   my_cfg->message_from_a_zombie
                  );


   if(memcached_add(&(data->memc), key, strlen(key), value, strlen(value), my_cfg->memcached_ttl, flags) == MEMCACHED_SUCCESS) return 1;

   return 0;
}


int getWBLFromMemcached(struct session_data *sdata, struct __data *data, struct __config *cfg){
   unsigned int len=0;
   uint32_t flags = 0;
   char key[SMALLBUFSIZE], *s=NULL, *p;

   snprintf(key, SMALLBUFSIZE-1, "%s:wbl%ld", MEMCACHED_CLAPF_PREFIX, sdata->uid);

   s = memcached_get(&(data->memc), key, &len, &flags);

   if(cfg->verbosity >= _LOG_DEBUG) syslog(LOG_PRIORITY, "%s: memcached wbl query=%s, data=%s (%d)", sdata->ttmpfile, key, s, len);

   if(len > 0){
      /* whiteemail1,whiteemail2:blackemail1,blackemail2 */

      p = strchr(s, ':');
      if(p){
         *p = '\0';
         snprintf(sdata->whitelist, MAXBUFSIZE-1, "%s", s);
         snprintf(sdata->blacklist, MAXBUFSIZE-1, "%s", p+1);
      }

      return 1;
   }

   return 0;
}


int putWBLToMemcached(struct session_data *sdata, struct __data *data, struct __config *cfg){
   uint32_t flags = 0;
   char key[SMALLBUFSIZE], value[2*MAXBUFSIZE];

   if(sdata->uid <= 0) return 0;

   snprintf(key, SMALLBUFSIZE-1, "%s:wbl%ld", MEMCACHED_CLAPF_PREFIX, sdata->uid);
   snprintf(value, 2*MAXBUFSIZE-1, "%s:%s", sdata->whitelist, sdata->blacklist);

   if(memcached_add(&(data->memc), key, strlen(key), value, strlen(value), cfg->memcached_ttl, flags) == MEMCACHED_SUCCESS) return 1;

   return 0;
}


