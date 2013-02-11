<?php

function LOGGER($event = '', $username = '') {
   if($event == "") { return 0; }

   if($username == '') {
      if(isset($_SESSION['username'])) { $username = $_SESSION['username']; }
      else { $username = 'unknown'; }
   }

   $log_entry = sprintf("[%s]: %s, %s, '%s'\n", date(LOG_DATE_FORMAT), $username, $_SERVER['REMOTE_ADDR'], $event);

   if($fp = @fopen(LOG_FILE, 'a')) {
      fwrite($fp, $log_entry);
      fflush($fp);
      fclose($fp);
    }

}


function AUDIT($action = 0, $email = '', $ipaddr = '', $id = 0, $description = '') {

   if(ENABLE_AUDIT == 0) { return 0; }

   if($ipaddr == '' && isset($_SERVER['REMOTE_ADDR'])) { $ipaddr = $_SERVER['REMOTE_ADDR']; }
   if($email == '') { $email = $_SESSION['email']; }

   $db = Registry::get('db');

   $query = $db->query("INSERT INTO " . TABLE_AUDIT . " (ts, email, action, ipaddr, meta_id, description) VALUES(?,?,?,?,?,?)", array(time(), $email, $action, $ipaddr, $id, $description));

   return $db->countAffected();
}


function getAuthenticatedUsername() {

   if(isset($_SESSION['username'])){ return $_SESSION['username']; }

   return "";
}


function isAdminUser() {
   if(isset($_SESSION['admin_user']) && $_SESSION['admin_user'] == 1){ return 1; }

   return 0;
}


function isAuditorUser() {
   if(isset($_SESSION['admin_user']) && $_SESSION['admin_user'] == 2){ return 1; }

   return 0;
}


function isReadonlyAdmin() {
   if(isset($_SESSION['admin_user']) && $_SESSION['admin_user'] == 3){ return 1; }

   return 0;
}


function logout() {
   AUDIT(ACTION_LOGOUT, $_SESSION['email'], '', '', '');

   $_SESSION['username'] = "";
   $_SESSION['admin_user'] = 0;

   unset($_SESSION['username']);
   unset($_SESSION['admin_user']);

   Registry::set('username', '');

   session_destroy();
}


function isBinary($num = '') {
   if($num == 0 || $num == 1){ return 1; }

   return 0; 
}


function get_page_length() {
   $page_len = PAGE_LEN;
   
   if(isset($_SESSION['pagelen']) && is_numeric($_SESSION['pagelen']) && $_SESSION['pagelen'] >= 10 && $_SESSION['pagelen'] <= 50) {
      $page_len = $_SESSION['pagelen'];
   }

   return $page_len;
}


function checkemail($email, $domains) {
   if(validemail($email) == 0){
      return 0;
   }

   if($email == 'admin@local') { return 1; }

   list($u, $d) = explode('@', $email);

   foreach ($domains as $domain) {
      if($domain == $d){ return 1; }
   }

   return -1;
}


function validemail($email = '') {
   if($email == '') { return 0; }

   if(preg_match("/@local$/", $email)) { return 1; }

   if(preg_match('/^[_a-zA-Z0-9-]+(\.[_a-zA-Z0-9-]+)*@[a-zA-Z0-9-]+(\.[a-zA-Z0-9-]+)*(\.[a-zA-Z]{2,5})$/', $email)) {
      return 1;
   }

   return 0;
}


function checkdomain($domain, $domains) {
   if(validdomain($domain) == 0){
      return 0;
   }

   if($domain == 'local') { return 1; }

   if(in_array($domain, $domains) ) {
      return 1;
   } else {
      return -1;
   }
}


function validdomain($domain = '') {
   if($domain == '') { return 0; }

   if(preg_match("/@local$/", $domain)) { return 1; }

   if(preg_match('/@?[a-zA-Z0-9-]+(\.[a-zA-Z0-9-]+)*(\.[a-zA-Z]{2,5})$/', $domain)) {
      return 1;
   }

   return 0;
}


function first_n_characters($what, $n){
   $x = "";
   $len = 0;

   $a = explode(" ", $what);
   while(list($k, $v) = each($a)){
      $x .= "$v "; $len += strlen($v) + 1;
      if($len >= $n){ return $x . "..."; }
   }

   return $x . "...";
}


