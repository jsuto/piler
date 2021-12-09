<?php

define('COUNT', 'count');
define('HASH_VALUE', 'hash_value');
define('RESPONSE_STRING', 'response_string');
define('RESPONSE_TIME', 'response_time');
define('START_ID', 'start_id');
define('STOP_ID', 'stop_id');

ini_set("session.save_path", "/tmp");

$webuidir = "";
$verbose = 0;
$mode = "unit";
$algo = "sha256";

$opts = 'h::v';
$lopts = array(
    'webui:',
    'mode:',
    'algo:',
    'verbose'
    );

if ( $options = getopt( $opts, $lopts ) )
{
    if ( isset($options['webui']) )
    {
        $webuidir = $options['webui'];
    } else
    {
        echo "\nError: must provide path to WebUI directory\n\n";

        display_help();
        exit;
    }

    if ( isset($options['mode']) && $options['mode'] == 'time') {
       $mode = $options['mode'];
    }

    if ( isset($options['algo']) ) {
       $algo = $options['algo'];
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

openlog("piler-timestamp", LOG_PID, LOG_MAIL);

require(DIR_SYSTEM . "/startup.php");

$db = new DB(DB_DRIVER, DB_HOSTNAME, DB_USERNAME, DB_PASSWORD, DB_DATABASE, DB_PREFIX);
Registry::set('DB_DATABASE', DB_DATABASE);

Registry::set('db', $db);

Registry::set('DB_DRIVER', DB_DRIVER);

define('MODE', $mode);
define('ALGO', $algo);


$data = get_hash_values();

// check if we have enough messages

if(MODE == 'unit' && $data[COUNT] < TSA_STAMP_REQUEST_UNIT_SIZE) {
   syslog(LOG_INFO, "not enough messages to sign yet (" . $data[COUNT] . ")");
   exit;
}

if(MODE == 'time' && $data[COUNT] < 1) {
   syslog(LOG_INFO, "not enough messages to sign yet (" . $data[COUNT] . ")");
   exit;
}

try {
   $requestfile_path = TrustedTimestamps::createRequestfile($data[HASH_VALUE]);
   $response = TrustedTimestamps::signRequestfile($requestfile_path, TSA_URL);
} catch(Exception $e) {
   die("Error: " . $e->getMessage() . "\n");
}

$data[RESPONSE_STRING] = $response[RESPONSE_STRING];
$data[RESPONSE_TIME] = $response[RESPONSE_TIME];

$rc = store_results($data);
if($rc == 1) { syslog(LOG_INFO, "signed " . $data[COUNT] . " messages (" . $data[START_ID] . "-" . $data[STOP_ID] . ") on @" . $data[RESPONSE_TIME]); }
else { syslog(LOG_INFO, "ERROR: failed to sign " . $data[COUNT] . " messages (" . $data[START_ID] . "-" . $data[STOP_ID] . ") on @" . $data[RESPONSE_TIME]); }



function get_last_entry_from_timestamp_table() {
   $db = Registry::get('db');

   $query = $db->query("SELECT start_id, stop_id FROM " . TABLE_TIMESTAMP . " WHERE id < 100000000000 ORDER BY id DESC LIMIT 1");

   if($query->num_rows == 0) {
      return 0;
   }

   return $query->row[STOP_ID];
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

   $query = $db->query("SELECT id, digest FROM " . TABLE_META . " WHERE id >= ? AND id <= ? ORDER BY id", array($start_id, $stop_id));

   foreach($query->rows as $q) {
      $count++;
      $s .= $q['digest'];
   }

   if(MODE == 'time') { $stop_id = $start_id + $count - 1; }

   return [
      START_ID => $start_id,
      STOP_ID => $stop_id,
      COUNT => $count,
      HASH_VALUE => hash(ALGO, $s)
   ];

}


function store_results($data = array()) {
   $db = Registry::get('db');

   $db->query("INSERT INTO " . TABLE_TIMESTAMP . " (start_id, stop_id, hash_value, `count`, response_time, response_string) VALUES(?,?,?,?,?,?)", array($data[START_ID], $data[STOP_ID], $data[HASH_VALUE], $data[COUNT], $data[RESPONSE_TIME], $data[RESPONSE_STRING]));

   return $db->countAffected();
}


function display_help() {
    $phpself = basename(__FILE__);
    echo "\nUsage: $phpself --webui [PATH] [OPTIONS...]\n\n";
    echo "\t--webui=\"[REQUIRED: path to the Piler WebUI Directory]\"\n\n";
    echo "options:\n";
    echo "\t--mode time|unit (default: unit)\n";
    echo "\t-v Provide a verbose output\n";
    echo "\t-h Prints this help screen and exits\n";
}
