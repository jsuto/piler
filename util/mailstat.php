<?php

define(EMAIL, 'email');

$webuidir = "/var/piler/www";
$date1 = $date2 = 0;
$date_condition = '';
$email = '';
$page_len = 20;
$data = array();

$opts = 'w:e:a:b:h';
$lopts = array(
                'webui:',
                'email:',
                'start:',
                'stop:',
                'help'
              );

$options = getopt($opts, $lopts);

if(isset($options['help']) || isset($options['h']) )
{
   display_help();
   exit;
}


if(isset($options['webui']))
{
   $webuidir = $options['webui'];
}

if(isset($options[EMAIL]))
{
   $email = $options[EMAIL];
} else {
   echo "\nError: must provide the email address\n\n";
   display_help();
   exit;
}

if(isset($options['start']))
{
   list($y,$m,$d) = preg_split("/(\.|\-|\/)/", $options['start']);
   $date1 = mktime(0, 0, 0, $m, $d, $y);
}

if(isset($options['stop']))
{
   list($y,$m,$d) = preg_split("/(\.|\-|\/)/", $options['stop']);
   $date2 = mktime(23, 59, 59, $m, $d, $y);
}


ini_set("session.save_path", "/tmp");

require_once($webuidir . "/config.php");

require(DIR_SYSTEM . "/startup.php");

$loader = new Loader();
Registry::set('load', $loader);

$language = new Language();
Registry::set('language', $language);

extract($language->data);

$db = new DB(DB_DRIVER, DB_HOSTNAME, DB_USERNAME, DB_PASSWORD, DB_DATABASE, DB_PREFIX);
Registry::set('DB_DATABASE', DB_DATABASE);

Registry::set('db', $db);

if($date1 > 0) { $date_condition = " AND sent >= $date1 "; }
if($date2 > 0) { $date_condition .= " AND sent <= $date2 "; }


$query = $db->query("select `from` AS email, count(`from`) AS num FROM " . VIEW_MESSAGES . " WHERE `to`=? $date_condition GROUP BY `from` ORDER BY num DESC LIMIT $page_len", array($email));

foreach($query->rows as $q) {
   $data[] = array(
                   'num'     => $q['num'],
                   EMAIL   => $q[EMAIL],
                   'type'    => 'rcvd'
                  );

}


$query = $db->query("select `to` AS email, count(`to`) AS num FROM " . VIEW_MESSAGES . " WHERE `from`=? $date_condition GROUP BY `to` ORDER BY num DESC LIMIT $page_len", array($email));

foreach($query->rows as $q) {
   $data[] = array(
                   'num'   => $q['num'],
                   EMAIL => $q[EMAIL],
                   'type'  => 'sent'
                  );
}


array_multisort($data, SORT_DESC);

foreach ($data as $q) {
   print $q['num'] . "\t" . $q['type'] . "\t" . $q[EMAIL] . "\n";
}

function display_help() {
   global $webuidir;

   $phpself = basename(__FILE__);
   echo "\nUsage: $phpself --webui [PATH] [OPTIONS...]\n\n";
   echo "\t--webui=\"[REQUIRED: path to the Piler WebUI Directory, default: $webuidir]\"\n";
   echo "\t--email=email address to look for conversations\n";

   echo "\noptions:\n";
   echo "\t-h Prints this help screen and exits\n";
   echo "\t--start=\"Beginning of date range to process, ok values are today, yesterday or DDMMMYYYY...anything php's strtotime can process.  Optional, will default to beginning of current day.\"\n";
   echo "\t--stop=\"End of date range, same parameters as above.  Optional (will default to end of current day)\"\n\n";
}
