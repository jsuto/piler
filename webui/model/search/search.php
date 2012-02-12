<?php

class ModelSearchSearch extends Model {

   public function search_messages($data = array(), $search_type = SIMPLE_SEARCH, $page = 0) {
      $one_page_of_ids = array();
      $total_hits = 0;
      $sort = "sent";
      $order = "DESC";
      $sortorder = "ORDER BY sent DESC";
      $cache_key = "";
      $q = "";
      $s = "";

      while(list($k,$v) = each($data)) {
         if($v) { $s .= '&' . $k . '=' . $v; }
      }

      if($s) { $s = substr($s, 1, strlen($s)); }

      AUDIT(ACTION_SEARCH, '', '', 0, $s);


      if($data['sort'] == "sent") { $sort = "sent"; }
      if($data['sort'] == "size") { $sort = "size"; }
      if($data['sort'] == "from") { $sort = "from"; }
      if($data['sort'] == "subj") { $sort = "subject"; }

      if($data['order'] == 1) { $order = "ASC"; }

      $sortorder = "ORDER BY `$sort` $order";

      $m = array();

      if(MEMCACHED_ENABLED) {
         $cache_key = $this->make_cache_file_name($data, $sortorder);
         $memcache = Registry::get('memcache');
         $m = $memcache->get($cache_key);
      }


      if(isset($m['ids'])) {
         $all_ids = $m['ids'];
      } else {

         if($data['ref']){
            $all_ids = $this->query_all_possible_IDs_by_reference($data['ref'], $cache_key);
         }
         else {
            if($search_type == SIMPLE_SEARCH) {
               $conditions = $this->assemble_simple_query_conditions($data);
            }
            else {
               $conditions = $this->assemble_advanced_query_conditions($data);
            }

            $all_ids = $this->query_all_possible_IDs($data, $conditions, $sort, $order, $sortorder, $cache_key);
         }
      }



      $total_hits = count($all_ids);

      $data['page_len'] = get_page_length();

      if($total_hits > 0) {
         $i = 0;

         foreach($all_ids as $id) {

            if($i >= $data['page_len'] * $page && $i < $data['page_len'] * ($page+1) ) {
               array_push($one_page_of_ids, $id);

               if($q) { $q .= ",?"; } else { $q = "?"; }
            }

            $i++;
         }

      }

      return array($total_hits, $this->get_meta_data($one_page_of_ids, $q, $sortorder));
   }


