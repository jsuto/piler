<?php

class ModelSearchMessage extends Model {


   public function verify_message($id = '') {
      if($id == '') { return 0; }

      $q = $this->db->query("SELECT `size`, `hlen`, `digest`, `bodydigest`,`attachments` FROM " . TABLE_META . " WHERE piler_id=?", array($id));

      $digest = $q->row['digest'];
      $bodydigest = $q->row['bodydigest'];
      $size = $q->row['size'];
      $hlen = $q->row['hlen'];
      $attachments = $q->row['attachments'];

      $data = $this->get_raw_message($id);

      $_digest = openssl_digest($data, "SHA256");
      $_bodydigest = openssl_digest(substr($data, $hlen), "SHA256");

      $data = '';

      if($_digest == $digest && $_bodydigest == $bodydigest) { return 1; }

      return 0;
   }


   public function get_file_size($sd, $id = '') {
      fputs($sd, "STAT $id\r\n");
      $s = fgets($sd, 8192);

      $a = explode(" ", $s);
      if(isset($a[2]) && $a[2] > 0) { return $a[2]; }

      return 0;
   }


   public function read_file($sd, $f = '', $size = 0) {
      global $start;
      $s = '';
      $len = 0;

      if($size <= 0) { return $s; }

      fputs($sd, "RETR $f\r\n");

      while(!safe_feof($sd, $start) && (microtime(true) - $start) < PILERGETD_TIMEOUT) {
         $s .= fread($sd, PILERGETD_READ_LENGTH);
         $len += PILERGETD_READ_LENGTH;
         if($len >= $size) break;
      }

      return $s;
   }


   public function connect_to_pilergetd() {
      if(PILERGETD_HOST) {
         $sd = fsockopen(PILERGETD_HOST, PILERGETD_PORT);
         if(!$sd) { return FALSE; }

         $l = fgets($sd, 4096);

         if(substr(PILERGETD_HOST, 0, 6) == 'ssl://') {
            fputs($sd, "AUTH " . PILERGETD_PASSWORD . "\r\n");
            $l = fgets($sd, 4096);
         }

         Registry::set('sd', $sd);
      }
   }


   public function disconnect_from_pilergetd() {
      if(PILERGETD_HOST) {
         $sd = Registry::get('sd');

         fputs($sd, "QUIT\r\n");

         fclose($sd);
      }
   }


   public function get_raw_message($id = '') {
      $s = '';

      if($id == '' || !preg_match("/^([0-9a-f]+)$/", $id)) { return $s; }

      if(PILERGETD_HOST) {

         $sd = Registry::get('sd');

         fputs($sd, "MESSAGE $id\r\n");

         $l = fgets($sd, 8192);
         $message = explode(" ", rtrim($l));

         while(list($k, $v) = each($message)) {
            if($k == 0) {
               $size = $this->get_file_size($sd, $v);

               $s = $this->read_file($sd, $v, $size);
               $s = gzuncompress($s);
            }
            else {
               $size = $this->get_file_size($sd, $v);
               $a = $this->read_file($sd, $v, $size);
               $a = gzuncompress($a);

               $repl = "ATTACHMENT_POINTER_" . $id . ".a" . $k . "_XXX_PILER";

               $s = preg_replace("/$repl/", $a, $s);

               $a = '';
            }
         }
      }
      else {
         $handle = popen(DECRYPT_BINARY . " $id", "r");
         while(($buf = fread($handle, DECRYPT_BUFFER_LENGTH))){
            $s .= $buf;
         }
         pclose($handle);
      }


      return $s;
   }


