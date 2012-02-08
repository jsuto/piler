<?php

require_once("config.php");

require(DIR_SYSTEM . "/startup.php");

$now = time();
$data = "";

$ts1 = $now - ($now % 3600);
$ts2 = $ts1 + 3600;


$db = new DB(DB_DRIVER, DB_HOSTNAME, DB_USERNAME, DB_PASSWORD, DB_DATABASE, DB_PREFIX);

$query = $db->query("select vcode FROM " . TABLE_META . " WHERE sent >= ? AND sent < ? ORDER BY id ASC", array($ts1, $ts2));


print $query->query . " $ts1, $ts2\n\n";



foreach ($query->rows as $q) {
   $data .= $q['vcode'];
}

print $data . "\n";

$digest = openssl_digest($data, "SHA256");

print "$digest\n";


print fetch_url("http://ecrive.net/api/plain?mode=sign&data=$digest");

//print fetch_url("http://ecrive.net/api/plain?mode=check&data=$digest&signature=$s");