function short_email($email) {
   if(strlen($email) > 25) {
      return substr($email, 0, 25) . "...";
   }

   else return $email;
}


function verify_piler_id($id = '') {
   if($id == '') { return 0; }

   if(preg_match("/^([0-9a-f]+)$/", $id)) { return 1; }

   return 0;
}


function generate_random_string($length = 8) {
   $rnd = "";
   $aZ09 = array_merge(range('A', 'Z'), range('a', 'z'),range(0, 9));

   for($c=0; $c < $length; $c++) {
      $rnd .= $aZ09[mt_rand(0, count($aZ09)-1)];
   }

   return $rnd;
}


function createTempName($dir = '', $prefix = '') {
   return $dir . "/" . $prefix . generate_random_string(8);
}


function read_konfig($configfile = '') {
   $cfg = array();

   if($configfile == '') { return $cfg; }

   $fp = fopen($configfile, "r");
   if($fp) {
      while(($line = fgets($fp, 4096))) {
         $line = trim($line);

         if($line[0] == '#' || $line[0] == ';') { continue; }

         list($host, $basedn, $binddn, $bindpw, $type, $domain, $gid, $policy_group) = explode(":", $line);

         $cfg[] = array(
                         'ldap_host'    => $host,
                         'ldap_basedn'  => $basedn,
                         'ldap_binddn'  => $binddn,
                         'ldap_bindpw'  => $bindpw,
                         'type'         => $type,
                         'domain'       => $domain,
                         'gid'          => $gid,
                         'policy_group' => $policy_group
                       );
      }


      fclose($fp);
   }


   return $cfg;
}


function my_qp_encode($s){
      $res = "";

      $a = explode("\n", $s);
      while(list($k, $v) = each($a)){
         $part = "";

         for($i=0; $i<strlen($v); $i++){
            //if($i > 0 && ($i % 76) == 0) $part .= "=\r\n";

            $c = substr($v, $i, 1);
            if(ord($c) >= 128){
               $c = "=" . strtoupper(dechex(ord($c)));
            }

            if($c == ' ') { $c = '_'; }

            $part .= $c;
         }

         $res .= $part . "\n";
      }

      return $res;
}


function format_qshape($desc = '', $filename = '') {
   $leadingspaces = 999;

   if($filename == '' || !file_exists($filename) ) { return array(); }

   $stat = stat($filename);

   $s = file_get_contents($filename);

   $a = explode("\n", $s);
   while(list($k, $v) = each($a)){
      $len1 = strlen($v);
      $v = preg_replace("/^\ {0,}/", "", $v);
      $delta = $len1 - strlen($v);
      if($len1 > 5 && $delta < $leadingspaces) { $leadingspaces = $delta; }
   }

   reset($a); $s="";

   while(list($k, $v) = each($a)){
      $s .= substr($v, $leadingspaces, strlen($v)) . "\n";
   }
   
   return array('desc' => $desc, 'date' => date(LOG_DATE_FORMAT, $stat['ctime']), 'lines' => $s);
}


function nice_size($size = 0, $space = '') {
   if($size < 1000) return "1k";
   if($size < 100000) return round($size/1000) . $space . "k";

   return sprintf("%.1f", $size/1000000) . $space . "M";
}


function assemble_search_url($term = '') {
   $term_value = "";

   if($term == '') { return $term_value; }

   parse_str($term, $a);

   if(isset($a['search'])) { $term_value = $a['search']; }

   if(isset($a['f'])) {
    foreach($a['f'] as $f) {
      $val = array_shift($a['v']);

      if($val == '') { continue; }

      if($f == 'from') { $term_value .= ", from: " . $val; }
      if($f == 'to') { $term_value .= ", to: " . $val; }
      if($f == 'subject') { $term_value .= ", subj: " . $val; }
      if($f == 'body') { $term_value .= ", body: " . $val; }
    }
   }

   if(isset($a['from'])) { $term_value .= ", from: " . $a['from'];}
   if(isset($a['to'])) { $term_value .= ", to: " . $a['to'];}
   if(isset($a['subject'])) { $term_value .= ", text: " . $a['subject'];}

   if(isset($a['date1'])) { $term_value .= ", date1: " . $a['date1']; }
   if(isset($a['date2'])) { $term_value .= ", date2: " . $a['date2']; }
   if(isset($a['direction'])) { $term_value .= ", d: " . $a['direction']; }

   if(isset($a['sort'])) { $term_value .= ", sort: " . $a['sort']; }
   if(isset($a['order'])) { $term_value .= ", order: " . $a['order']; }


   $term_value = preg_replace("/^\, /", "", $term_value);

   return $term_value;
}


