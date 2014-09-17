<?php

require_once("config.php");
require(DIR_SYSTEM . "/language.php");

$language = new Language();

$javascript = DIR_BASE . '/view/javascript/piler-in.js';

/*
 * some funky User-Agent scripts:
 *
 *
 * - chrome: Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/37.0.2062.120 Safari/537.36
 *
 * - firefox: Mozilla/5.0 (Windows NT 6.3; WOW64; rv:32.0) Gecko/20100101 Firefox/32.0
 *
 * - IE 11: Mozilla/5.0 (Windows NT 6.3; WOW64; Trident/7.0; rv:11.0) like Gecko
 *
 */


if(OUTLOOK == 1 || strstr($_SERVER['HTTP_USER_AGENT'], " MSIE") || strstr($_SERVER['HTTP_USER_AGENT'], "Trident")) {
   define('BROWSER_WITH_NTLM_BUG', 1);
} else {
   define('BROWSER_WITH_NTLM_BUG', 0);
}

Header("Content-Type: text/javascript");
require_once($javascript);

?>
