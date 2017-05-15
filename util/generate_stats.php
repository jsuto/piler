<?php

$webuidir = '';
$process_all = false;
$start = NULL;
$stop = NULL;
$timestart = microtime(true);

ini_set("session.save_path", "/tmp");

$opts = 'h::';
$lopts = array(
               'start:',
               'stop:',
               'webui:',
              );
    
if($options = getopt($opts, $lopts)) {

   if(isset($options['h'])) {
      display_help();
   }

   if(isset($options['start'])) {
      $start = $options['start'];
   }

   if(isset($options['stop'])) {
      $stop = $options['stop'];
   }

   if(isset($options['webui'])) {
      $webuidir = $options['webui'];
   }

} else {
    display_help();
}


if($webuidir == '') {
   echo("\nError: must provide path to WebUI directory\n\n");
   display_help();
}


require_once($webuidir . "/config.php");
require(DIR_SYSTEM . "/startup.php");
date_default_timezone_set(TIMEZONE);

$loader = new Loader();

$language = new Language();
Registry::set('language', $language);

if(MEMCACHED_ENABLED) {
   $memcache = new Memcache();
   foreach ($memcached_servers as $m){
      $memcache->addServer($m[0], $m[1]);
   }

   Registry::set('memcache', $memcache);
}

$db = new DB(DB_DRIVER, DB_HOSTNAME, DB_USERNAME, DB_PASSWORD, DB_DATABASE, DB_PREFIX);
Registry::set('db', $db);

$loader->model('accounting/accounting');
$loader->model('domain/domain');
$messagestats = new ModelAccountingAccounting();

$_SESSION['username'] = 'cli-admin';
$_SERVER['REMOTE_ADDR'] = '127.0.0.1';

extract($language->data);

$records = $messagestats->run_counters($start, $stop, 'sent');

$timeend = microtime(true);
$timegone = $timeend - $timestart;

echo("\nFinished Executing Statistics Generation\n");
echo("*************************************************\n");
echo("Start Date: ".date(DATE_TEMPLATE,$records['starttimestamp'])."\n");
echo("Stop Date: ".date(DATE_TEMPLATE,$records['stoptimestamp'])."\n");
echo("Removed ".$records['deletedstats']." records\n");
echo("Added ".$records['addedstats']." records\n");
echo("Completed Run in ".$timegone." seconds\n\n");


function display_help() {
   $phpself = basename(__FILE__);
   echo("\nUsage: $phpself --webui [PATH] [OPTIONS...]\n\n");
   echo("\t--webui=\"[REQUIRED: path to the Piler WebUI Directory]\"\n\n");
   echo("options:\n");
   echo("\t-a Reruns statistics for all records in the message view\n");
   echo("\t-h Prints this help screen and exits\n");
   echo("\t--start=\"Beginning of date range to process, ok values are today, yesterday or MYYYY/MM/DD...anything php's strtotime can process.  Optional, will default to beginning of current day.\"\n");
   echo("\t--stop=\"End of date range, same parameters as above.  Optional (will default to end of current day)\"\n\n");

   exit;
}