   public function get_attachment($piler_id = '', $attachment_id = '') {
      $data = '';

      if($piler_id == '' || $attachment_id == '' || !preg_match("/^([0-9a-f]+)$/", $piler_id) || !preg_match("/^([0-9m]+)$/", $attachment_id)) { return $data; }

      if(PILERGETD_HOST) {
         $sd = fsockopen(PILERGETD_HOST, PILERGETD_PORT);
         if(!$sd) { return $data; }

         $l = fgets($sd, 4096);

         $size = $this->get_file_size($sd, $piler_id . ".a" . $attachment_id);

         $data = $this->read_file($sd, $piler_id . ".a" . $attachment_id, $size);
         $data = gzuncompress($data);

         fclose($sd);
      }
      else {
         $handle = popen(DECRYPT_ATTACHMENT_BINARY . " $piler_id $attachment_id", "r");
         while(($buf = fread($handle, DECRYPT_BUFFER_LENGTH))){
            $data .= $buf;
         }
         pclose($handle);
      }

      /* check if it's a base64 encoded stuff */

      $s = substr($data, 0, 4096);
      $s = preg_replace("/(\r|\n)/", "", $s);

      if(!preg_match("/\s/", $s)) {
         return base64_decode(preg_replace("/\s/", "", $data));
      }

      return $data;
   }


   public function get_message_headers($id = '') {
      $data = '';

      $this->connect_to_pilergetd();
      $msg = $this->get_raw_message($id);
      $this->disconnect_from_pilergetd();

      $has_journal = $this->remove_journal($msg);

      $pos = strpos($msg, "\n\r\n");
      if($pos == false) {
         $pos = strpos($msg, "\n\n");
      }

      if($pos == false) { return $msg; }

      $data = substr($msg, 0, $pos);
      $msg = '';

      $data = preg_replace("/\</", "&lt;", $data);
      $data = preg_replace("/\>/", "&gt;", $data);

      return array('headers' => $data, 'has_journal' => $has_journal);
   }


   public function get_message_journal($id = '') {
      $data = '&lt; &gt;';
      $boundary = '';

      $this->connect_to_pilergetd();
      $msg = $this->get_raw_message($id);
      $this->disconnect_from_pilergetd();

      $hdr = substr($msg, 0, 8192);

      $s = preg_split("/\n/", $hdr);
      while(list($k, $v) = each($s)) {
         if(preg_match("/boundary\s{0,}=\s{0,}\"{0,}([\w\_\-\@\.]+)\"{0,}/i", $v, $m)) {
            if(isset($m[1])) { $boundary = $m[1]; break; }
         }
      }


      $p = strstr($msg, "\nX-MS-Journal-Report:");
      $msg = '';

      if($p) {

         $s = preg_split("/\n/", $p);

         $i=0; $j=0; $data = '';

         while(list($k, $v) = each($s)) {
            if(strstr($v, $boundary)) { $i++; }
            if($i > 0 && preg_match("/^\s{1,}$/", $v)) { $j++; }

            if($j == 1) {
               $data .= "$v\n";
            }

            if($i >= 2) { break; } 
         }

         $p = '';

         $data = preg_replace("/\</", "&lt;", $data);
         $data = preg_replace("/\>/", "&gt;", $data);

      }

      return $data;
   }


   public function remove_journal(&$msg = '') {
      $p = $q = '';
      $boundary = '';
      $has_journal = 0;

      $hdr = substr($msg, 0, 4096);

      $s = preg_split("/\n/", $hdr);
      while(list($k, $v) = each($s)) {
         if(preg_match("/boundary\s{0,}=\s{0,}\"{0,}([\w\_\-\@\.]+)\"{0,}/i", $v, $m)) {
            if(isset($m[1])) { $boundary = $m[1]; break; }
         }
      }

      $p = strstr($msg, "\nX-MS-Journal-Report:");
      if($p) {
         $has_journal = 1;

         $msg = '';
         $q = strstr($p, "Received: from");
         if($q) {
            $p = '';
            $msg = $q;
            $q = '';
         }
         else {
            $msg = $p;
            $p = '';
         }
      }

      if($boundary) {
         $msg = substr($msg, 0, strlen($msg) - strlen($boundary) - 6);
      }

      return $has_journal;
   }


