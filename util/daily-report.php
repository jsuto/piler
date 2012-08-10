<?php

$webuidir = "";
$verbose = 0;

if(isset($_SERVER['argv'][1])) { $webuidir = $_SERVER['argv'][1]; }

for($i=2; $i<$_SERVER['argc']; $i++){
   if($_SERVER['argv'][$i] == "verbose") { $verbose = 1; }
}

require_once($webuidir . "/config.php");

require(DIR_SYSTEM . "/startup.php");

$loader = new Loader();
Registry::set('load', $loader);

$loader->load->model('user/user');
$loader->load->model('health/health');
$loader->load->model('stat/counter');
$loader->load->model('mail/mail');

$language = new Language();
Registry::set('language', $language);

extract($language->data);

Registry::set('admin_user', 1);


Registry::set('health_smtp_servers', $health_smtp_servers);
Registry::set('partitions_to_monitor', $partitions_to_monitor);


$db = new DB(DB_DRIVER, DB_HOSTNAME, DB_USERNAME, DB_PASSWORD, DB_DATABASE, DB_PREFIX);
Registry::set('DB_DATABASE', DB_DATABASE);

Registry::set('db', $db);

Registry::set('DB_DRIVER', DB_DRIVER);

$date = date(AUDIT_DATE_FORMAT, NOW);

$fp = fopen(LOCK_FILE, "r");
if(!$fp) { die("cannot open: " . LOCK_FILE . "\n"); }
if(!flock($fp, LOCK_EX | LOCK_NB)) { fclose($fp); die("cannot get a lock on " . LOCK_FILE . "\n"); }


$health = new ModelHealthHealth();
$counter = new ModelStatCounter();
$mail = new ModelMailMail();


      foreach (Registry::get('health_smtp_servers') as $smtp) {
         $_health[] = $health->checksmtp($smtp, $text_error);
      }

      $processed_emails = $health->count_processed_emails();

      list ($uptime, $cpuload) = $health->uptime();

      $cpuinfo = 100 - (int)file_get_contents(CPUSTAT);


      list($totalmem, $meminfo, $totalswap, $swapinfo) = $health->meminfo();
      $shortdiskinfo = $health->diskinfo();

      list($archive_size, $counters) = $counter->get_counters();

      $sysinfo = $health->sysinfo();

      $options = $health->get_options();


      $msg = "From: " . SMTP_FROMADDR . EOL;
      $msg .= "To: " . ADMIN_EMAIL . EOL;
      $msg .= "Subject: =?UTF-8?Q?" . preg_replace("/\n/", "", my_qp_encode($text_daily_piler_report)) . "?=" . EOL;
      $msg .= "MIME-Version: 1.0" . EOL;
      $msg .= "Content-Type: text/html; charset=\"utf-8\"" . EOL;
      $msg .= EOL . EOL;

      ob_start();

      include($webuidir . "/view/theme/default/templates/health/daily-report.tpl");

      $msg .= ob_get_contents();

      ob_end_clean();

      $rcpt = array(ADMIN_EMAIL);

      $x = $mail->send_smtp_email(SMARTHOST, SMARTHOST_PORT, SMTP_DOMAIN, SMTP_FROMADDR, $rcpt, $msg);



if($fp) {
   flock($fp, LOCK_UN);
   fclose($fp);
}


?>
