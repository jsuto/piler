<?php

require 'Zend/Mime/Decode.php';

class ModelSearchMessage extends Model {

   public $encoding_aliases = array(
                                     'GB2312'   => 'GBK',
                                     'GB231280' => 'GBK'
                                   );
   public $message;
   private $verification = 0;

   public function get_boundary($line='') {
      $parts = explode(";", $line);

      for($i=0; $i<count($parts); $i++) {
         if(stristr($parts[$i], "boundary")) {
            $parts[$i] = preg_replace("/boundary\s{0,}=\s{0,}/i", "boundary=", $parts[$i]);
            $parts[$i] = preg_replace("/\"\;{0,1}/", "", $parts[$i]);
            $parts[$i] = preg_replace("/\'/", "", $parts[$i]);

            $b = explode("boundary=", $parts[$i]);

            return rtrim($b[count($b)-1]);
         }
      }

      return "";
   }


   public function verify_message($piler_id = '', $data = '') {
      if($piler_id == '') { return 0; }

      $q = $this->db->query("SELECT `size`, `hlen`, `digest`, `bodydigest`,`attachments` FROM " . TABLE_META . " WHERE piler_id=?", array($piler_id));

      $digest = $q->row['digest'];
      $bodydigest = $q->row['bodydigest'];
      $size = $q->row['size'];
      $hlen = $q->row['hlen'];
      $attachments = $q->row['attachments'];

      $_digest = openssl_digest($data, "SHA256");
      $_bodydigest = openssl_digest(substr($data, $hlen), "SHA256");

      if($_digest == $digest && $_bodydigest == $bodydigest) {

         if(TSA_PUBLIC_KEY_FILE) {
            $id = $this->get_id_by_piler_id($piler_id);
            if($this->check_rfc3161_timestamp_for_id($id) == 1) { return 1; }
         }
         else { return 1; }
      }

      return 0;
   }


   public function get_raw_message($id = '') {
      $s = '';

      if($id == '' || !preg_match("/^([0-9a-f]+)$/", $id)) { return $s; }

      if(LOG_LEVEL >= DEBUG) { syslog(LOG_INFO, DECRYPT_BINARY . " $id"); }

      $handle = popen(DECRYPT_BINARY . " $id", "r");
      while(($buf = fread($handle, DECRYPT_BUFFER_LENGTH))) {
         $s .= $buf;
      }
      pclose($handle);

      if($s == '') {
         $handle = popen(DECRYPT_BINARY . " $id nocrypt", "r");
         while(($buf = fread($handle, DECRYPT_BUFFER_LENGTH))) {
            $s .= $buf;
         }
         pclose($handle);
      }

      if(ENABLE_ON_THE_FLY_VERIFICATION == 0) {
         $this->verification = $this->verify_message($id, $s);
      }

      if(Registry::get('auditor_user') == 0 && HEADER_LINE_TO_HIDE) {
         $s = preg_replace("/" . HEADER_LINE_TO_HIDE . ".{1,}(\n(\ |\t){1,}.{1,}){0,}" . "\n/i", "", $s);
      }

      return $s;
   }


   public function get_attachment($piler_id = '', $attachment_id = '') {
      $data = '';

      if($piler_id == '' || $attachment_id == '' || !preg_match("/^([0-9a-f]+)$/", $piler_id) || !preg_match("/^([0-9m]+)$/", $attachment_id)) { return $data; }

      if(LOG_LEVEL >= DEBUG) { syslog(LOG_INFO, DECRYPT_ATTACHMENT_BINARY . " $piler_id $attachment_id"); }

      $handle = popen(DECRYPT_ATTACHMENT_BINARY . " $piler_id $attachment_id", "r");
      while(($buf = fread($handle, DECRYPT_BUFFER_LENGTH))){
         $data .= $buf;
      }
      pclose($handle);

      /* check if it's a base64 encoded stuff */

      $s = substr($data, 0, 4096);
      $s = preg_replace("/(\r|\n)/", "", $s);

      if(!preg_match("/\s/", $s)) {
         return base64_decode(preg_replace("/\s/", "", $data));
      }

      return $data;
   }


   public function get_message_headers($id = '') {
      $headers = '';

      $msg = $this->get_raw_message($id);

      Zend_Mime_Decode::splitMessageRaw($msg, $headers, $body);

      $has_journal = $this->remove_journal($headers);

      $headers = $this->escape_lt_gt_symbols($headers);

      return array('headers' => $headers, 'has_journal' => $has_journal);
   }


   public function get_message_journal($id = '') {
      $data = '&lt; &gt;';
      $boundary = '';

      $msg = $this->get_raw_message($id);

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

         $data = $this->escape_lt_gt_symbols($data);
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

         if($boundary) {
            $msg = substr($msg, 0, strlen($msg) - strlen($boundary) - 6);
         }
      }