function fix_email_address($email = '') {
   return preg_replace("/(^\ {0,}|\ {0,}$)/", "", $email);
}


function assemble_search_term($data = array()) {
   $term = "";

   if(isset($data['advanced']) && $data['advanced'] == 1) {
      $term = "advanced=1&date1=" . $data['date1'] . "&date2=" . $data['date2'];

      $term .= "&from=" . $data['from'];
      for($i=2; $i<MAX_NUMBER_OF_FROM_ITEMS; $i++) {
         if(isset($data['from'.$i])) { $term .= "&from$i=" .  $data['from'.$i]; }
      }

      $term .= "&to=" . $data['to'];
      for($i=2; $i<MAX_NUMBER_OF_FROM_ITEMS; $i++) {
         if(isset($data['to'.$i])) { $term .= "&to$i=" .  $data['to'.$i]; }
      }

      $term .= "&subj=" . $data['subj'] . "&body=" . $data['body'] . "&tag=" . $data['tag'];

      if(isset($data['w_attachment'])) { $term .= "&w_attachment=on"; }
      if(isset($data['wo_attachment'])) { $term .= "&wo_attachment=on"; }
   }
   else {
      $term = "date1=" . $data['date1'] . "&date2=" . $data['date2'] . "&subj=" . $data['subj'] . "&from=" . $data['from'] . "&to=" . $data['to'] . "&tag=" . $data['tag'];
      if(strlen($term) < 36) { return ""; }
   }

   return $term;
}


function escape_gt_lt_quote_symbols($s = '') {
   if($s == '') { return $s; }

   $s = preg_replace("/\>/", "&gt;", $s);
   $s = preg_replace("/\</", "&lt;", $s);
   $s = preg_replace('/"/', "&quot;", $s);

   return $s;
}


function get_search_url_by_page($page = 0) {

   return "/search-helper.php?page=$page";

   /*isset($_SERVER['REQUEST_URI']) ? $a = preg_replace("/\/([\w]+)\.php\?{0,1}/", "", $_SERVER['REQUEST_URI']) : "";

   $a = preg_replace("/page=\d{0,100}\&{0,1}/", "", $a);

   if($page <= 0) { return "search.php?$a"; }
   if($page >= 1 && $a == "") { return "search.php?page=$page"; }

   return "search.php?page=$page&$a";*/
}


function parse_string_to_array($s = '', $arr = array()) {
   $a = array();

   parse_str($s, $a);

   while(list($k, $v) = each($a)) {
      if(!isset($arr[$k]) || $arr[$k] == '') $arr[$k] = $v;
   }

}


function fetch_url($url = '') {
   if($url == '') { return ''; }

   $ch = curl_init();

   curl_setopt($ch, CURLOPT_URL, $url);
   curl_setopt($ch, CURLOPT_HEADER, 0);
   curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

   $result = curl_exec($ch);

   curl_close($ch);

   return $result;
}


function fixup_date_condition($field = '', $date1 = 0, $date2 = 0) {
   $date = "";

   if($date1) {
      list($y,$m,$d) = preg_split("/(\.|\-)/", $date1);
      $date1 = mktime(0, 0, 0, $m, $d, $y);

      if($date1 > 0) { $date .= "$field >= $date1 "; }
   }

   if($date2) {
      list($y,$m,$d) = preg_split("/(\.|\-)/", $date2);
      $date2 = mktime(23, 59, 59, $m, $d, $y);

      if($date2 > 0) {
         if($date) { $date .= " AND "; }
         $date .= "$field <= $date2 ";
      }
   }


   return $date;
}


function make_short_string($what, $length) {
   return strlen($what) > $length ? substr($what, 0, $length) . "..." : $what;
}


?>
