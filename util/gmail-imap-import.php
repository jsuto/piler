<?php

$webuidir = "";

if(isset($_SERVER['argv'][1])) { $webuidir = $_SERVER['argv'][1]; }

require_once($webuidir . "/config.php");

ini_set("session.save_path", DIR_TMP);

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

$ug = new ModelUserGoogle();
$g = new ModelGoogleGoogle();



$query = $db->query("SELECT email FROM " . TABLE_GOOGLE);
if(isset($query->rows)) {
   foreach($query->rows as $q) {
      $access_token = $ug->refresh_access_token($q['email']);
      $g->download_users_emails($q['email'], $access_token);
   }
}

?>
