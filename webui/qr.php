<?php

session_start();


require_once("config.php");

require(DIR_SYSTEM . "/startup.php");

$session = new Session();
$loader = new Loader();
$language = new Language();

$db = new DB(DB_DRIVER, DB_HOSTNAME, DB_USERNAME, DB_PASSWORD, DB_DATABASE, DB_PREFIX);
Registry::set('db', $db);

$loader->model('user/prefs');
$loader->helper('phpqrcode/qrlib');
$loader->helper('PHPGangsta_GoogleAuthenticator');

$p = new ModelUserPrefs();

if(isset($_GET['refresh'])) {
   $GA = new PHPGangsta_GoogleAuthenticator();

   $new_secret = $GA->createSecret();

   $p->update_ga_secret($session->get('username'), $new_secret);

   print "$new_secret <a href=\"#\" onclick=\"Piler.new_qr(); return false;\">" . $language->data['text_refresh_qr_code'] . "</a><br /><img src=\"qr.php?ts=" . microtime(true) . "\" />\n";

   exit;
}
else if(isset($_GET['toggle'])) {
   $p->toggle_ga($session->get('username'), $_GET['toggle']);
}


$ga = $p->get_ga_settings($session->get('username'));

QRcode::png("otpauth://totp/" . SITE_NAME . "?secret=" . $ga['ga_secret'], false, "L", 8, 2);

?>
