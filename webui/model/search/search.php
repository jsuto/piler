<?php

class ModelSearchSearch extends Model {

   public function search_messages($data = array(), $page = 0) {
      $one_page_of_ids = array();
      $total_hits = 0;
      $sort = "sent";
      $order = "DESC";
      $sortorder = "ORDER BY sent DESC";
      $cache_key = "";
      $q = "";
      $s = "";
      $all_ids_csv = "";

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

         if(isset($data['ref']) && $data['ref']){
            $all_ids = $this->query_all_possible_IDs_by_reference($data['ref'], $cache_key);
         }
         else {
            $all_ids = $this->query_all_possible_IDs($data, $sort, $order, $sortorder, $cache_key);
         }
      }


      $total_hits = count($all_ids);


      $data['page_len'] = get_page_length();

      if($total_hits > 0) {
         $i = 0;

         foreach($all_ids as $id) {

            if($i >= $data['page_len'] * $page && $i < $data['page_len'] * ($page+1) ) {
               array_push($one_page_of_ids, $id);
               $all_ids_csv .= ",$id";

               if($q) { $q .= ",?"; } else { $q = "?"; }
            }

            $i++;
         }

      }

      $all_ids_csv = substr($all_ids_csv, 1, strlen($all_ids_csv));


      return array($total_hits, $all_ids_csv, $this->get_meta_data($one_page_of_ids, $q, $sortorder));
   }


   private function assemble_email_address_condition($from = '', $to = '') {
      $s = '';
      $f_from = $f_fromdomain = $f_to = $f_todomain = '';
      $o_from = $o_fromdomain = $o_to = $o_todomain = '';
      $f_f = $o_f = $f_t = $o_t = '';
      $f = $t = $fdomain = $tdomain = '';

      $session_emails = $this->fix_email_address_for_sphinx($_SESSION['emails']);
      $session_domains = $this->fix_email_address_for_sphinx($_SESSION['auditdomains']);

      $all_your_addresses = $this->get_all_your_address();

      $from = preg_replace("/OR/", "", $from);
      $to = preg_replace("/OR/", "", $to);

      if($from) {
         $e = preg_split("/\s/", $from);
         foreach ($e as $email) {
            if($email == '') { continue; }

            $email = $this->fix_email_address_for_sphinx($email);

            $field = 'from';
            if($email[0] == 'X') {
               $email = substr($email, 1, strlen($email));

               $field = 'fromdomain';
               $fdomain .= "|$email";
            }
            else {
               $f .= "|$email";
            }


            if(in_array($email, $session_emails)) {
               $a = "o_$field";
            }
            else {
               $a = "f_$field";
            }

            if($$a) { $$a .= "|"; }
            $$a .= "$email";
         }
      }

      if($to) {
         $e = preg_split("/\s/", $to);
         foreach ($e as $email) {
            if($email == '') { continue; }

            $email = $this->fix_email_address_for_sphinx($email);

            $field = 'to';
            if($email[0] == 'X') {
               $email = substr($email, 1, strlen($email));
               $field = 'todomain';
               $tdomain .= "|$email";
            }
            else {
               $t .= "|$email";
            }

            if(in_array($email, $session_emails)) {
               $a = "o_$field";
            }
            else {
               $a = "f_$field";
            }

            if($$a) { $$a .= "|"; }
            $$a .= "$email";
         }
      }


      if($f) { $f = preg_replace("/^\|/", "@from ", $f); }

      if($fdomain) {
         $fdomain = preg_replace("/^\|/", "@fromdomain ", $fdomain);
         if($f) { $f = "(($f)|($fdomain))"; }
         else { $f = "($fdomain)"; }
      }

      if($t) { $t = preg_replace("/^\|/", "@to ", $t); }

      if($tdomain) {
         $tdomain = preg_replace("/^\|/", "@todomain ", $tdomain);
         if($t) { $t = "(($t)|($tdomain))"; }
         else { $t = "($tdomain)"; }
      }


      if(Registry::get('auditor_user') == 1 || ENABLE_FOLDER_RESTRICTIONS == 1) {
         $domain_restrictions = '';
         $sd = '';

         foreach ($session_domains as $d) {
            $sd .= '|'.$d;
         }
         $sd = preg_replace("/^\|/", "", $sd);


         if(RESTRICTED_AUDITOR == 1) {
            $domain_restrictions = ' (@todomain ' . $sd . ' | @fromdomain '  . $sd . ')';
         }

         if($from == '' && $to == '') { return $domain_restrictions; }

         if(RESTRICTED_AUDITOR == 1) {
            $domain_restrictions = " & $domain_restrictions";
         }

         if($f && $t) { return "($f & $t) $domain_restrictions"; }
         else if($f) { return "($f) $domain_restrictions"; }
         else if($t) { return "($t) $domain_restrictions"; }
      }


      if($f_from) { $f_f = "@from $f_from"; }
      if($f_fromdomain) { if($f_f) { $f_f = "($f_f | @fromdomain $f_fromdomain)"; } else { $f_f = "@fromdomain $f_fromdomain"; } }

      if($o_from) { $o_f = "@from $o_from"; }

      if($f_to) { $f_t = "@to $f_to"; }
      if($f_todomain) { if($f_t) { $f_t = "($f_t | @todomain $f_todomain)"; } else { $f_t = "@todomain $f_todomain"; } }

      if($o_to) { $o_t = "@to $o_to"; }


      if($f_f == '' && $o_f == '' && $f_t == '' && $o_t == '') { return "@to $all_your_addresses"; }
      if($f_f == '' && $o_f == '' && $f_t == '' && $o_t      ) { return "$o_t"; }
      if($f_f == '' && $o_f == '' && $f_t       && $o_t == '') { return "(@from $all_your_addresses & $f_t)"; }
      if($f_f == '' && $o_f == '' && $f_t       && $o_t      ) { return "($o_t | (@from $all_your_addresses & $f_t))"; }

      if($f_f == '' && $o_f       && $f_t == '' && $o_t == '') { return "$o_f"; }
      if($f_f == '' && $o_f       && $f_t == '' && $o_t      ) { return "($o_f & $o_t)"; }
      if($f_f == '' && $o_f       && $f_t       && $o_t == '') { return "($o_f & $f_t)"; }
      if($f_f == '' && $o_f       && $f_t       && $o_t      ) { return "(($o_f & $f_t) | ($o_f & $o_t))"; }

      if($f_f       && $o_f == '' && $f_t == '' && $o_t == '') { return "($f_f & @to $all_your_addresses)"; }
      if($f_f       && $o_f == '' && $f_t == '' && $o_t      ) { return "($f_f & $o_t)"; }
      if($f_f       && $o_f == '' && $f_t       && $o_t == '') { return "@from INVALID"; }
      if($f_f       && $o_f == '' && $f_t       && $o_t      ) { return "($f_f & $o_t)"; }

      if($f_f       && $o_f       && $f_t == '' && $o_t == '') { return "(($f_f & @to $all_your_addresses)|$o_f)"; }
      if($f_f       && $o_f       && $f_t == '' && $o_t      ) { return "(($f_f & $o_t)|($o_f & $o_t))"; }
      if($f_f       && $o_f       && $f_t       && $o_t == '') { return "($o_f & $f_t)"; }
      if($f_f       && $o_f       && $f_t       && $o_t      ) { return "(($f_f & $o_t)|($o_f & $f_t))"; }


      return "(@from $all_your_addresses | @to $all_your_addresses)";

   }


   private function query_all_possible_IDs($data = array(), $sort = 'sent', $order = 'DESC', $sortorder = '', $cache_key = '') {
      $ids = array();
      $__folders = array();
      $match = '';
      $direction = $attachment = $size = $folders = '';
      $tag_id_list = '';
      $a = "";


      $match = $this->assemble_email_address_condition($data['from'], $data['to']);

      if($data['body']) {
         $data['body'] = $this->fixup_meta_characters($data['body']);
         $data['body'] = $this->fixup_sphinx_operators($data['body']);
         if($match) { $match .= " & "; } $match .= "(@body " . $data['body'] . ") ";
      }

      if($data['subject']) {
         $data['subject'] = $this->fixup_meta_characters($data['subject']);
         $data['subject'] = $this->fixup_sphinx_operators($data['subject']);
         if($match) { $match .= " & "; } $match .= "(@subject " . $data['subject'] . ") ";
      }

      if($data['attachment_type'] && $data['attachment_type'] != "any") { if($match) { $match .= " & "; } $match .= "(@attachment_types " . $data['attachment_type'] . ") "; }


      if($data['any']) {
         $data['any'] = $this->fixup_meta_characters($data['any']);
         $data['any'] = $this->fixup_sphinx_operators($data['any']);
         $data['any'] = $this->fix_email_address_for_sphinx($data['any']);
         $fields = '';
         if($match) { $match = "($match) & "; $fields = '@subject,@body '; } $match .= "($fields " . $data['any'] . ") ";
      }



      if($data['sort'] == 'from' || $data['sort'] == 'subj') { $sortorder = ''; }

      $date = fixup_date_condition('sent', $data['date1'], $data['date2']);

      if($date) { $date .= " AND "; }

      if(isset($data['direction']) && $data['direction'] != '') { $direction = "direction = " . $data['direction'] . " AND "; }

      if(isset($data['size']) && $data['size']) {
         $data['size'] = preg_replace("/\s/", "", $data['size']);
         if(preg_match("/^(\>|\<)\={0,}\d{1,}$/", $data['size'])) { $size = "size " . $data['size'] . " AND "; }
      }

      if(isset($data['attachment_type']) && $data['attachment_type'] == 'any') { $a = "attachments > 0 AND "; }
      else if(isset($data['has_attachment']) && $data['has_attachment'] == 1) { $attachment = "attachments > 0 AND "; }


      if(ENABLE_FOLDER_RESTRICTIONS == 1) {
         $s = explode(" ", $data['folders']);
         while(list($k,$v) = each($s)) {
            if(in_array($v, $_SESSION['folders'])) {
               array_push($__folders, $v);
            }
         }

         if(count($__folders) > 0) {
            $folders = "folder IN (" . implode(",", $__folders) . ") AND ";
         }

      }


      if(isset($data['tag']) && $data['tag']) {
         $id_list = $this->get_sphinx_id_list($data['tag'], SPHINX_TAG_INDEX, 'tag');
         $query = $this->sphx->query("SELECT id FROM " . SPHINX_MAIN_INDEX . " WHERE $folders id IN ($id_list) $sortorder LIMIT 0," . MAX_SEARCH_HITS);
      }
      else if(isset($data['note']) && $data['note']) {
         $id_list = $this->get_sphinx_id_list($data['note'], SPHINX_NOTE_INDEX, 'note');
         $query = $this->sphx->query("SELECT id FROM " . SPHINX_MAIN_INDEX . " WHERE $folders id IN ($id_list) $sortorder LIMIT 0," . MAX_SEARCH_HITS);
      }
      else if(ENABLE_FOLDER_RESTRICTIONS == 1 && isset($data['extra_folders']) && $data['extra_folders']) {
         $ids_in_extra_folders = $this->get_sphinx_id_list_by_extra_folders($data['extra_folders']);
         $query = $this->sphx->query("SELECT id FROM " . SPHINX_MAIN_INDEX . " WHERE $a $date $attachment $direction $size MATCH('$match') AND id IN ($ids_in_extra_folders) $sortorder LIMIT 0," . MAX_SEARCH_HITS);
      }
      else {
         $query = $this->sphx->query("SELECT id FROM " . SPHINX_MAIN_INDEX . " WHERE $a $date $attachment $direction $size $folders MATCH('$match') $sortorder LIMIT 0," . MAX_SEARCH_HITS);
      }

      if(ENABLE_SYSLOG == 1) { syslog(LOG_INFO, sprintf("sphinx query: '%s' in %.2f s, %d hits", $query->query, $query->exec_time, $query->num_rows)); }


      /*
       * build an id list
       */

      $q = "";

      if(isset($query->rows)) {
         foreach($query->rows as $a) {
            array_push($ids, $a['id']);

            if($q) { $q .= ",?"; }
            else { $q = "?"; }
         }
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

      if(ENABLE_FOLDER_RESTRICTIONS == 1) {
         $query = $this->sphx->query("SELECT id, folder FROM " . SPHINX_MAIN_INDEX . " WHERE id IN (" . implode(",", $ids) . ")");
         $ids = array();
         foreach($query->rows as $q) {
            if(isset($q['folder']) && in_array($q['folder'], $_SESSION['folders'])) { array_push($ids, $q['id']); }
         }
      }


      if(MEMCACHED_ENABLED && $cache_key) {
         $memcache = Registry::get('memcache');
         $memcache->add($cache_key, array('ts' => time(), 'total_hits' => count($ids), 'ids' => $ids), 0, MEMCACHED_TTL);
      }

      return $ids;
   }


   private function get_sphinx_id_list($s = '', $sphx_table = '', $field = '') {
      $id_list = '';

      $s = $this->fixup_sphinx_operators($s);

      $q = $this->sphx->query("SELECT id FROM $sphx_table WHERE uid=" . $_SESSION['uid'] . " AND MATCH('@$field $s') ");

      foreach($q->rows as $a) {
         $id_list .= "," . $a['id'];
      }

      if($id_list) { $id_list = substr($id_list, 1, strlen($id_list)); }

      return $id_list;
   }


   private function get_sphinx_id_list_by_extra_folders($extra_folders = '') {
      $id_list = '';
      $q = '';
      $__folders = array();

      $s = explode(" ", $extra_folders);
      while(list($k,$v) = each($s)) {
         if(in_array($v, $_SESSION['extra_folders']) && is_numeric($v)) {
            array_push($__folders, $v);
            if($q) { $q .= ",?"; }
            else { $q = "?"; }
         }
      }


      $q = $this->db->query("SELECT id FROM " . TABLE_FOLDER_MESSAGE . " WHERE folder_id IN ($q)", $__folders);

      foreach($q->rows as $a) {
         $id_list .= "," . $a['id'];
      }

      if($id_list) { $id_list = substr($id_list, 1, strlen($id_list)); }

      return $id_list;
   }


   private function get_meta_data($ids = array(), $q = '', $sortorder = '') {
      $messages = array();
      $tag = array();
      $note = array();

      if(count($ids) == 0) return $messages;

      if(MEMCACHED_ENABLED) {
         $cache_key = $this->make_cache_file_name($ids, 'meta');
         $memcache = Registry::get('memcache');
         $m = $memcache->get($cache_key);
         if(isset($m['meta'])) { return unserialize($m['meta']); }
      }

      $query = $this->db->query("SELECT `id`, `from`, `subject`, `piler_id`, `reference`, `size`, `spam`, `sent`, `arrived`, `attachments` FROM `" . TABLE_META . "` WHERE `id` IN ($q) $sortorder", $ids);

      if(isset($query->rows)) {

         array_unshift($ids, (int)$_SESSION['uid']);

         $tags = $this->db->query("SELECT `id`, `tag` FROM `" . TABLE_TAG . "` WHERE `uid`=? AND `id` IN ($q)", $ids);

         foreach ($tags->rows as $t) {
            $tag[$t['id']] = $t['tag'];
         }

         $notes = $this->db->query("SELECT `id`, `note` FROM " . TABLE_NOTE . " WHERE `uid`=? AND `id` IN ($q)", $ids);

         foreach ($notes->rows as $n) {
            $note[$n['id']] = $n['note'];
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
            if(isset($note[$m['id']])) { $m['note'] = $note[$m['id']]; } else { $m['note'] = ''; }

            array_push($messages, $m);
         }
      }

      if(MEMCACHED_ENABLED) {
         $memcache->add($cache_key, array('meta' => serialize($messages)), 0, MEMCACHED_TTL);
      }

      return $messages;
   }


   public function get_message_recipients($id = '') {
      $rcpt = array();
      $domains = array();

      if(Registry::get('auditor_user') == 0) { return $rcpt; }

      $query = $this->db->query("SELECT `domain` FROM " . TABLE_DOMAIN);
      foreach($query->rows as $q) {
         array_push($domains, $q['domain']);
      }

      $query = $this->db->query("SELECT `to` FROM " . VIEW_MESSAGES . " WHERE id=?", array($id));

      foreach($query->rows as $q) {
         $mydomain = 0;

         foreach ($domains as $domain) {
            if(preg_match("/\@$domain$/", $q['to'])) { $mydomain = 1; break; }
         }

         if($mydomain == 1) {
            array_push($rcpt, $q['to']);
         }
      }

      return $rcpt;
   }


   public function get_message_addresses_in_my_domain($id = '') {
      $addr = array();
      $domains = array();

      if(Registry::get('auditor_user') == 0) { return $addr; }

      $query = $this->db->query("SELECT `domain` FROM " . TABLE_DOMAIN);
      foreach($query->rows as $q) {
         array_push($domains, $q['domain']);
      }

      $query = $this->db->query("SELECT `from`, `to` FROM " . VIEW_MESSAGES . " WHERE id=?", array($id));

      if(isset($query->row)) {
         foreach ($domains as $domain) {
            if(preg_match("/\@$domain$/", $query->row['from'])) { array_push($addr, $query->row['from']); }
         }
      }

      foreach($query->rows as $q) {
         $mydomain = 0;

         foreach ($domains as $domain) {
            if(preg_match("/\@$domain$/", $q['to'])) { $mydomain = 1; break; }
         }

         if($mydomain == 1) {
            if(!in_array($q['to'], $addr)) { array_push($addr, $q['to']); }
         }
      }

      return $addr;

   }


   private function get_all_your_address() {
      $s = '';

      while(list($k, $v) = each($_SESSION['emails'])) {
         if($s) { $s .= '| ' .  $this->fix_email_address_for_sphinx($v); }
         else { $s = $this->fix_email_address_for_sphinx($v); }
      }

      return $s;
   }


   public function check_your_permission_by_id($id = '') {
      $q = '';
      $arr = $a = array();

      if($id == '') { return 0; }

      if(Registry::get('auditor_user') == 1 && RESTRICTED_AUDITOR == 0) { return 1; }

      array_push($arr, $id);

      if(Registry::get('auditor_user') == 1 && RESTRICTED_AUDITOR == 1) {
         while(list($k, $v) = each($_SESSION['auditdomains'])) {
            if(validdomain($v) == 1) {
               $q .= ",?";
               array_push($a, $v);
            }
         }
      }
      else {
         while(list($k, $v) = each($_SESSION['emails'])) {
            if(validemail($v) == 1) {
               $q .= ",?";
               array_push($a, $v);
            }
         }
      }

      $q = preg_replace("/^\,/", "", $q);

      $arr = array_merge($arr, $a, $a);

      if(ENABLE_FOLDER_RESTRICTIONS == 1) {
         $query = $this->sphx->query("SELECT folder FROM " . SPHINX_MAIN_INDEX . " WHERE id=" . (int)$id);
         if(isset($query->row['folder']) && in_array($query->row['folder'], $_SESSION['folders'])) { return 1; }
      }
      else {
         if(Registry::get('auditor_user') == 1 && RESTRICTED_AUDITOR == 1) {
            $query = $this->db->query("SELECT id FROM " . VIEW_MESSAGES . " WHERE id=? AND ( `fromdomain` IN ($q) OR `todomain` IN ($q) )", $arr);
         } else {
            $query = $this->db->query("SELECT id FROM " . VIEW_MESSAGES . " WHERE id=? AND ( `from` IN ($q) OR `to` IN ($q) )", $arr);
         }

         if(isset($query->row['id'])) { return 1; }
      }

      return 0;
   }


   public function check_your_permission_by_id_list($id = array()) {
      $q = $q2 = '';
      $arr = $a = $result = array();

      if(count($id) < 1) { return $result; }

      $arr = $id;

      for($i=0; $i<count($id); $i++) {
         $q2 .= ",?";
      }


      $q2 = preg_replace("/^\,/", "", $q2);

      if(Registry::get('auditor_user') == 1 && RESTRICTED_AUDITOR == 1) {
         while(list($k, $v) = each($_SESSION['auditdomains'])) {
            if(validdomain($v) == 1) {
               $q .= ",?";
               array_push($a, $v);
            }
         }
      }
      else {
         if(Registry::get('auditor_user') == 0) {
            while(list($k, $v) = each($_SESSION['emails'])) {
               if(validemail($v) == 1) {
                  $q .= ",?";
                  array_push($a, $v);
               }
            }
         }
      }

      $q = preg_replace("/^\,/", "", $q);


      if(Registry::get('auditor_user') == 1 && RESTRICTED_AUDITOR == 0) {
         $query = $this->db->query("SELECT id FROM `" . TABLE_META . "` WHERE `id` IN ($q2)", $arr);
      }
      else {

         if(ENABLE_FOLDER_RESTRICTIONS == 1) {
            $query = $this->sphx->query("SELECT id, folder FROM " . SPHINX_MAIN_INDEX . " WHERE id IN (" . implode(",", $id) . ")");
         }
         else {
            $arr = array_merge($arr, $a, $a);
            if(Registry::get('auditor_user') == 1 && RESTRICTED_AUDITOR == 1) {
               $query = $this->db->query("SELECT id FROM `" . VIEW_MESSAGES . "` WHERE `id` IN ($q2) AND ( `fromdomain` IN ($q) OR `todomain` IN ($q) )", $arr);
            } else {
               $query = $this->db->query("SELECT id FROM `" . VIEW_MESSAGES . "` WHERE `id` IN ($q2) AND ( `from` IN ($q) OR `to` IN ($q) )", $arr);
            }

         }
      }

      if($query->num_rows > 0) {
         foreach ($query->rows as $q) {
            if(ENABLE_FOLDER_RESTRICTIONS == 1) {
               if(in_array($q['folder'], $_SESSION['folders'])) { array_push($result, $q['id']); }
            }
            else {
               array_push($result, $q['id']);
            }
         }
      }

      return $result;
   }


   private function fix_email_address_for_sphinx($email = '') {
      $email = preg_replace("/\|@/", "|", $email);
      return preg_replace("/[\@\.\+\-\_]/", "X", $email);
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


   private function fixup_meta_characters($s = '') {
      if($s == '') { return $s; }

      $s = preg_replace("/\'/", ' ', $s);

      return $s;
   }


   private function fixup_sphinx_operators($s = '') {
      if($s == '') { return $s; }

      $s = preg_replace("/ OR /", "|", $s);
      $s = preg_replace("/(\-)/", " ", $s);
      $s = preg_replace("/\'/", '"', $s);
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
