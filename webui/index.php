<?php

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


openlog("piler-webui", LOG_PID, LOG_MAIL);


/* check if user has authenticated himself. If not, we send him to login */

Registry::set('username', getAuthenticatedUsername());
Registry::set('admin_user', isAdminUser());
Registry::set('auditor_user', isAuditorUser());
Registry::set('readonly_admin', isReadonlyAdmin());
Registry::set('data_officer', isDataOfficer());


$db = new DB(DB_DRIVER, DB_HOSTNAME, DB_USERNAME, DB_PASSWORD, DB_DATABASE, DB_PREFIX);
Registry::set('DB_DATABASE', DB_DATABASE);

Registry::set('db', $db);

Registry::set('DB_DRIVER', DB_DRIVER);

$sphx = new DB(SPHINX_DRIVER, SPHINX_HOSTNAME_READONLY, "", "", SPHINX_DATABASE, "");
$sphxrw = new DB(SPHINX_DRIVER, SPHINX_HOSTNAME, "", "", SPHINX_DATABASE, "");
Registry::set('sphx', $sphx);
Registry::set('sphxrw', $sphxrw);


if(MEMCACHED_ENABLED) {
   $memcache = new Memcached();
   $memcache->addServer($memcached_server[0], $memcached_server[1]);
   Registry::set('memcache', $memcache);
}

Registry::set('counters', $counters);
Registry::set('langs', $langs);
Registry::set('paging', $paging);
Registry::set('themes', $themes);
Registry::set('letters', $letters);
Registry::set('ldap_types', array("AD", "iredmail", "lotus", "zimbra", LDAP_TYPE_GENERIC));
Registry::set('health_smtp_servers', $health_smtp_servers);
Registry::set('partitions_to_monitor', $partitions_to_monitor);
Registry::set('actions', $actions);
Registry::set('import_status', $import_status);



if($session->get("ga_block") == 1 && $request->get['route'] != 'login/logout' ) {
   $action = new Router('login/ga');
}
else if($session->get("four_eyes") == 1 && $request->get['route'] != 'login/logout' ) {
   $action = new Router('login/foureyes');
}
else if(Registry::get('username')) {

   if(isset($request->get['route'])){
      $action = new Router($request->get['route']);
   }
   else {
      $action = new Router('search/search');
   }

   if(ENABLE_SAAS == 1) {
      $query = $db->query("UPDATE " . TABLE_ONLINE . " SET last_activity=? WHERE username=? AND ipaddr=?", array(NOW, $session->get('email'), getRemoteAddr()));

      if($db->countAffected() == 0) {
         $query = $db->query("INSERT INTO " . TABLE_ONLINE . " (username, ts, last_activity, ipaddr) VALUES(?,?,?,?)", array($session->get('email'), NOW, NOW, getRemoteAddr()));
      }
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
