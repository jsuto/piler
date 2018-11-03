<?php

define('EMAIL_STR', 'email');

ini_set("session.save_path", "/tmp");

$webuidir = "";
$email = "";
$daemonize = 0;


$opts = 'hd::';
$lopts = array(
                'webui:',
                'email:'
              );


if($options = getopt($opts, $lopts)) {

   if(isset($options['webui'])) {
       $webuidir = $options['webui'];
   }
   else {
      echo "\nError: must provide path to WebUI directory\n\n";
      exit;
   }

   if(isset($options['d']))  {
      $daemonize = 1;
   }

   if(isset($options[EMAIL_STR])) {
      $email = $options[EMAIL_STR];
   }

}
else {
    display_help();
    exit;
}




require_once($webuidir . "/config.php");

require(DIR_SYSTEM . "/startup.php");

require_once 'Zend/Mail/Protocol/Imap.php';
require_once 'Zend/Mail/Storage/Imap.php';

$request = new Request();
Registry::set("request", $request);


Registry::set('document', new Document());


$loader = new Loader();
Registry::set('load', $loader);


$language = new Language();
Registry::set('language', $language);


$db = new DB(DB_DRIVER, DB_HOSTNAME, DB_USERNAME, DB_PASSWORD, DB_DATABASE, DB_PREFIX);
Registry::set('DB_DATABASE', DB_DATABASE);

Registry::set('db', $db);

Registry::set('DB_DRIVER', DB_DRIVER);


$loader->model('user/google');
$loader->model('google/google');

openlog("piler-gmail-import", LOG_PID, LOG_MAIL);

if($daemonize == 1) {
   while(1) {
      syslog(LOG_INFO, "started another imap poll");

      poll_imap_accounts();

      sleep(300);
   }
}
else {
   poll_imap_accounts($email);
}




function poll_imap_accounts($email = '') {
   $db = Registry::get('db');

   $ug = new ModelUserGoogle();
   $g = new ModelGoogleGoogle();


   if($email) {
      $query = $db->query("SELECT email FROM " . TABLE_GOOGLE . " WHERE email=?", array($email));
   } else {
      $query = $db->query("SELECT email FROM " . TABLE_GOOGLE);
   }

   if(isset($query->rows)) {
      foreach($query->rows as $q) {
         $access_token = $ug->refresh_access_token($q[EMAIL_STR]);
         $g->download_users_emails($q[EMAIL_STR], $access_token);
      }
   }
}



function display_help() {
    $phpself = basename(__FILE__);
    echo "\nUsage: $phpself --webui [PATH] [OPTIONS...]\n\n";
    echo "\t--webui=\"[REQUIRED: path to the Piler WebUI Directory]\"\n\n";
    echo "options:\n";
    echo "\t--email: Email address to poll. Leave it empty to poll all email addresses\n";
    echo "\t-d:      Daemonize the imap polling\n";
    echo "\t-h:      Prints this help screen and exits\n";
}
