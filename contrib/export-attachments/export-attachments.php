#!/usr/bin/php
<?php

define('EXPORT_LOCK_FILE', '/var/piler/tmp/export-attachments.lock');

$webuidir = "";

$outdir = '';


$opts = 'hw:d:';
$lopts = [
   'webui:',
   'dir:',
   'help'
];


if($options = getopt($opts, $lopts)) {
   if(isset($options['webui'])) { $webuidir = $options['webui']; }
   if(isset($options['dir'])) { $outdir = $options['dir']; }
   if(isset($options['help'])) { usage(); }
}


if($webuidir == '' || $outdir == '') { usage(); }



require_once("$webuidir/config.php");

require(DIR_SYSTEM . "/startup.php");

$request = new Request();
Registry::set("request", $request);


$start = NULL;

$loader = new Loader();
Registry::set('load', $loader);

$loader->load->model('domain/domain');
$loader->load->model('search/search');
$loader->load->model('search/message');
$loader->load->model('message/attachment');


$db = new DB(DB_DRIVER, DB_HOSTNAME, DB_USERNAME, DB_PASSWORD, DB_DATABASE, DB_PREFIX);
Registry::set('db', $db);

Registry::set('auditor_user', 1);

openlog("export-attachments", LOG_PID, LOG_MAIL);

$fp = fopen(EXPORT_LOCK_FILE, "w");
if(!flock($fp, LOCK_EX)) {
   syslog(LOG_INFO, "WARN: couldn't get a lock on " . EXPORT_LOCK_FILE);
   exit;
}


$domain = new ModelDomainDomain();
$attachment = new ModelMessageAttachment();
$message = new ModelSearchMessage();

$domains = $domain->get_mapped_domains();

$last_id = $attachment->get_last_attachment_id();
$start_id = $attachment->get_checkpoint();

syslog(LOG_INFO, "start: $start_id, limit: $last_id");


for($i=$start_id; $i<$last_id; $i++) {
   $a = $attachment->get_attachment_by_id($i);
   $m = $message->get_message_addresses_by_piler_id($a['piler_id'], $domains);

   $attachment->dump_attachment($outdir, "out", $m['sender'], $i, $a);

   foreach($m['rcpt'] as $rcpt) {
      $attachment->dump_attachment($outdir, "in", $rcpt, $i, $a);
   }

   if($i % 100 == 0) { $attachment->update_checkpoint($i); }
}

$attachment->update_checkpoint($i);

// Release lock
flock($fp, LOCK_UN);
fclose($fp);



function usage() {
   print "\nUsage: " . __FILE__ . "\n\n";
   print "\t--webui <path to webui directory>\n";
   print "\t--dir <basedir to write attachments>\n";
   print "\t--help\n\n";

   exit;
}
