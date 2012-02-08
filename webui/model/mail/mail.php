<?php

class ModelMailMail extends Model {


   public function send_smtp_email($smtphost, $smtpport, $yourdomain, $from, $to = array(), $msg){
      $ok = 0;

      if($to == "" || strlen($msg) < 30){ return $ok; }

      $r = fsockopen($smtphost, $smtpport);
      if(!$r){ return -1; }

      $l = fgets($r, 4096);

      fputs($r, "HELO $yourdomain\r\n");
      $l = fgets($r, 4096);

      fputs($r, "MAIL FROM: <$from>\r\n");
      $l = fgets($r, 4096);

      while(list($k, $v) = each($to)) {
         fputs($r, "RCPT TO: <$v>\r\n");
         $l = fgets($r, 4096);
      }

      fputs($r, "DATA\r\n");
      $l = fgets($r, 4096);
      if(!preg_match("/^354/", $l)){ $l = fgets($r, 4096); }

      fputs($r, $msg);

      if(!preg_match("/\r\n.\r\n$/", $msg)){
         fputs($r, "\r\n.\r\n");
      }

      $l = fgets($r, 4096);

      if(preg_match("/^250/", $l)){ $ok = 1; }

      fputs($r, "QUIT\r\n");
      $l = fgets($r, 4096);

      fclose($r);

      return $ok;
   }

}

?>
