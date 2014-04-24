<?php

class ModelGoogleGoogle extends Model {

   //Holds the Zend Storage
   private $storage;

   private function constructAuthString($email, $accessToken) {
      return base64_encode("user=$email\1auth=Bearer $accessToken\1\1");
   }


   public function oauth2Authenticate($imap, $email, $accessToken) {
      $authenticateParams = array('XOAUTH2', $this->constructAuthString($email, $accessToken));

      $imap->sendRequest('AUTHENTICATE', $authenticateParams);
      while (true) {
         $response = "";
         $is_plus = $imap->readLine($response, '+', true);
         if($is_plus) {
            error_log("got an extra server challenge: $response");
            // Send empty client response.
            $imap->sendRequest('');
         } else {
            if(preg_match('/^NO /i', $response) ||
               preg_match('/^BAD /i', $response)) {
               error_log("got failure response: $response");
               return false;
            } else if (preg_match("/^OK /i", $response)) {
               return true;
            } else {
               // Some untagged response, such as CAPABILITY
            }
         }
      }
   }


   public function try_imap_login($email, $accessToken) {
      $imap = new Zend_Mail_Protocol_Imap('imap.gmail.com', '993', true);

      if($this->oauth2Authenticate($imap, $email, $accessToken)) {
         return $imap;
      } else {
         return '';
      }
   }


   // Save all Messages from selected folder

   private function saveMessages($email) {
      $last_msg_id = -1;
      $from = 1;
      $count = 0;
      $storage = $this->storage;

      $from = $this->get_last_message_id_by_unique_id($email, $storage) + 1;

      //print "will download messages from: $from\n";


      $num = $storage->countMessages();

      $to = $from;

      while($from <= $num) {
         if($num - $from > 9) { $delta = 9; }
         else { $delta = $num-$from; }

         $to = $from + $delta;

         //print "downloading $from, $to\n";

         $unique_msg_id = $storage->getUniqueId($to);

         $messages = $storage->piler_batch_fetch($from, $to);

         while(list($k, $v) = each($messages)) {
            $uuid = $storage->getUniqueId($k);

            $tmpname = "piler-" . $email . "-" . $k . "-" . $uuid . ".eml";
            $f = fopen(DIR_TMP . "/" . $tmpname, "w+");
            if($f){
               fwrite($f, $v['RFC822.HEADER'] . $v['RFC822.TEXT']);
               fclose($f);

               rename(DIR_TMP . "/" . $tmpname, DIR_IMAP . "/" . $tmpname);

               $count++;
            }
            //print "k: $k\n";
         }

         $this->update_imap_table($email, $unique_msg_id, $to);

         $from += $delta + 1;
      }


      syslog(LOG_INFO, "downloaded $count messages for $email");   

      return $count;
   }


   public function download_users_emails($email, $accessToken) {
      $count = 0;

      if(!$email || !$accessToken) { return 0; }

      $imap = $this->try_imap_login($email, $accessToken);

      if($imap) {

         $this->storage = new Zend_Mail_Storage_Imap($imap);

         //$storage->selectFolder('[Gmail]/INBOX');
         $folders = new RecursiveIteratorIterator($this->storage->getFolders(),
                                         RecursiveIteratorIterator::SELF_FIRST);

         foreach ($folders as $localName => $folder) {

            if($folder->isSelectable()) {
               $this->storage->selectFolder($folder);
               $count += $this->saveMessages($email);
            }

         }
      }

      if($count > 0) { $this->run_import_command(); }

      return $count;
   }


   private function get_last_message_id_by_unique_id($email = '', $storage) {
      if($email == '') { return 0; }

      $query = $this->db->query("SELECT last_msg_id FROM " . TABLE_GOOGLE_IMAP . " WHERE email=? ORDER BY last_msg_id DESC", array($email));
      if(isset($query->rows)) {
         foreach ($query->rows as $q) {
            try {
               $num = $storage->getNumberByUniqueId($q['last_msg_id']);
               return $num;
            }
            catch(Exception $exception) {}
         }
      }

      return 0;
   }


   private function update_imap_table($email, $unique_msg_id, $id) {
      $query = $this->db->query("INSERT INTO " . TABLE_GOOGLE_IMAP . " (id, email, last_msg_id) VALUES(?,?,?)", array($id, $email, $unique_msg_id));

      return $this->db->countAffected();
   }


   private function run_import_command() {
      syslog(LOG_INFO, "importing emails ...");
      system(PILERIMPORT_IMAP_COMMAND, $val);
      syslog(LOG_INFO, "importing emails done");
   }


}

?>