   private function assemble_advanced_query_conditions($data = array()) {
      $f1 = $f2 = $t1 = $t2 = $fd = $td = '';
      $incoming = $outgoing = '';
      $email = $match = '';
      $n_fc = $n_tc = 0;

      $data['f_from'] = $this->fix_email_address_for_sphinx($data['f_from']);
      $data['o_from'] = $this->fix_email_address_for_sphinx($data['o_from']);
      $data['f_to'] = $this->fix_email_address_for_sphinx($data['f_to']);
      $data['o_to'] = $this->fix_email_address_for_sphinx($data['o_to']);
      $data['from_domain'] = $this->fix_email_address_for_sphinx($data['from_domain']);
      $data['to_domain'] = $this->fix_email_address_for_sphinx($data['to_domain']);

      $data['body'] = $this->fixup_sphinx_operators($data['body']);
      $data['subject'] = $this->fixup_sphinx_operators($data['subject']);


      if(Registry::get('admin_user') == 1 || Registry::get('auditor_user') == 1) {
         if($data['f_from']) { $f1 .= "|" . $data['f_from']; $n_fc++; }
         if($data['o_from']) { $f1 .= "|" . $data['o_from']; $n_fc++; }
         if($data['from_domain']) { $fd .= "(@fromdomain " . substr($data['from_domain'], 1, strlen($data['from_domain'])) . ")"; $n_fc++; }
         if($data['from']) { $f1 .= "|" . $this->fixup_sphinx_operators($data['from']); $n_fc++; }

         if($data['f_to']) { $t1 .= "|" . $data['f_to']; $n_tc++; }
         if($data['o_to']) { $t1 .= "|" . $data['o_to']; $n_tc++; }
         if($data['to_domain']) { $td .= "(@todomain " . substr($data['to_domain'], 1, strlen($data['to_domain'])) . ")"; $n_tc++; }
         if($data['to']) { $f2 .= "|" . $this->fixup_sphinx_operators($data['to']); $n_tc++; }

         if($f1) { $f1 = "(@from " . substr($f1, 1, strlen($f1)) . ")"; }
         if($t1) { $t1 = "(@to " . substr($t1, 1, strlen($t1)) . ")"; }

      }
      else {
         $all_your_addresses = $this->get_all_your_address();

         if($data['f_from']) { $f1 = "(@from " . $data['f_from'] . " @to $all_your_addresses)"; $n_fc++; }
         if($data['o_from']) { $f2 = "(@from " . $data['o_from'] . ")"; $n_fc++; }
         if($data['from_domain']) { $fd = "(@fromdomain " . substr($data['from_domain'], 1, strlen($data['from_domain'])) . " @to $all_your_addresses)"; $n_fc++; }
         if($data['from']) { $fd = "(@from " . $this->fixup_sphinx_operators($data['from']) . " @to $all_your_addresses)"; $n_fc++; }

         if($data['f_to']) { $t1 = "(@to " . $data['f_to'] . " @from $all_your_addresses)"; $n_tc++; }
         if($data['o_to']) { $t2 = "(@to " . $data['o_to'] . ")"; $n_tc++; }
         if($data['to_domain']) { $td = "(@todomain " . substr($data['to_domain'], 1, strlen($data['to_domain'])) . " @from $all_your_addresses)"; $n_tc++; }
         if($data['to']) { $fd = "(@to " . $this->fixup_sphinx_operators($data['to']) . " @from $all_your_addresses)"; $n_tc++; }

         if($n_fc == 0 && $n_tc == 0 && $data['from_domain'] == '' && $data['to_domain'] == '') {
            if($data['direction'] == 2) {
               $f1 = " (@from " . $all_your_addresses . ")";
            } else {
               $t1 = " (@to " . $all_your_addresses . ")";
            }
         }
      }

      if($f1) { $incoming .= "|$f1"; }
      if($f2) { $incoming .= "|$f2"; }
      if($fd) { $incoming .= "|$fd"; }

      if($t1) { $outgoing .= "|$t1"; }
      if($t2) { $outgoing .= "|$t2"; }
      if($td) { $outgoing .= "|$td"; }

      if($incoming) { $incoming = substr($incoming, 1, strlen($incoming)); if($n_fc > 1) { $incoming = "($incoming)"; } }
      if($outgoing) { $outgoing = substr($outgoing, 1, strlen($outgoing)); if($n_tc > 1) { $outgoing = "($outgoing)"; } }


      if($incoming) {
         $email = $incoming;
         if($outgoing) { $email = $incoming . " & " . $outgoing; }
      } else if($outgoing) {
         $email = $outgoing;
      }


      if($email) { $match = $email; }

      if($data['body']) { if($match) { $match .= " & "; } $match .= "(@body " . $data['body'] . ") "; }
      if($data['subject']) { if($match) { $match .= " & "; } $match .= "(@subject " . $data['subject'] . ") "; }
      if($data['attachment_type'] && $data['attachment_type'] != "any") { if($match) { $match .= " & "; } $match .= "(@attachment_types " . $data['attachment_type'] . ") "; }

      return $match;
   }