   public function extract_message($id = '', $terms = '') {
      $header = "";
      $body_chunk = "";
      $is_header = 1;
      $state = "UNDEF";
      $b = array();
      $boundary = array();
      $text_plain = 1;
      $text_html = 0;
      $charset = "";
      $qp = $base64 = 0;
      $has_text_plain = 0;
      $rfc822 = 0;
      $_1st_header = 1;

      $from = $to = $subject = $date = $message = "";

      $this->connect_to_pilergetd();
      $msg = $this->get_raw_message($id);
      $this->disconnect_from_pilergetd();

      $has_journal = $this->remove_journal($msg);

      $a = explode("\n", $msg); $msg = "";

      while(list($k, $l) = each($a)){
            $l .= "\n";

            if(($l[0] == "\r" && $l[1] == "\n" && $is_header == 1) || ($l[0] == "\n" && $is_header == 1) ){
               $is_header = $_1st_header = 0;

               if($rfc822 == 1) { $rfc822 = 0; $is_header = 1; }

            }

            if($is_header == 1 && preg_match("/^Content-Type:/i", $l)) $state = "CONTENT_TYPE";
            if($is_header == 1 && preg_match("/^Content-Transfer-Encoding:/i", $l)) $state = "CONTENT_TRANSFER_ENCODING";

            if($state == "CONTENT_TYPE"){
               $x = strstr($l, "boundary");
               if($x){
                  $x = preg_replace("/boundary =/", "boundary=", $x);
                  $x = preg_replace("/boundary= /", "boundary=", $x);

                  $x = preg_replace("/\"\;{0,1}/", "", $x);
                  $x = preg_replace("/\'/", "", $x);

                  $b = explode("boundary=", $x);

                  array_push($boundary, rtrim($b[count($b)-1]));
               }

               if(preg_match("/charset/i", $l)){
                  $types = explode(";", $l);
                  foreach ($types as $type){
                     if(preg_match("/charset/i", $type)){
                        $type = preg_replace("/[\"\'\ ]/", "", $type);

                        $x = explode("=", $type);
                        $charset = $x[1];
                     }
                  }
               }

               if(strstr($l, "message/rfc822")) { $rfc822 = 1; }

               if(strstr($l, "text/plain")){ $text_plain = 1; $has_text_plain = 1; }
               if(strstr($l, "text/html")){ $text_html = 1; $text_plain = 0; }
            }

            if($state == "CONTENT_TRANSFER_ENCODING"){
               if(preg_match("/quoted-printable/i", $l)){ $qp = 1; }
               if(preg_match("/base64/i", $l)){ $base64 = 1; }
            }


            if($is_header == 1){
               if($l[0] != " " && $l[0] != "\t"){ $state = "UNDEF"; }
               if(preg_match("/^From:/i", $l)){ $state = "FROM"; }
               if(preg_match("/^To:/i", $l) || preg_match("/^Cc:/i", $l)){ $state = "TO"; }
               if(preg_match("/^Date:/i", $l)){ $state = "DATE"; }
               if(preg_match("/^Subject:/i", $l)){ $state = "SUBJECT"; }
               if(preg_match("/^Content-Type:/", $l)){ $state = "CONTENT_TYPE"; }
               if(preg_match("/^Content-Disposition:/", $l)){ $state = "CONTENT_DISPOSITION"; }

               $l = preg_replace("/</", "&lt;", $l);
               $l = preg_replace("/>/", "&gt;", $l);

               if($_1st_header == 1) {
                  if($state == "FROM"){ $from .= preg_replace("/\r|\n/", "", $l); }
                  if($state == "TO"){ $to .= preg_replace("/\r|\n/", "", $l); }
                  if($state == "SUBJECT"){ $subject .= preg_replace("/\r|\n/", "", $l); }
                  if($state == "DATE"){ $date .= preg_replace("/\r|\n/", "", $l); }
               }
            }
            else {

               if($this->check_boundary($boundary, $l) == 1){

                  if($text_plain == 1 || $has_text_plain == 0) {
                     $message .= $this->flush_body_chunk($body_chunk, $charset, $qp, $base64, $text_plain, $text_html);
                  }

                  $text_plain = $text_html = $qp = $base64 = 0;

                  $charset = $body_chunk = "";

                  $is_header = 1;

                  continue;
               }

               else if(($l[0] == "\r" && $l[1] == "\n") || $l[0] == "\n"){
                  $state = "BODY";
                  $body_chunk .= $l;
               }

               else if($state == "BODY"){
                  if($text_plain == 1 || $text_html == 1){ $body_chunk .= $l; }

               }

            }


         }

      if($body_chunk && ($text_plain == 1 || $has_text_plain == 0) ){
         $message .= $this->flush_body_chunk($body_chunk, $charset, $qp, $base64, $text_plain, $text_html);
      }


      return array('from' => $this->decode_my_str($from),
                   'to' => $this->decode_my_str($to),
                   'subject' => $this->highlight_search_terms($this->decode_my_str($subject), $terms),
                   'date' => $this->decode_my_str($date),
                   'message' => $this->highlight_search_terms($message, $terms),
                   'has_journal' => $has_journal
            );
   }


