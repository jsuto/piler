<?php

function H($s = '') {
   print htmlentities($s);
}


function LOGGER($event = '', $username = '') {
   $ipaddr = '';

   if($event == "") { return 0; }

   if(isset($_SERVER['REMOTE_ADDR'])) { $ipaddr = $_SERVER['REMOTE_ADDR']; }

   $session = Registry::get('session');

   if($username == '') {
      if($session->get("username")) { $username = $session->get("username"); }
      else { $username = 'unknown'; }
   }

   syslog(LOG_INFO, "username=$username, event='$event', ipaddr=$ipaddr");
}


function AUDIT($action = 0, $email = '', $ipaddr = '', $id = 0, $description = '') {

   if(ENABLE_AUDIT == 0) { return 0; }

   $session = Registry::get('session');

   if($ipaddr == '' && isset($_SERVER['REMOTE_ADDR'])) { $ipaddr = $_SERVER['REMOTE_ADDR']; }
   if($email == '') { $email = $session->get("email"); }

   $a = explode("@", $email);

   $sphxrw = Registry::get('sphxrw');

   $description = htmlspecialchars($description);

   $query = $sphxrw->query("INSERT INTO " . SPHINX_AUDIT_INDEX . " (ts, email, action, ipaddr, meta_id, description) VALUES(?,?,?,?,?,?)", array(time(), $email, $action, $ipaddr, $id, $description));

   return $sphxrw->countAffected();
}


function getAuthenticatedUsername() {
   $session = Registry::get('session');

   if($session->get("username")) { return $session->get("username"); }

   return "";
}


function isAdminUser() {
   $session = Registry::get('session');

   if($session->get("admin_user") == 1){ return 1; }

   return 0;
}


function isAuditorUser() {
   $session = Registry::get('session');

   if(ADMIN_CAN_POWER_SEARCH == 1 && Registry::get('admin_user') == 1) { return 1; }


   if($session->get("admin_user") == 2){ return 1; }

   return 0;
}


function isReadonlyAdmin() {
   $session = Registry::get('session');

   if($session->get("admin_user") == 3){ return 1; }

   return 0;
}

function isDataOfficer() {
   $session = Registry::get('session');

   if($session->get("admin_user") == 4){ return 1; }

   return 0;
}


function logout() {
   $session = Registry::get('session');

   AUDIT(ACTION_LOGOUT, $session->get("email"), '', '', '');

   $session->set("username", "");
   $session->set("admin_user", 0);

   $session->remove("username");
   $session->remove("admin_user");

   Registry::set('username', '');

   session_destroy();
}


function isBinary($num = '') {
   if($num == 0 || $num == 1){ return 1; }

   return 0;
}


