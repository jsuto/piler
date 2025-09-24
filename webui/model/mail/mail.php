<?php

class ModelMailMail extends Model {


   public function send_smtp_email($smtphost, $smtpport, $yourdomain, $from, $to, $msg){

      require_once 'Zend/Mail/Protocol/Smtp.php';
      require_once 'Zend/Mail/Protocol/Smtp/Auth/Login.php';

      // Workaround for the zend framework
      $msg = str_replace("\r", "", $msg);

      $ok = 0;

      if($to == "" || strlen($msg) < 30){ return $ok; }

      if(REWRITE_MESSAGE_ID == 1) {
         $msg = preg_replace("/Message-ID:([^\n]+)\n/i", "Message-ID: <" . generate_random_string(25) . '@' . SITE_NAME . ">\n", $msg);
      }

      # config array for connection configuration
      $config = [];

      # setting implicit ssl/tls for port 465 and explicit ssl/tls for any other port
      # in config array zend needs ssl and port 465 for implicit ssl/tls
      # and tls with any other port for explicit ssl/tls
      if ($smtpport == '465') {
         $config['ssl'] = 'ssl';
      } else {
         $config['ssl'] = 'tls';
      }

      $config['port'] = $smtpport;
      $config['name'] = $yourdomain;

      try {
         $config['username'] = SMARTHOST_USER;
         $config['password'] = SMARTHOST_PASSWORD;

         if($config['username'] && $config['password']) {
            $config['auth'] = 'login';
            $connection = new Zend_Mail_protocol_Smtp_Auth_Login($smtphost, $smtpport, $config);
         } else {
            $connection = new Zend_Mail_protocol_Smtp($smtphost, $smtpport, $config);
         }

         $connection->connect();
         $connection->helo($smtphost);
      } catch (Exception $e) {
         $connection->__destruct();

         # fallback to unencrypted connection if the port is not 465
         if ($smtpport != '465') {
            unset($config['ssl']);

            try {
               if($config['username'] && $config['password']) {
                  $config['auth'] = 'login';
                  $connection = new Zend_Mail_protocol_Smtp_Auth_Login($smtphost, $smtpport, $config);
               } else {
                  $connection = new Zend_Mail_protocol_Smtp($smtphost, $smtpport, $config);
               }

               $connection->connect();
               $connection->helo(SMTP_DOMAIN);

            } catch (Exception $e) {
               $connection->__destruct();
               syslog(LOG_ERR, "sending mail from=$from, rcpt=" . implode(" ", $to) . ", status=$ok (" . $e->getCode() . "), msg=" . rtrim($e->getMessage()) );
               return -1;
            }
         } else {
            syslog(LOG_ERR, "sending mail from=$from, rcpt=" . implode(" ", $to) . ", status=$ok (" . $e->getCode() . "), msg=" . rtrim($e->getMessage()) );
            return -1;
         }
      }


      try {
         $connection->mail($from);

         foreach ($to as $recipient) {
            $connection->rcpt($recipient);
         }

         $connection->data($msg);

         $connectionResponse = $connection->getResponse();

         $connectionResponseArray = explode(" ", $connectionResponse[0]);

         $queue_id = '';

         if($connectionResponseArray[0] == 250) {
            $ok = 1;
            $queue_id = trim($connectionResponse[0]);
         }

         syslog(LOG_INFO, "sending mail from=$from, rcpt=" . implode(" ", $to) . ", status=$ok, queue_id=$queue_id");

      } catch (Exception $e) {
         syslog(LOG_ERR, "sending mail from=$from, rcpt=" . implode(" ", $to) . ", status=$ok (" . $e->getCode() . "), msg=" . rtrim($e->getMessage()) );
         $ok = -1;
      } finally {
         $connection->__destruct();
      }

      return $ok;
   }


   public function message_as_rfc822_attachment($piler_id = '', $msg = '', $rcpt = '') {
      if($piler_id == '' || $msg == '' || $rcpt == '') { return ''; }

      $boundary = generate_random_string(24);

      $hdr = substr($msg, 0, 8192);
      $subject = "";

      $s = strstr($hdr, "Subject:");
      if($s) {
         $l1 = strlen($s);
         $l2 = strlen(strstr($s, EOL));
         if($l1 > $l2 + 10) {
            $subject = substr($s, 0, $l1 - $l2) . EOL;
         }
      }


      $s = "";
      $s .= "Received: by piler" . EOL . PILER_HEADER_FIELD . $piler_id . EOL;
      $s .= "Date: " . date("r") . EOL;
      $s .= "Message-ID: <" . generate_random_string(25) . '@' . SITE_NAME . ">" . EOL;
      $s .= "From: " . SMTP_FROMADDR . EOL;
      $s .= "To: " . $rcpt . EOL;

      if($subject) { $s .= $subject; }
      else { $s .= "Subject: Retrieved message from the email archive" . EOL; }

      $s .= "MIME-Version: 1.0" . EOL;
      $s .= "Content-Type: multipart/mixed; boundary=\"$boundary\"" . EOL . EOL . EOL;
      $s .= "--$boundary" . EOL;
      $s .= "Content-Type: message/rfc822; name=\"" . $piler_id . "\"" . EOL;
      $s .= "Content-Disposition: attachment; filename=\"" . $piler_id . "\"" . EOL . EOL;
      $s .= $msg . EOL;
      $s .= "--$boundary" . EOL;

      return $s;
   }


   public function connect_imap() {
      $this->imap = new Zend_Mail_Protocol_Imap(IMAP_HOST, IMAP_PORT, IMAP_SSL);

      $session = Registry::get('session');

      if($this->imap) {
         if($this->imap->login($session->get("username"), $session->get("password"))) { return 1; }
      }

      return 0;
   }


   public function disconnect_imap() {
      $this->imap->logout;
   }


}