   private function highlight_search_terms($s = '', $terms = '') {
      $terms = preg_replace("/(\'|\")/", "", $terms);

      $terms = explode(" ", $terms);

      if(count($terms) <= 0) { return $s; }

      while(list($k, $v) = each($terms)) {
         if(strlen($v) < 3) { continue; }

         $s = preg_replace("/$v/i", "<span class=\"message_highlight\">$v</span>", $s);
      }

      return $s;
   }


   private function check_boundary($boundary, $line) {

      for($i=0; $i<count($boundary); $i++){
         if(strstr($line, $boundary[$i])){
            return 1;
         }
      }

      return 0;
   }


   private function flush_body_chunk($chunk, $charset, $qp, $base64, $text_plain, $text_html) {

      if($qp == 1){
         $chunk = $this->qp_decode($chunk);
      }

      if($base64 == 1){
         $chunk = base64_decode($chunk);
      }

      if($charset && !preg_match("/utf-8/i", $charset)){
         $s = @iconv($charset, 'utf-8', $chunk);
         if($s) { $chunk = $s; $s = ''; }
      }

      if($text_plain == 1){
         $chunk = preg_replace("/</", "&lt;", $chunk);
         $chunk = preg_replace("/>/", "&gt;", $chunk);

         $chunk = preg_replace("/\n/", "<br />\n", $chunk);
         $chunk = "\n" . $this->print_nicely($chunk);
      }

      if($text_html == 1){
         $chunk = preg_replace("/\<style([^\>]+)\>([\w\W]+)\<\/style\>/i", "", $chunk);

         if(ENABLE_REMOTE_IMAGES == 0) {
            $chunk = preg_replace("/style([\s]{0,}=[\s]{0,})\"([^\"]+)/", "style=\"xxxx", $chunk);
            $chunk = preg_replace("/style([\s]{0,}=[\s]{0,})\'([^\']+)/", "style=\'xxxx", $chunk);
         }

         $chunk = preg_replace("/\<body ([\w\s\;\"\'\#\d\:\-\=]+)\>/i", "<body>", $chunk);

         if(ENABLE_REMOTE_IMAGES == 0) { $chunk = preg_replace("/\<img([^\>]+)\>/i", "<img src=\"" . REMOTE_IMAGE_REPLACEMENT . "\" />", $chunk); }

         /* prevent scripts in the HTML part */

         $chunk = preg_replace("/document\.write/", "document.writeee", $chunk);
         $chunk = preg_replace("/<\s{0,}script([\w\W]+)\/script\s{0,}\>/i", "<!-- disabled javascript here -->", $chunk);
      }

      return $chunk;
   }


