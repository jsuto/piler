<?php

$webuidir = "";

if(isset($_SERVER['argv'][1])) { $webuidir = $_SERVER['argv'][1]; }

require_once($webuidir . "/config.php");
require(DIR_SYSTEM . "/startup.php");
require(DIR_SYSTEM . "/ldap.php");

$trash_passwords = 0;


$cfg = read_konfig(LDAP_IMPORT_CONFIG_FILE);

foreach ($_SERVER['argv'] as $argv) {
   if($argv == "--trash-passwords") {
      $trash_passwords = 1;
   }

}


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

$loader->model('user/user');
$loader->model('user/import');

$import = new ModelUserImport();

$_SESSION['username'] = 'cli-admin';
$_SERVER['REMOTE_ADDR'] = '127.0.0.1';

$totalusers = 0;
$totalnewusers = 0;
$totaldeletedusers = 0;

extract($language->data);


foreach ($cfg as $ldap_params) {
   $users = $import->model_user_import->query_remote_users($ldap_params, $ldap_params['domain']);
   $rc = $import->model_user_import->fill_remote_table($ldap_params, $ldap_params['domain']);

   $totalusers += count($users);

   list($newusers, $deletedusers) = $import->model_user_import->process_users($users, $ldap_params);
   list($a1, $a2) = $import->model_user_import->process_users($users, $ldap_params);

   $totalnewusers += $newusers;
   $totaldeletedusers += $deletedusers;

   if($trash_passwords == 1) {
      $import->model_user_import->trash_password($users);
   }

}

$total_emails_in_database = $import->model_user_import->count_email_addresses();

print date(LOG_DATE_FORMAT); ?>, <?php print $totalusers; ?>/<?php print $totalnewusers; ?>/<?php print $totaldeletedusers; ?>/<?php print $total_emails_in_database; ?>
