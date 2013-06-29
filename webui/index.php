<?php

function go_to_setup() {
   Header("Location: setup/setup.php");
   exit;
}

$stat = stat("config.php") or go_to_setup();
if($stat[7] < 15){ go_to_setup(); }

session_start();


require_once("config.php");

require(DIR_SYSTEM . "/startup.php");

$request = new Request();
Registry::set("request", $request);


Registry::set('document', new Document());


$start = NULL;


$loader = new Loader();
Registry::set('load', $loader);


$language = new Language();
Registry::set('language', $language);


if(ENABLE_SYSLOG == 1) { openlog("piler-webui", LOG_PID, LOG_MAIL); }


/* check if user has authenticated himself. If not, we send him to login */

Registry::set('username', getAuthenticatedUsername());
Registry::set('admin_user', isAdminUser());
Registry::set('auditor_user', isAuditorUser());
Registry::set('readonly_admin', isReadonlyAdmin());


$db = new DB(DB_DRIVER, DB_HOSTNAME, DB_USERNAME, DB_PASSWORD, DB_DATABASE, DB_PREFIX);
Registry::set('DB_DATABASE', DB_DATABASE);

Registry::set('db', $db);

Registry::set('DB_DRIVER', DB_DRIVER);

$sphx = new DB(SPHINX_DRIVER, SPHINX_HOSTNAME, "", "", SPHINX_DATABASE, "");
Registry::set('sphx', $sphx);


if(MEMCACHED_ENABLED) {
   $memcache = new Memcache();
   foreach ($memcached_servers as $m){
      $memcache->addServer($m[0], $m[1]);
   }

   Registry::set('memcache', $memcache);
}

Registry::set('counters', $counters);
Registry::set('langs', $langs);
Registry::set('themes', $themes);
Registry::set('letters', $letters);

Registry::set('health_smtp_servers', $health_smtp_servers);
Registry::set('partitions_to_monitor', $partitions_to_monitor);


if(Registry::get('username')) {

   if(isset($request->get['route'])){
      $action = new Router($request->get['route']);
   }
   else {
      $action = new Router('search/search');
   }
}
else {
   if(ENABLE_GOOGLE_LOGIN == 1 && isset($request->get['route']) && $request->get['route'] == 'login/google') {
      $action = new Router('login/google');
   } 
   else if(ENABLE_SSO_LOGIN == 1) {
      if(isset($request->get['route']) && $request->get['route'] == 'login/login') {
         $action = new Router('login/login');
      }
      else {
         header("Location: " . SITE_URL . 'sso.php');
         exit;
     }
   }
   else {
      $action = new Router('login/login');
   }
}


$controller = new Front();
$controller->dispatch($action, new Router('common/not_found'));


?>
