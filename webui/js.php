<?php

require_once("config.php");
require(DIR_SYSTEM . "/language.php");

$language = new Language();

$javascript = DIR_BASE . '/view/javascript/piler-in.js';

Header("Content-Type: text/javascript");
require_once($javascript);

?>
