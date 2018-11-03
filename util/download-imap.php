<?php


$webuidir = "";

$port = 143;
$ssl = false;
$server = '';
$username = '';
$password = '';


$opts = 'h:s:P:u:p:';
$lopts = array(
                'webui:',
                'server:',
                'port:',
                'username:',
                'password:',
                'help'
              );


if($options = getopt($opts, $lopts)) {

   if(isset($options['webui'])) { $webuidir = $options['webui']; }
   if(isset($options['server'])) { $server = $options['server']; }
   if(isset($options['port'])) { $port = $options['port']; }
   if(isset($options['username'])) { $username = $options['username']; }
   if(isset($options['password'])) { $password = $options['password']; }

   if(isset($options['help'])) { usage(); }
}


if($webuidir == '' || $server == '' || $username == '') { usage(); }

if($port == 993) { $ssl = true; }

$pwd = getcwd();

chdir($webuidir);

require_once 'Zend/Mail/Protocol/Imap.php';
require_once 'Zend/Mail/Storage/Imap.php';


chdir($pwd);


try {
   $imap = new Zend_Mail_Protocol_Imap($server, $port, $ssl);

   $login = $imap->login($username, $password);

   if($login) {
      print "login OK\n";
      $storage = new Zend_Mail_Storage_Imap($imap);
      download_users_emails($storage);

   } else {
      print "login FAILED at $server for $username\n";
   }
}
catch (Zend_Mail_Protocol_Exception $e) {
   print "ERR connecting\n";
}




function download_users_emails($storage) {
   global $imap;
   $count = 0;

   $folders = new RecursiveIteratorIterator($storage->getFolders(), RecursiveIteratorIterator::SELF_FIRST);

   foreach ($folders as $folder) {

      $__folder = preg_replace("/\W/", "_", $folder);

      if($folder->isSelectable()) {
         $storage->selectFolder($folder);
         $num = $storage->countMessages();

         $__folder = preg_replace("/\W/", "_", $folder);

         print "processing $folder ($num messages)\n";

         $count += saveMessages($storage, $__folder, $num);
      }
   }

   print "downloaded $count messages\n";

   return $count;
}


function saveMessages($storage, $folder = '', $num = 0) {
   global $username;
   $count = 0;

   $messages = $storage->piler_batch_fetch(1, $num);

   while(list($k, $v) = each($messages)) {
      $uuid = $storage->getUniqueId($k);

      $tmpname = "piler-" . $username . "-" . $folder . "-" . $k . "-" . $uuid . ".eml";
      $f = fopen($tmpname, "w+");
      if($f){
         fwrite($f, $v['RFC822.HEADER'] . $v['RFC822.TEXT']);
         fclose($f);

         $count++;
      }
   }

   return $count;
}


function usage() {
   print "\nUsage: " . __FILE__ . "\n\n";
   print "\t--webui <path to webui directory>\n";
   print "\t--server <imap server name>\n";
   print "\t--port <port number> [default: 143]\n";
   print "\t--username <imap username>\n";
   print "\t--password <imap password>\n";
   print "\t--help\n\n";

   exit;
}
