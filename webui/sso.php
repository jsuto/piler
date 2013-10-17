<?php

session_start();


require_once("config.php");

require(DIR_SYSTEM . "/startup.php");

$session = new Session();
Registry::set("session", $session);

$request = new Request();
Registry::set("request", $request);


Registry::set('document', new Document());


$loader = new Loader();
Registry::set('load', $loader);


$language = new Language();
Registry::set('language', $language);

if(ENABLE_SYSLOG == 1) { openlog("piler-webui", LOG_PID, LOG_MAIL); }


$db = new DB(DB_DRIVER, DB_HOSTNAME, DB_USERNAME, DB_PASSWORD, DB_DATABASE, DB_PREFIX);
Registry::set('DB_DATABASE', DB_DATABASE);

Registry::set('db', $db);

Registry::set('DB_DRIVER', DB_DRIVER);

$action = new Router('login/sso');

$controller = new Front();
$controller->dispatch($action, new Router('common/not_found'));


?>
