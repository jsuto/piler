<?php

ini_set("session.save_path", "/tmp");

$webuidir = "";
$verbose = 0;
$mode = "unit";

$opts = 'h::v';
$lopts = array(
    'webui:',
    'mode:',
    'verbose'
    );
    
if ( $options = getopt( $opts, $lopts ) )
{
    if ( isset($options['webui']) ) 
    {
        $webuidir = $options['webui'];
    } else
    {
        echo("\nError: must provide path to WebUI directory\n\n");
    
        display_help();
        exit;
    }
   
    if ( isset($options['mode']) )
    {
       if($options['mode'] == 'time') { $mode = $options['mode']; }
    }
 
    if ( isset($options['h']) ) 
    {
        display_help();
        exit;
    }
    if ( isset($options['verbose']) )
    {
        $verbose = 1;
    }
} else {
    display_help();
    exit;   
}

require_once($webuidir . "/config.php");

if(ENABLE_SYSLOG == 1) { openlog("piler-timestamp", LOG_PID, LOG_MAIL); }

require(DIR_SYSTEM . "/startup.php");

$db = new DB(DB_DRIVER, DB_HOSTNAME, DB_USERNAME, DB_PASSWORD, DB_DATABASE, DB_PREFIX);
Registry::set('DB_DATABASE', DB_DATABASE);

Registry::set('db', $db);

Registry::set('DB_DRIVER', DB_DRIVER);

define('MODE', $mode);


$data = get_hash_values();

//print_r($data);

// check if we have enough messages

if(MODE == 'unit' && $data['count'] < TSA_STAMP_REQUEST_UNIT_SIZE) {
   syslog(LOG_INFO, "not enough messages to sign yet (" . $data['count'] . ")");
   exit;
}

if(MODE == 'time' && $data['count'] < 1) {
   syslog(LOG_INFO, "not enough messages to sign yet (" . $data['count'] . ")");
   exit;
}

$requestfile_path = TrustedTimestamps::createRequestfile($data['hash_value']);

$response = TrustedTimestamps::signRequestfile($requestfile_path, TSA_URL);

$data['response_string'] = $response['response_string'];
$data['response_time'] = $response['response_time'];

$rc = store_results($data);
if($rc == 1) { syslog(LOG_INFO, "signed " . $data['count'] . " messages (" . $data['start_id'] . "-" . $data['stop_id'] . ") on @" . $data['response_time']); }
else { syslog(LOG_INFO, "ERROR: failed to sign " . $data['count'] . " messages (" . $data['start_id'] . "-" . $data['stop_id'] . ") on @" . $data['response_time']); }



function get_last_entry_from_timestamp_table() {
   $db = Registry::get('db');

   $query = $db->query("SELECT start_id, stop_id FROM " . TABLE_TIMESTAMP . " WHERE id < 100000000000 ORDER BY id DESC LIMIT 1");

   if($query->num_rows == 0) {
      return 0;
   }

   return $query->row['stop_id'];
}


function get_hash_values() {
   $s = '';
   $count = 0;

   $db = Registry::get('db');

   $last_id = get_last_entry_from_timestamp_table();

   if($last_id == 0) {
      $start_id = TSA_START_ID;
      if(MODE == 'unit') { $stop_id = $start_id + TSA_STAMP_REQUEST_UNIT_SIZE - 1; }
      else { $stop_id = 1000000000; }
   }
   else {
      $start_id = $last_id + 1;
      if(MODE == 'unit') { $stop_id = $start_id + TSA_STAMP_REQUEST_UNIT_SIZE - 1; }
      else { $stop_id = 1000000000; }
   }

   $query = $db->query("SELECT id, digest FROM " . TABLE_META . " WHERE id >= ? AND id <= ?", array($start_id, $stop_id));

   foreach($query->rows as $q) {
      $count++;
      $s .= $q['digest'];
   }

   if(MODE == 'time') { $stop_id = $start_id + $count - 1; }

   return array(
                'start_id' => $start_id,
                'stop_id' => $stop_id,
                'count' => $count,
                'hash_value' => sha1($s)
          );

}


function store_results($data = array()) {
   $db = Registry::get('db');

   $query = $db->query("INSERT INTO " . TABLE_TIMESTAMP . " (start_id, stop_id, hash_value, `count`, response_time, response_string) VALUES(?,?,?,?,?,?)", array($data['start_id'], $data['stop_id'], $data['hash_value'], $data['count'], $data['response_time'], $data['response_string']));

   $rc = $db->countAffected();

   return $rc;
}


function display_help() {
    $phpself = basename(__FILE__);
    echo("\nUsage: $phpself --webui [PATH] [OPTIONS...]\n\n");
    echo("\t--webui=\"[REQUIRED: path to the Piler WebUI Directory]\"\n\n");
    echo("options:\n");
    echo("\t--mode time|unit (default: unit)\n");
    echo("\t-v Provide a verbose output\n");
    echo("\t-h Prints this help screen and exits\n");
}


?>