      return $has_journal;
   }


   public function extract_message($id = '', $terms = '') {
      $from = "From: ";
      $to = "To: ";
      $subject = "Subject: ";
      $date = "Date: ";

      $msg = $this->get_raw_message($id);

      $has_journal = $this->remove_journal($msg);

      Zend_Mime_Decode::splitMessage($msg, $headers, $body);
      $boundary = $this->get_boundary($headers['content-type']);

      if(is_array($headers['date'])) { $headers['date'] = $headers['date'][0]; }

      if(isset($headers['from'])) $from .= $this->escape_lt_gt_symbols($headers['from']);
      if(isset($headers['to'])) $to .= $this->escape_lt_gt_symbols($headers['to']);
      if(isset($headers['subject'])) $subject .= $this->escape_lt_gt_symbols($headers['subject']);
      if(isset($headers['date'])) $date .= $headers['date'];

      $this->message = array(
                             'text/plain' => '',
                             'text/html' => ''
                            );

      $this->extract_textuals_from_mime_parts($headers, $body, $boundary);

      return array('from' => $from,
                   'to' => $to,
                   'subject' => $this->highlight_search_terms($subject, $terms),
                   'date' => $date,
                   'message' => $this->message['text/html'] ? $this->message['text/html'] : $this->message['text/plain'],
                   'has_journal' => $has_journal,
                   'verification' => $this->verification
            );
   }


   private function extract_textuals_from_mime_parts($headers = array(), $body = '', $boundary = '') {
      $mime_parts = array();

      if($boundary) {
         $mime_parts = Zend_Mime_Decode::splitMessageStruct($body, $boundary);
      } else {
         $mime_parts[] = array('header' => $headers, 'body' => $body);
      }

      require_once DIR_SYSTEM . 'helper/HTMLPurifier.standalone.php';

      $config = HTMLPurifier_Config::createDefault();
      $config->set('URI', 'DisableExternal', 'true');
      $config->set('URI', 'DisableExternalResources', 'true');
      $config->set('Cache.SerializerPath', DIR_BASE . 'tmp');

      $purifier = new HTMLPurifier($config);

      for($i=0; $i<count($mime_parts); $i++) {
         $mime = array(
                       'content-type' => '',
                       'encoding' => ''
                      );

         if(isset($mime_parts[$i]['header']['content-type'])) {
            $mime['content-type'] = Zend_Mime_Decode::splitContentType($mime_parts[$i]['header']['content-type']);
         }
         /*
           Fix the mime type for some emails having a single textual body part
           without the Content-type header.
          */
         else if (count($mime_parts) == 1) {
            $mime['content-type']['type'] = 'text/plain';
         }

         if(in_array($mime['content-type']['type'], array('multipart/mixed', 'multipart/related', 'multipart/alternative')))
            $this->extract_textuals_from_mime_parts($mime_parts[$i]['header'], $mime_parts[$i]['body'], $mime['content-type']['boundary']);

         if(isset($mime_parts[$i]['header']['content-transfer-encoding']))
            $mime['encoding'] = $mime_parts[$i]['header']['content-transfer-encoding'];

         if(in_array($mime['content-type']['type'], array('text/plain', 'text/html')))
            $this->message[$mime['content-type']['type']] .= $this->fix_mime_body_part($purifier, $mime, $mime_parts[$i]['body']);
      }
   }


   private function fix_mime_body_part($purifier, $mime = array(), $body = '') {
      if($mime['encoding'] == 'quoted-printable')
         $body = Zend_Mime_Decode::decodeQuotedPrintable($body);

      if($mime['encoding'] == 'base64')
         $body = base64_decode($body);

      if(strtolower($mime['content-type']['charset']) != 'utf-8')
         $body = iconv($mime['content-type']['charset'], 'utf-8' . '//IGNORE', $body);


      if(strtolower($mime['content-type']['type']) == 'text/plain') {

         $body = $this->escape_lt_gt_symbols($body);

         $body = preg_replace("/\n/", "<br />\n", $body);
         $body = "\n" . $this->print_nicely($body);
      }

      if(strtolower($mime['content-type']['type']) == 'text/html') {
         $body = $purifier->purify($body);
      }

      return $body;
   }


   private function escape_lt_gt_symbols($s = '') {
      $s = preg_replace("/</", "&lt;", $s);
      $s = preg_replace("/>/", "&gt;", $s);

      return $s;
   }


   private function highlight_search_terms($s = '', $terms = '', $html = 0) {
      $fields = array("from:", "to:", "subject:", "body:");

      $terms = preg_replace("/(\,|\s){1,}/", " ", $terms);

      $a = explode(" ", $terms);
      $terms = array();

      while(list($k, $v) = each($a)) {
         if(strlen($v) >= 3 && !in_array($v, $fields)) {
            $v = preg_replace("/\*/", "", $v);
            if($v) { array_push($terms, $v); }
         }
      }

      if(count($terms) <= 0) { return $s; }

      if($html == 0) {
         while(list($k, $v) = each($terms)) {
            $s = preg_replace("/$v/i", "<span class=\"message_highlight\">$v</span>", $s);
         }

         return $s;
      }

      $tokens = preg_split("/\</", $s);
      $s = '';

      while(list($k, $token) = each($tokens)) {

         $pos = strpos($token, ">");
         if($pos > 0) {
            $len = strlen($token);

            $s .= '<' . substr($token, 0, $pos) . '>';

            if($len > $pos+1) {
               $str = substr($token, $pos+1, $len);

               reset($terms);
               while(list($k, $v) = each($terms)) {
                  $str = preg_replace("/$v/i", "<span class=\"message_highlight\">$v</span>", $str);
               }

               $s .= $str;
            }

         }
      }

      return $s;
   }


   private function print_nicely($chunk) {
      $k = 0;
      $nice_chunk = "";

      $x = explode(" ", $chunk);

      for($i=0; $i<count($x); $i++){
         $nice_chunk .= $x[$i] . " ";
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


   public function get_subject_id_by_id($id = 0) {
      $query = $this->db->query("SELECT `subject` FROM `" . TABLE_META . "` WHERE id=?", array($id));
      if(isset($query->row['subject'])) { return $query->row['subject']; }
      return '';
   }


   public function get_metadata_by_id($id = 0) {
      $query = $this->db->query("SELECT * FROM `" . TABLE_META . "` WHERE id=?", array($id));
      if(isset($query->row['piler_id'])) { return $query->row; }
      return '';
   }


   public function fix_subject($s = '') {
      if($s == '') { $s = 'nosubject'; }
      return preg_replace("/^\-{1,}/", "", preg_replace("/\W{1,}/", "-", $s));
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


   public function check_rfc3161_timestamp_for_id($id = 0) {
      $s = '';
      $computed_hash = '';

      /*
       * determine which entry in the timestamp table holds the aggregated hash value,
       * then compute the aggregated hash value for the digests between start_id and stop_id.
       * If the hashes are the same, then verify by the public key as well
       */

      $query = $this->db->query("SELECT `start_id`, `stop_id`, `hash_value`, `response_time`, `response_string` FROM " . TABLE_TIMESTAMP . " WHERE start_id <= ? AND stop_id >= ?", array($id, $id));

      if(isset($query->row['start_id']) && isset($query->row['stop_id'])) {

         if(MEMCACHED_ENABLED) {
            $cache_key = "rfc3161_" . $query->row['start_id'] . "+" . $query->row['stop_id'];
            $memcache = Registry::get('memcache');
            $computed_hash = $memcache->get($cache_key);
         }

         if($computed_hash == '') {

            $query2 = $this->db->query("SELECT digest FROM " . TABLE_META . " WHERE id >= ? AND id <= ?", array($query->row['start_id'], $query->row['stop_id']));

            foreach($query2->rows as $q) {
               $s .= $q['digest'];
            }

            $computed_hash = sha1($s);

            if(MEMCACHED_ENABLED) {
               $memcache->add($cache_key, $computed_hash, 0, MEMCACHED_TTL);
            }
         }

         if($query->row['hash_value'] == $computed_hash) {
            $validate = TrustedTimestamps::validate($query->row['hash_value'], $query->row['response_string'], $query->row['response_time'], TSA_PUBLIC_KEY_FILE);
            if($validate == true) { return 1; }
         }

      }

      return 0;
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


   public function is_message_spam($id = 0) {
      $spam = 0;

      if($id > 0 && DEFAULT_RETENTION > 30) {
         $query = $this->db->query("SELECT spam FROM " . TABLE_META . " WHERE id=?", array($id));

         if(isset($query->row['spam'])) { $spam = $query->row['spam']; }
      }

      return $spam;
   }


   public function not_spam($id = 0) {

      if($id > 0) {
         $query = $this->db->query("UPDATE " . TABLE_META . " SET spam=0, retained=? WHERE id=?", array(NOW + (DEFAULT_RETENTION*86400), $id));
      }

   }


   public function mark_as_private($id = 0) {
      if($id > 0) {
         $query = $this->db->query("INSERT INTO " . TABLE_PRIVATE . " (id) VALUES(?)", array($id));
      }

      return 1;
   }


   public function unmark_as_private($id = 0) {
      if($id > 0) {
         $query = $this->db->query("DELETE FROM " . TABLE_PRIVATE . " WHERE id=?", array($id));
      }

      return 1;
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


   public function get_message_private($id = 0) {
      if($id == 0) { return 0; }

      $query = $this->db->query("SELECT `id` FROM " . TABLE_PRIVATE . " WHERE id=?", array($id));

      if(isset($query->row['id'])) { return 1; }

      return 0;
   }

}

?>