   private function print_nicely($chunk) {
      $k = 0;
      $nice_chunk = "";

      $x = explode(" ", $chunk);

      for($i=0; $i<count($x); $i++){
         $nice_chunk .= "$x[$i] ";
         $k += strlen($x[$i]);

         if(strstr($x[$i], "\n")){ $k = 0; }

         if($k > 70){ $nice_chunk .= "\n"; $k = 0; }
      }

      return $nice_chunk;
   }


   public function NiceSize($size) {
      if($size < 1000) return "1k";
      if($size < 100000) return round($size/1000) . "k";

      return sprintf("%.1f", $size/1000000) . "M";
   }


   private function qp_decode($l) {
      $res = "";
      $c = "";

      if($l == ""){ return ""; }

      /* remove soft breaks at the end of lines */

      if(preg_match("/\=\r\n/", $l)){ $l = preg_replace("/\=\r\n/", "", $l); }
      if(preg_match("/\=\n/", $l)){ $l = preg_replace("/\=\n/", "", $l); }

      for($i=0; $i<strlen($l); $i++){
         $c = $l[$i];

         if($c == '=' && ctype_xdigit($l[$i+1]) && ctype_xdigit($l[$i+2])){
            $a = $l[$i+1];
            $b = $l[$i+2];

            $c = chr(16*hexdec($a) + hexdec($b));

            $i += 2;
         }

         $res .= $c;

      }

      return $res;
   }


   public function decode_my_str($what = '') {
      $result = "";

      $what = rtrim($what);

      $a = preg_split("/\s/", $what);

      while(list($k, $v) = each($a)){
         $x = preg_match("/\?\=$/", $v);

         if( ($x == 0 && $k > 0) || ($x == 1 && $k == 1) ){
            $result .= " ";
         }

         $result .= $this->fix_encoded_string($v);
      }

      return $result;
   }


   private function fix_encoded_string($what = '') {
      $s = "";

      $what = rtrim($what, "\"\r\n");
      $what = ltrim($what, "\"");

      if(preg_match("/^\=\?/", $what) && preg_match("/\?\=$/", $what)){
         $what = preg_replace("/^\=\?/", "", $what);
         $what = preg_replace("/\?\=$/", "", $what);

         $encoding = substr($what, 0, strpos($what, '?'));

         if(preg_match("/\?Q\?/i", $what)){
            $x = preg_replace("/^([\w\-]+)\?Q\?/i", "", $what);

            $s = quoted_printable_decode($x);
            $s = preg_replace("/_/", " ", $s);
         }

         if(preg_match("/\?B\?/i", $what)){
            $x = preg_replace("/^([\w\-]+)\?B\?/i", "", $what);

            $s = base64_decode($x);
            $s = preg_replace('/\0/', "*", $s);
         }

         if(!preg_match("/utf-8/i", $encoding)){
            $s = iconv($encoding, 'utf-8', $s);
         }

      }
      else {
         $s = utf8_encode($what);
      }

      return $s;
   }


   public function get_piler_id_by_id($id = 0) {
      $query = $this->db->query("SELECT `piler_id` FROM `" . TABLE_META . "` WHERE id=?", array($id));
      if(isset($query->row['piler_id'])) { return $query->row['piler_id']; }
      return '';
   }


   public function get_id_by_piler_id($piler_id = '') {
      if($piler_id == '') { return -1; }

      $query = $this->db->query("SELECT `id` FROM `" . TABLE_META . "` WHERE piler_id=?", array($piler_id));
      if(isset($query->row['id'])) { return $query->row['id']; }

      return -1;
   }


