<?php

require_once("config.php");
require(DIR_SYSTEM . "/language.php");

$language = new Language();

$javascript = DIR_BASE . '/view/javascript/piler-in.js';

openlog("piler-webui", LOG_PID, LOG_MAIL);

syslog(LOG_INFO, "lang:" . LANG);


Header("Content-Type: text/javascript");

require_once($javascript);

?>
