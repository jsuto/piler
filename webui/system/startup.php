<?php

require(DIR_SYSTEM . "/controller.php");
require(DIR_SYSTEM . "/document.php");
require(DIR_SYSTEM . "/front.php");
require(DIR_SYSTEM . "/language.php");
require(DIR_SYSTEM . "/loader.php");
require(DIR_SYSTEM . "/model.php");
require(DIR_SYSTEM . "/registry.php");
require(DIR_SYSTEM . "/request.php");
require(DIR_SYSTEM . "/router.php");

require(DIR_SYSTEM . "/misc.php");

require(DIR_SYSTEM . "/db.php");
require_once(DIR_SYSTEM . "/database/ldap.php");

date_default_timezone_set(TIMEZONE);

?>