   public function get_attachment_by_id($id = 0) {
      if($id <= 0) { return array(); }

      $query = $this->db->query("SELECT id, piler_id, attachment_id, name, type, ptr FROM " . TABLE_ATTACHMENT . " WHERE id=?", array($id));

      if(isset($query->row)) {
         $metaid = $this->get_id_by_piler_id($query->row['piler_id']);

         if($metaid > 0 && $this->model_search_search->check_your_permission_by_id($metaid) == 1) {
            if($query->row['ptr'] > 0) {
               $query = $this->db->query("SELECT id, piler_id, attachment_id, name, type FROM " . TABLE_ATTACHMENT . " WHERE id=?", array($query->row['ptr']));
            }

            $attachment = $this->get_attachment($query->row['piler_id'], $query->row['attachment_id']);

            return array('filename' => fix_evolution_mime_name_crap($query->row['name']), 'piler_id' => $query->row['piler_id'], 'attachment' => $attachment);
         }
      }

      return array();
   }


   public function get_attachment_list($piler_id = 0) {
      $data = array();

      if($piler_id == '') { return array(); }

      $query = $this->db->query("SELECT id, name, type, ptr FROM " . TABLE_ATTACHMENT . " WHERE piler_id=?", array($piler_id));

      if(!isset($query->rows)) { return array(); }

      foreach($query->rows as $q) {
         $q['name'] = fix_evolution_mime_name_crap($q['name']);
         array_push($data, $q);
      }

      return $data;
   }


   public function get_message_tag($id = '', $uid = 0) {
      if($id == '' || $uid <= 0) { return ''; }

      $query = $this->db->query("SELECT `tag` FROM " . TABLE_TAG . " WHERE uid=? AND id=?", array($uid, $id));

      if(isset($query->row['tag'])) { return strip_tags($query->row['tag']); }

      return '';
   }


   public function add_message_tag($id = '', $uid = 0, $tag = '') {
      if($id == '' || $uid <= 0) { return 0; }

      if($tag == '') {
         $query = $this->db->query("DELETE FROM " . TABLE_TAG . " WHERE uid=? AND id=?", array($uid, $id));
      } else {
         $query = $this->db->query("UPDATE " . TABLE_TAG . " SET tag=? WHERE uid=? AND id=?", array($tag, $uid, $id));
         if($this->db->countAffected() == 0) {
            $query = $this->db->query("INSERT INTO " . TABLE_TAG . " (id, uid, tag) VALUES(?,?,?)", array($id, $uid, $tag));
         }
      }

      return 1;
   }


   public function bulk_add_message_tag($ids = array(), $uid = 0, $tag = '', $q = '') {
      $arr = array_merge(array($uid), $ids);

      $query = $this->db->query("DELETE FROM " . TABLE_TAG . " WHERE uid=? AND id IN ($q)", $arr);

      if($tag) {
         foreach ($ids as $id) {
            $query = $this->db->query("INSERT INTO " . TABLE_TAG . " (id, uid, tag) VALUES(?,?,?)", array($id, $uid, $tag));
         }
      } 
   }


   public function get_message_note($id = '', $uid = 0) {
      if($id == '' || $uid <= 0) { return ''; }

      $query = $this->db->query("SELECT `note` FROM " . TABLE_NOTE . " WHERE uid=? AND id=?", array($uid, $id));

      if(isset($query->row['note'])) { return strip_tags(urldecode($query->row['note'])); }

      return '';
   }


   public function add_message_note($id = '', $uid = 0, $note = '') {
      if($id == '' || $uid <= 0) { return 0; }

      if($note == '') {
         $query = $this->db->query("DELETE FROM " . TABLE_NOTE . " WHERE uid=? AND id=?", array($uid, $id));
      } else {
         $query = $this->db->query("UPDATE " . TABLE_NOTE . " SET note=? WHERE uid=? AND id=?", array($note, $uid, $id));
         if($this->db->countAffected() == 0) {
            $query = $this->db->query("INSERT INTO " . TABLE_NOTE . " (id, uid, note) VALUES(?,?,?)", array($id, $uid, $note));
         }
      }

      return 1;
   }



}

?>