   private function assemble_simple_query_conditions($data = array(), $sort = 'sent', $order = 'DESC', $sortorder = '', $cache_key = '') {
      $email = $match = "";

      if(Registry::get('admin_user') == 0 && Registry::get('auditor_user') == 0) {

         $all_your_addresses = $this->get_all_your_address();

         if(isset($data['from'])) { $data['from'] = fix_email_address($data['from']); }
         if(isset($data['to'])) { $data['to'] = fix_email_address($data['to']); }

         // missing From: address

         if(!isset($data['from'])) {

            if(isset($data['to']) && substr($data['to'], 0, 1) == '@') {
               $email = "@from $all_your_addresses @todomain " . $this->fix_email_address_for_sphinx(substr($data['to'], 1, strlen($data['to'])));
            }
            else if(isset($data['to']) && !strstr($data['to'], '@')) { $email = "@from $all_your_addresses @to " . $this->fixup_sphinx_operators($this->fix_email_address_for_sphinx($data['to'])); }
            else if(!isset($data['to'])) { $email = "@to $all_your_addresses"; }
            else if(!in_array($data['to'], $_SESSION['emails'])) { $email = "@from $all_your_addresses @to " . $this->fix_email_address_for_sphinx($data['to']); }
            else { $email = "@to " . $this->fix_email_address_for_sphinx($data['to']); }

         }

         // missing To: address

         else if(!isset($data['to'])) {
            if(isset($data['from']) && substr($data['from'], 0, 1) == '@') {
               $email = "@to $all_your_addresses @fromdomain " . $this->fix_email_address_for_sphinx(substr($data['from'], 1, strlen($data['from'])));
            }
            else if(isset($data['from']) && !strstr($data['from'], '@')) { $email = "@to $all_your_addresses @from " . $this->fixup_sphinx_operators($this->fix_email_address_for_sphinx($data['from'])); }
            else if(!in_array($data['from'], $_SESSION['emails'])) { $email = "@to $all_your_addresses @from " . $this->fix_email_address_for_sphinx($data['from']); }
            else { $email = "@from " . $this->fix_email_address_for_sphinx($data['from']); }
         }

         else if(isset($data['from']) && isset($data['to'])) {

            if(
               (!in_array($data['from'], $_SESSION['emails']) && in_array($data['to'], $_SESSION['emails'])) ||
               (!in_array($data['to'], $_SESSION['emails']) && in_array($data['from'], $_SESSION['emails']))
            ) {
               $email = "@from " . $this->fix_email_address_for_sphinx($data['from']) . " @to " . $this->fix_email_address_for_sphinx($data['to']);
            }

            else {
               $email = " @to INVALID ";
            }
         }

      }
      else {
         if(isset($data['from'])) {
            if(substr($data['from'], 0, 1) == '@') { $match .= " @fromdomain " . $this->fix_email_address_for_sphinx(substr($data['from'], 1, strlen($data['from']))); }
            else { $match .= " @from " . $this->fixup_sphinx_operators($this->fix_email_address_for_sphinx($data['from'])); }
         }

         if(isset($data['to'])) {
            if(substr($data['to'], 0, 1) == '@') { $match .= " @todomain " . $this->fix_email_address_for_sphinx(substr($data['to'], 1, strlen($data['to']))); }
            else { $match .= " @to " . $this->fixup_sphinx_operators($this->fix_email_address_for_sphinx($data['to'])); }
         }

      }



      if(isset($data['subject'])) {
         $data['subject'] = $this->fixup_sphinx_operators($data['subject']);
         $match .= " @(subject,body) " . $data['subject'];
      }


      if($email) { $match = " $email " . $match; }

      return $match;
   }