function get_page_length() {
   $page_len = PAGE_LEN;
   $session = Registry::get('session');

   if($session->get("pagelen") && is_numeric($session->get("pagelen")) && $session->get("pagelen") >= 10 && $session->get("pagelen") <= MAX_SEARCH_HITS) {
      $page_len = $session->get("pagelen");
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
   if($email == '' || !strchr($email, '@')) { return 0; }

   $arr = explode("@", $email);

   // This is a pretty relaxed formula making sure we have something as the local part
   if(count($arr) == 2 && strlen($arr[0]) >= 1 && validdomain($arr[1])) { return 1; }

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
   if(preg_match("/@?local$/", $domain) || preg_match(DOMAIN_REGEX, $domain)) {
      return 1;
   }

   return 0;
}


function first_n_characters($what, $n){
   $x = "";
   $len = 0;

   $a = explode(" ", $what);
   foreach($a as $k => $v) {
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
      foreach($a as $k => $v) {
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


function format_number($n) {
   return number_format($n, 0, DECIMAL_SEPARATOR, THOUSANDS_SEPARATOR);
}


function nice_size($size = 0, $space = '') {
   if($size < 1000) return "1k";
   if($size < 1000000) return round($size/1000) . $space . "k";
   if($size < 1000000000) return round($size/1000000) . $space . "M";

   return sprintf("%.1f", $size/1000000000) . $space . "G";
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

   foreach($a as $k => $v) {
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


function convert_date_string_to_ymd_by_template($date_string, $date_template) {
   $Y = $m = $d = 0;

   $s = $template_array = preg_split("/(\.|\-|\/)/", $date_template);
   sort($s);

   $date_array = preg_split("/(\.|\-|\/)/", $date_string);

   if($s != ['Y','d','m'] || count($template_array) != 3 || count($date_array) != 3) {
      return [$Y, $m, $d];
   }

   foreach($template_array as $k => $v) {
      $$v = $date_array[$k];
   }

   return [$Y, $m, $d];
}


function fixup_date_condition($field = '', $date1 = 0, $date2 = 0) {
   global $session;

   $date = "";

   // Check if we want to apply a mandatory date1 (=not before) restriction
   // to a non-auditor user
   if(Registry::get('auditor') == 0 && $session->get('not_before_date')) {
      $date1 = $session->get('not_before_date');
   }


   if($date1) {
      list($y,$m,$d) = convert_date_string_to_ymd_by_template($date1, DATE_TEMPLATE);

      $date1 = mktime(0, 0, 0, $m, $d, $y);

      if($date1 > 0) { $date .= "$field >= $date1 "; }
   }

   if($date2) {
      list($y,$m,$d) = convert_date_string_to_ymd_by_template($date2, DATE_TEMPLATE);

      $date2 = mktime(23, 59, 59, $m, $d, $y);

      if($date2 > 0) {
         if($date) { $date .= " AND "; }
         $date .= "$field <= $date2 ";
      }
   }


   return $date;
}


function make_short_string($what, $length) {
   if($length < 1) { return ''; }

   if(strlen($what) <= $length) { return $what; }

   $arr = preg_split("/\s/", $what);
   $s = '';

   $i = 0;
   foreach($arr as $a) {
      if($i == 0) {
         if($length > 0 && strlen($a) > $length) {
            return substr($a, 0, $length) . '...';
         }
      }

      if(strlen($s) + strlen($a) <= $length) {
         $s .= $a . ' ';
      } else {
         break;
      }

      $i++;
   }

   return $s . '...';
}


function convert_days_ymd($convert) {
   $years = ($convert / 365) ; // days / 365 days
   $years = floor($years); // Remove all decimals

   $month = ($convert % 365) / 30.5; // I choose 30.5 for Month (30,31) ;)
   $month = floor($month); // Remove all decimals

   $days = ($convert % 365) % 30.5; // the rest of days

   // Return array of years, months, days
   return array( $years,$month,$days );
}


function fix_evolution_mime_name_crap($s = '') {
   if(preg_match("/iso\-\d{1,4}\-\d{1,}\'\'/i", $s)) {
      $s = preg_replace("/iso\-\d{1,4}\-\d{1,}\'\'/i", "", $s);
      $s = utf8_encode(urldecode($s));
   }

   return $s;
}


function safe_feof($fp, &$start = NULL) {
   $start = microtime(true);
   return feof($fp);
}


function anonimize_ip_addr($ip = '') {
   $ip = explode(".", $ip);
   if(count($ip) == 4) {
      $ip[0] = $ip[1] = 'x';

      $ip = implode(".", $ip);
   }

   return $ip;
}


function get_ldap_attribute_names($ldap_type = '') {

   $ldap_mail_attr = LDAP_MAIL_ATTR;
   $ldap_account_objectclass = LDAP_ACCOUNT_OBJECTCLASS;
   $ldap_distributionlist_attr = LDAP_DISTRIBUTIONLIST_ATTR;
   $ldap_distributionlist_objectclass = LDAP_DISTRIBUTIONLIST_OBJECTCLASS;

   switch ($ldap_type) {

            case 'AD':
                       $ldap_mail_attr = 'proxyAddresses';
                       $ldap_account_objectclass = 'user';
                       $ldap_distributionlist_attr = 'member';
                       $ldap_distributionlist_objectclass = 'group';
                       break;

            case 'zimbra':
                       $ldap_mail_attr = 'mail';
                       $ldap_account_objectclass = 'zimbraAccount';
                       $ldap_distributionlist_attr = 'zimbraMailForwardingAddress';
                       $ldap_distributionlist_objectclass = 'zimbraDistributionList';
                       break;

            case 'iredmail':
                       $ldap_mail_attr = 'mail';
                       $ldap_account_objectclass = 'mailUser';
                       $ldap_distributionlist_attr = 'memberOfGroup';
                       $ldap_distributionlist_objectclass = 'mailList';
                       break;

            case 'lotus':
                       $ldap_mail_attr = 'mail';
                       $ldap_account_objectclass = 'dominoPerson';
                       $ldap_distributionlist_attr = 'mail';
                       $ldap_distributionlist_objectclass = 'dominoGroup';
                       break;


   }

   return array($ldap_mail_attr, $ldap_account_objectclass, $ldap_distributionlist_attr, $ldap_distributionlist_objectclass);
}


function htmlentities_on_array($arr = []) {
   foreach($arr as $k => $v) {
      if(is_array($v)) {
         $arr[$k] = htmlentities_on_array($v);
      } else {
         $arr[$k] = htmlentities($v);
      }
   }

   return $arr;
}


function encrypt_password($password = '') {
   return crypt($password, '$6$rounds=5000$' . generate_random_string() . '$');
}


function get_q_string($arr = []) {
   $q = str_repeat("?,", count($arr));
   $q = substr($q, 0, strlen($q)-1);

   return $q;
}
