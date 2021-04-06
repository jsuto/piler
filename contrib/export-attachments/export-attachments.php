<?php

require_once("config.php");

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

$outdir = "/path/to/attachments";

openlog("export-attachments", LOG_PID, LOG_MAIL);


$domain = new ModelDomainDomain();
$attachment = new ModelMessageAttachment();
$message = new ModelSearchMessage();

$domains = $domain->get_mapped_domains();

$last_id = $attachment->get_last_attachment_id();
$start_id = $attachment->get_checkpoint();

syslog(LOG_INFO, "start: $start, limit: $limit");


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