   private function query_all_possible_IDs($data = array(), $conditions = '', $sort = 'sent', $order = 'DESC', $sortorder = '', $cache_key = '') {
      $ids = array();
      $direction = $size = '';
      $tag_id_list = '';
      $a = "";

      if($data['sort'] == 'from' || $data['sort'] == 'subj') { $sortorder = ''; }

      $date = fixup_date_condition('sent', $data['date1'], $data['date2']);

      if($date) { $date .= " AND "; }

      if(isset($data['direction']) && $data['direction'] != '') { $direction = "direction = " . $data['direction'] . " AND "; }

      if(isset($data['size']) && $data['size']) {
         $data['size'] = preg_replace("/\s/", "", $data['size']);
         if(preg_match("/^(\>|\<)\={0,}\d{1,}$/", $data['size'])) { $size = "size " . $data['size'] . " AND "; }
      }

      if(isset($data['attachment_type']) && $data['attachment_type'] == 'any') { $a = "attachments > 0 AND "; }

      if($data['tag']) {
         $data['tag'] = $this->fixup_sphinx_operators($data['tag']);

         $aa = $this->sphx->query("SELECT id FROM " . SPHINX_TAG_INDEX . " WHERE uid=" . $_SESSION['uid'] . " AND MATCH('@tag " . $data['tag'] . " ') ");

         foreach($aa->rows as $a) {
            $tag_id_list .= "," . $a['id'];
         }

         $query = $this->sphx->query("SELECT id FROM " . SPHINX_MAIN_INDEX . " WHERE id IN (" . substr($tag_id_list, 1, strlen($tag_id_list)) . ") $sortorder LIMIT 0," . MAX_SEARCH_HITS);
      }
      else {
         $query = $this->sphx->query("SELECT id FROM " . SPHINX_MAIN_INDEX . " WHERE $a $date $direction $size MATCH('$conditions') $sortorder LIMIT 0," . MAX_SEARCH_HITS);
      }

//print $query->query; print "<p>" . $query->exec_time . "</p>\n";


      /*
       * build an id list
       */

      $q = "";

      foreach($query->rows as $a) {
         array_push($ids, $a['id']);

         if($q) { $q .= ",?"; }
         else { $q = "?"; }
      }


      /*
       * if the query was requested to be sorted by sender or subject, then sphinx cannot do
       * that, so we assemble the list of all sphinx IDs matching the query
       */

      if($data['sort'] == 'from' || $data['sort'] == 'subj') {

         $query = $this->db->query("SELECT id FROM " . TABLE_META . " WHERE id IN ($q) ORDER BY `$sort` $order", $ids);

         $ids = array();

         foreach($query->rows as $q) {
            array_push($ids, $q['id']);
         }

      }


      if(MEMCACHED_ENABLED && $cache_key) {
         $memcache = Registry::get('memcache');
         $memcache->add($cache_key, array('ts' => time(), 'total_hits' => count($ids), 'ids' => $ids), 0, MEMCACHED_TTL);
      }


      return $ids;
   }


   private function query_all_possible_IDs_by_reference($reference = '', $cache_key = '') {
      $ids = array();

      if($reference == '') { return $ids; }

      $query = $this->db->query("SELECT id FROM " . TABLE_META . " WHERE message_id=? OR reference=? ORDER BY id DESC", array($reference, $reference));

      foreach($query->rows as $q) {
         array_push($ids, $q['id']);
      }

      if(MEMCACHED_ENABLED && $cache_key) {
         $memcache = Registry::get('memcache');
         $memcache->add($cache_key, array('ts' => time(), 'total_hits' => count($ids), 'ids' => $ids), 0, MEMCACHED_TTL);
      }

      return $ids;
   }


   private function get_meta_data($ids = array(), $q = '', $sortorder = '') {
      $messages = array();
      $tag = array();


      if(count($ids) == 0) return $messages;

      $query = $this->db->query("SELECT `id`, `from`, `subject`, `piler_id`, `reference`, `size`, `spam`, `sent`, `arrived`, `attachments` FROM `" . TABLE_META . "` WHERE `id` IN ($q) $sortorder", $ids);


      if(isset($query->rows)) {

         $tags = $this->db->query("SELECT `id`, `tag` FROM `" . TABLE_TAG . "` WHERE `id` IN ($q)", $ids);
         foreach ($tags->rows as $t) {
            $tag[$t['id']] = $t['tag'];
         }


         $lang = Registry::get('language');

         foreach($query->rows as $m) {
            $m['shortfrom'] = make_short_string($m['from'], MAX_CGI_FROM_SUBJ_LEN);

            if($m['subject'] == "") { $m['subject'] = "&lt;" . $lang->data['text_no_subject'] . "&gt;"; }

            $m['subject'] = escape_gt_lt_quote_symbols($m['subject']);
            $m['shortsubject'] = make_short_string($m['subject'], MAX_CGI_FROM_SUBJ_LEN);

            $m['date'] = date(SEARCH_HIT_DATE_FORMAT, $m['sent']);
            $m['size'] = nice_size($m['size']);

            /*
             * verifying 20 messages takes some time, still it's useful
             */

            if(ENABLE_ON_THE_FLY_VERIFICATION == 1) {
               $m['verification'] = $this->model_search_message->verify_message($m['piler_id']);
            }

            if(isset($tag[$m['id']])) { $m['tag'] = $tag[$m['id']]; } else { $m['tag'] = ''; }

            array_push($messages, $m);
         }
      }

      return $messages;
   }


