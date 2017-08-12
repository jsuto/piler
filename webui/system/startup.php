<?php

require(DIR_SYSTEM . "/controller.php");
require(DIR_SYSTEM . "/document.php");
require(DIR_SYSTEM . "/front.php");
require(DIR_SYSTEM . "/language.php");
require(DIR_SYSTEM . "/loader.php");
require(DIR_SYSTEM . "/model.php");
require(DIR_SYSTEM . "/router.php");

require(DIR_SYSTEM . "/misc.php");

require(DIR_SYSTEM . "/db.php");
require_once(DIR_SYSTEM . "/database/ldap.php");

if(ENABLE_GOOGLE_LOGIN == 1) {
   require_once DIR_BASE . 'google-api/apiClient.php';
   require_once DIR_BASE . 'google-api/contrib/apiOauth2Service.php';
}

if(TSA_URL) {
   require_once(DIR_SYSTEM . "/helper/TrustedTimestamps.php");
}


date_default_timezone_set(TIMEZONE);
