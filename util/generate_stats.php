<?php
// init/set default values
$webuidir = '';
$process_all = false;
$start = NULL;
$end = NULL;
$timestart = microtime(true);

// get options from command line
$opts = 'h::';
$lopts = array(
    'webui:',
    'start::',
    'end::',
    );
    
if ( $options = getopt( $opts, $lopts ) )
{
    if ( isset($options['webui']) ) 
    {
        $webuidir = $options['webui'];
    } else
    {
        echo("\nError: must provide path to WebUI directory\n\n");  // todo: language
    
        display_help();
        exit;
    }
    
    if ( isset($options['h']) ) 
    {
        display_help();
        exit;
    } elseif ( isset($options['start']) )
    {
        $start = $options['start'];
    } elseif ( isset($options['end']) )
    {
        $end = $options['end'];
    }
} else {
    display_help();
    exit;   
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
$messagestats = new ModelAccountingAccounting();

$_SESSION['username'] = 'cli-admin';
$_SERVER['REMOTE_ADDR'] = '127.0.0.1';

extract($language->data);

//$day_to_process = strtotime($day_to_process);
$records = $messagestats->run_counters($start,$end);

$timeend = microtime(true);
$timegone = $timeend - $timestart;

echo("\nFinished Executing Statistics Generation\n");
echo("*************************************************\n");
echo("Start Date: ".date("d M Y",$records['starttimestamp'])."\n");
echo("Stop Date: ".date("d M Y",$records['endtimestamp'])."\n");
echo("Removed ".$records['deletedstats']." records\n");
echo("Added ".$records['addedstats']." records\n");
echo("Completed Run in ".$timegone." seconds\n\n");

# Functions
function display_help() {
    $phpself = basename(__FILE__);
    echo("\nUsage: $phpself --webui [PATH] [OPTIONS...]\n\n");
    echo("\t--webui=\"[REQUIRED: path to the Piler WebUI Directory]\"\n\n");
    echo("options:\n");
    echo("\t-a Reruns statistics for all records in the message view\n");
    echo("\t-h Prints this help screen and exits\n");
    echo("\t--start=\"Beginning of date range to process, ok values are today, yesterday or DDMMMYYYY...anything php's strtotime can process.  Optional, will default to beginning of current day.\"\n");
    echo("\t--stop=\"End of date range, same parameters as above.  Optional (will default to end of current day)\"\n\n");
}
?>