   public function get_message_recipients($id = '') {
      $rcpt = array();

      if(Registry::get('admin_user') == 0 && Registry::get('auditor_user') == 0) { return $rcpt; }

      $query = $this->db->query("SELECT `to` FROM " . VIEW_MESSAGES . " WHERE piler_id=?", array($id));

      foreach($query->rows as $q) {
         array_push($rcpt, $q['to']);
      }

      return $rcpt;
   }


   private function get_all_your_address() {
      $s = '';

      while(list($k, $v) = each($_SESSION['emails'])) {
         if($s) { $s .= '| ' .  $this->fix_email_address_for_sphinx($v); }
         else { $s = $this->fix_email_address_for_sphinx($v); }
      }

      return $s;
   }


   public function check_your_permission_by_piler_id($id = '') {
      $q = '';
      $arr = $a = array();

      if($id == '') { return 0; }

      if(Registry::get('admin_user') == 1 || Registry::get('auditor_user') == 1) { return 1; }

      array_push($arr, $id);

      while(list($k, $v) = each($_SESSION['emails'])) {
         if(validemail($v) == 1) {
            $q .= ",?";
            array_push($a, $v);
         }
      }

      $q = preg_replace("/^\,/", "", $q);

      $arr = array_merge($arr, $a, $a);

      $query = $this->db->query("SELECT * FROM " . VIEW_MESSAGES . " WHERE piler_id=? AND ( `from` IN ($q) OR `to` IN ($q) )", $arr);

      if(isset($query->row['id'])) { return 1; }

      return 0;
   }


   public function remove_message($id = '') {
      if($id == '') { return 0; }

      if(Registry::get('admin_user') == 0) { return 0; }

      $query = $this->db->query("UPDATE " . TABLE_META . " SET deleted=1 WHERE piler_id=?", array($id));

      return $this->db->countAffected();
   }


   private function fix_email_address_for_sphinx($email = '') {
      return preg_replace("/[\@\.\+\-]/", "X", $email);
   }


   public function get_search_terms() {

      $query = $this->db->query("SELECT term FROM " . TABLE_SEARCH . " where email=? ORDER BY ts DESC", array($_SESSION['email']));
      if(isset($query->rows)) { return $query->rows; }

      return array();
   }


   public function add_search_term($term = '') {
      if($term == '') { return 0; }

      if($this->update_search_term($term) == 0) {
         AUDIT(ACTION_SAVE_SEARCH, '', '', '', $term);
         $query = $this->db->query("INSERT INTO " . TABLE_SEARCH . " (email, ts, term) VALUES(?,?,?)", array($_SESSION['email'], time(), $term));
      }

      return 1;
   }


   public function update_search_term($term = '') {
      if($term == '') { return 0; }

      AUDIT(ACTION_SEARCH, '', '', '', $term);

      $query = $this->db->query("UPDATE " . TABLE_SEARCH . " SET ts=? WHERE term=? AND email=?", array(time(), $term, $_SESSION['email']));

      return $this->db->countAffected();
   }


   private function fixup_sphinx_operators($s = '') {
      if($s == '') { return $s; }

      $s = preg_replace("/ OR /", "|", $s);
      $s = preg_replace("/(\-)/", " ", $s);
      $a = explode(" ", $s);
      $s = '';

      while(list($k, $v) = each($a)) {

         if(substr($v, 0, 4) == 'http') {
            $v = preg_replace("/http(s){0,1}\:\/\//", "__URL__", $v);
            $b = explode("/", $v);
            $s .= ' ' . $this->fix_email_address_for_sphinx($b[0]);
         }
         else {
            $s .= ' ' . $v;
         }
      }

      return $s;
   }


   private function make_cache_file_name($data = array(), $sortorder = '') {
      return sha1($_SESSION['email'] . "/" . join("*", $data) . "-" . (NOW - NOW % 3600) . "-" . $sortorder);
   }

}


?>
