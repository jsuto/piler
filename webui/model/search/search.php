<?php

class ModelSearchSearch extends Model {

   public function search_messages($data = array(), $page = 0) {
      $one_page_of_ids = array();
      $total_hits = 0;
      $total_found = 0;
      $sort = "sent";
      $order = "DESC";
      $sortorder = "ORDER BY sent DESC";
      $cache_key = "";
      $q = "";
      $s = "";
      $all_ids_csv = "";

      $session = Registry::get('session');

      foreach($data as $k => $v) {
         if($v) { if(is_array($v)) { $v = implode(" ", $v); } $s .= '&' . $k . '=' . $v; }
      }

      if($s) { $s = substr($s, 1, strlen($s)); }

      AUDIT(ACTION_SEARCH, '', '', 0, $s);


      if($data['sort'] == "sent") { $sort = "sent"; }
      if($data['sort'] == "size") { $sort = "size"; }
      if($data['sort'] == "from") { $sort = "from"; }
      if($data['sort'] == "subj") { $sort = "subject"; }

      if($data['order'] == 1) { $order = "ASC"; }

      $sortorder = "ORDER BY `$sort` $order";

      // TODO: check if data is cached


      if(isset($data['ref']) && $data['ref']){
         list ($total_hits, $ids) = $this->query_all_possible_IDs_by_reference($data['ref'], $page);
      }
      else {
         list ($total_hits, $ids) = $this->get_results($data, $sort, $order, $sortorder, $page);
      }


      $current_hits = count($ids);

      if($current_hits > 0) {
         $session->set('last_search', serialize($ids));
      } else {
         $session->set('last_search', '');
      }

      return array($current_hits, $total_hits, implode(",", $ids), $this->get_meta_data($ids, $sortorder));
   }


   private function assemble_email_address_filter() {
      $session = Registry::get('session');

      if(Registry::get('auditor_user') == 1) {

         if(RESTRICTED_AUDITOR == 1) {
            $session_domains = $session->get('auditdomains');

            $sd = "";

            foreach($session_domains as $d) {
               $sd .= '|' . $this->fix_email_address_for_sphinx($d);
            }

            $sd = preg_replace("/^\|/", "", $sd);

            return sprintf(" (%s %s | %s %s ) ", TODOMAIN_TOKEN, $sd, FROMDOMAIN_TOKEN, $sd);
         }

         else { return ''; }
      }

      if(ENABLE_FOLDER_RESTRICTIONS == 1) { return ''; }

      $all_your_addresses = $this->get_all_your_address("emails");
      $all_your_wildcard_domains = $this->get_all_your_address("wildcard_domains");

      if($all_your_wildcard_domains) {
         return sprintf(" ( (%s %s) | (%s %s) | (%s %s) | (%s %s) ) ", FROM_TOKEN, $all_your_addresses, TO_TOKEN, $all_your_addresses, FROMDOMAIN_TOKEN, $all_your_wildcard_domains, TODOMAIN_TOKEN, $all_your_wildcard_domains);
      } else {
         return sprintf(" ( (%s %s) | (%s %s) ) ", FROM_TOKEN, $all_your_addresses, TO_TOKEN, $all_your_addresses);
      }
   }


   private function get_results($data = array(), $sort = 'sent', $order = 'DESC', $sortorder = '', $page = 0) {
      $ids = array();
      $__folders = array();
      $match = '';
      $direction = $attachment = $size = $folders = '';
      $tag_id_list = '';
      $a = "";
      $id = "";
      $offset = 0;
      $total_sphx_hits = $num_rows = 0;
      $fields = ["@(subject,body)", FROM_TOKEN, TO_TOKEN, "@subject", "@body", "@attachment_types"];


      $pagelen = get_page_length();
      $offset = $page * $pagelen;

      $emailfilter = $this->assemble_email_address_filter();

      $session = Registry::get('session');

      $i = 0;
      foreach($data['match'] as $k => $v) {
         if($v == "@attachment_types") {
            $i++;
            if($data['match'][$i] == "any") {
               $data['match'][$i-1] = "";
               $data['match'][$i] = "";
            }

            if($a == '') {
               $a = "attachments > 0 AND ";
            }
         }

         if(substr($v, 0, 7) == "http://") { $v = preg_replace("/\./", "X", $v); $data['match'][$i] = preg_replace("/http\:\/\//", "__URL__", $v); }

         if(!in_array($v, $fields) && $i > 0 && strchr($v, "@")) {

            if(substr($v, 0, 1) == "@") {
               $v = substr($v, 1, strlen($v)-1);
               if($data['match'][$i-1] == FROM_TOKEN) { $data['match'][$i-1] = FROMDOMAIN_TOKEN; }
               if($data['match'][$i-1] == TO_TOKEN) { $data['match'][$i-1] = TODOMAIN_TOKEN; }
            }

            $data['match'][$i] = $this->fix_email_address_for_sphinx($v);
         }
         $i++;
      }

      if(SEARCH_QUERY_QUOTING == 1) {
         $quoting = 0;

         for($i=2; $i<count($data['match']); $i++) {
            if(preg_match("/^\"/", $data['match'][$i])) {
               $quoting = 1;
            }

            if($quoting == 0) {
               $data['match'][$i] = '"' . $data['match'][$i] . '"';
            }

            if(preg_match("/\"$/", $data['match'][$i])) {
               $quoting = 0;
            }
         }
      }

      $match = implode(" ", $data['match']);

      if(Registry::get('auditor_user') == 1 && RESTRICTED_AUDITOR == 0 && $data['raw']) {
         $match .= $data['raw'];
      }

      if($emailfilter) {
         if(strlen($match) > 2) { $match = "( $match ) & $emailfilter"; }
         else { $match = $emailfilter; }
      }


      if($match == " ") { $match = ""; }

      if($data['sort'] == 'from' || $data['sort'] == 'subj') { $sortorder = ''; }

      $date = fixup_date_condition('sent', $data['date1'], $data['date2']);

      if($date) { $date .= " AND "; }

      if(isset($data['direction']) && $data['direction'] != '') { $direction = "direction = " . $data['direction'] . " AND "; }

      if(isset($data['size']) && $data['size']) {
         $data['size'] = preg_replace("/\s/", "", $data['size']);
         if(preg_match("/^(\>|\<)\={0,}\d{1,}$/", $data['size'])) { $size = "size " . $data['size'] . " AND "; }
      }

      if(isset($data['attachment_type']) && strstr($data['attachment_type'], 'any')) { $a = "attachments > 0 AND "; }
      else if(isset($data['has_attachment']) && $data['has_attachment'] == 1) { $attachment = "attachments > 0 AND "; }


      if(isset($data['id']) && $data['id']) {
         $data['id'] = preg_replace("/ /", "," , substr($data['id'], 1, strlen($data['id'])));
         $id = " id IN (" . $data['id'] . ") AND ";
      }


      if(ENABLE_FOLDER_RESTRICTIONS == 1) {
         $s = explode(" ", $data['folders']);
         foreach($s as $k => $v) {
            if(in_array($v, $session->get("folders"))) {
               array_push($__folders, $v);
            }
         }

         if(count($__folders) > 0) {
            $folders = "folder IN (" . implode(",", $__folders) . ") AND ";
         }
         else {
            $folders = "folder IN (" . implode(",", $session->get("folders")) . ") AND ";
         }
      }

      if(isset($data['tag']) && $data['tag']) {
         list ($total_found, $num_rows, $id_list) = $this->get_sphinx_id_list($data['tag'], SPHINX_TAG_INDEX, 'tag', $page);
         $query = $this->sphx->query("SELECT id FROM " . SPHINX_MAIN_INDEX . " WHERE $folders id IN ($id_list) $sortorder LIMIT 0,$pagelen OPTION max_matches=" . MAX_SEARCH_HITS);
      }
      else if(isset($data['note']) && $data['note']) {
         list ($total_found, $num_rows, $id_list) = $this->get_sphinx_id_list($data['note'], SPHINX_NOTE_INDEX, 'note', $page);
         $query = $this->sphx->query("SELECT id FROM " . SPHINX_MAIN_INDEX . " WHERE $folders id IN ($id_list) $sortorder LIMIT 0,$pagelen OPTION max_matches=" . MAX_SEARCH_HITS);
      }
      else if(ENABLE_FOLDER_RESTRICTIONS == 1 && isset($data['extra_folders']) && strlen($data['extra_folders']) > 0) {
         $query = $this->sphx->query("SELECT id FROM " . SPHINX_MAIN_INDEX . " WHERE $a $id $date $attachment $direction $size folder IN (" . preg_replace("/ /", ",", $data['extra_folders']) . ") AND MATCH('$match') $sortorder LIMIT $offset,$pagelen OPTION max_matches=" . MAX_SEARCH_HITS);
         $total_found = $query->total_found;
         $num_rows = $query->num_rows;
      }
      else {
         $query = $this->sphx->query("SELECT id FROM " . SPHINX_MAIN_INDEX . " WHERE $a $id $date $attachment $direction $size $folders MATCH('$match') $sortorder LIMIT $offset,$pagelen OPTION max_matches=" . MAX_SEARCH_HITS);
         $total_found = $query->total_found;
         $num_rows = $query->num_rows;

         $b = preg_split("/\ ORDER\ /", $query->query);
         $a = preg_split("/\ WHERE\ /", $b[0]);
         if(isset($a[1])) {
            $sphx_query = preg_replace("/\'/", "\'", $a[1]);
            $session->set("sphx_query", $sphx_query);
         }
      }

      /*
       * build an id list
       */

      $q = "";

      if(isset($query->rows)) {
         foreach($query->rows as $a) {
            if(isset($a['mid'])) { array_push($ids, $a['mid']); }
            else { array_push($ids, $a['id']); }

            if($q) { $q .= ",?"; }
            else { $q = "?"; }
         }
      }


      /*
       * if the query was requested to be sorted by sender or subject, then sphinx cannot do
       * that, so we assemble the list of all sphinx IDs matching the query
       */

      if($data['sort'] == 'from' || $data['sort'] == 'subj') {

         $fs_query = $this->db->query("SELECT id FROM " . TABLE_META . " WHERE id IN ($q) ORDER BY `$sort` $order", $ids);

         $ids = array();

         foreach($fs_query->rows as $q) {
            array_push($ids, $q['id']);
         }

      }

      // TODO: add caching if necessary

      return array($total_found, $ids);
   }


   private function query_all_possible_IDs_by_reference($reference = '', $page = 0) {
      $ids = array();
      $offset = 0;

      if($reference == '') { return $ids; }

      $session = Registry::get('session');

      $pagelen = get_page_length();
      $offset = $page * $pagelen;

      $query = $this->db->query("SELECT id FROM " . TABLE_META . " WHERE message_id=? OR reference=? ORDER BY id DESC LIMIT $offset,$pagelen", array($reference, $reference));

      foreach($query->rows as $q) {
         if($this->check_your_permission_by_id($q['id'])) {
            array_push($ids, $q['id']);
         }
      }

      if(ENABLE_FOLDER_RESTRICTIONS == 1) {
         $query = $this->sphx->query("SELECT id, folder FROM " . SPHINX_MAIN_INDEX . " WHERE id IN (" . implode(",", $ids) . ")");
         $ids = array();
         foreach($query->rows as $q) {
            if(isset($q['folder']) && in_array($q['folder'], $session->get("folders"))) { array_push($ids, $q['id']); }
         }
      }

      $total_found = count($ids);

      if($total_found >= $pagelen) {
         $query = $this->db->query("SELECT count(*) AS num FROM " . TABLE_META . " WHERE message_id=? OR reference=?", array($reference, $reference));
         $total_found = $query->row['num'];
      }

      // TODO: add caching if necessary

      return array($total_found, $ids);
   }


   public function preprocess_post_expert_request($data = array()) {
      $token = 'match';
      $ndate = 0;
      $match = array();

      $a = array(
                    'date1'           => '',
                    'date2'           => '',
                    'direction'       => '',
                    'size'            => '',
                    'attachment_type' => '',
                    'tag'             => '',
                    'note'            => '',
                    'ref'             => '',
                    'folders'         => '',
                    'extra_folders'   => '',
                    'id'              => '',
                    'raw'             => '',
                    'match'           => array()
      );

      if(!isset($data['search'])) { return $a; }

      $s = preg_replace("/https{0,1}:/", "httpX", $data['search']);
      $s = preg_replace("/:/", ": ", $s);
      $s = preg_replace("/,/", " ", $s);
      $s = preg_replace("/\(/", "( ", $s);
      $s = preg_replace("/\)/", ") ", $s);
      $s = preg_replace("/OR/", "|", $s);
      $s = preg_replace("/AND/", "", $s);
      $s = preg_replace("/\s{1,}/", " ", $s);
      $s = preg_replace("/httpX/", "http:", $s);
      $b = explode(" ", $s);

      foreach($b as $k => $v) {
         if($v == '') { continue; }

         if($v == 'from:') { $token = 'match'; $a['match'][] = FROM_TOKEN; continue; }
         else if($v == 'to:') { $token = 'match'; $a['match'][] = TO_TOKEN; continue; }
         else if($v == 'subject:') { $token = 'match'; $a['match'][] = '@subject'; continue; }
         else if($v == 'body:') { $token = 'match'; $a['match'][] = '@body'; continue; }
         else if($v == 'direction:' || $v == 'd:') { $token = 'direction'; continue; }
         else if($v == 'attachment:' || $v == 'a:') { $token = 'match'; $a['match'][] = '@attachment_types'; continue; }

         else if(in_array($v, ['size:', 'date1:', 'date2:', 'tag:', 'note:', 'ref:', 'id:', 'raw:'])) {
            $token = substr($v, 0, strlen($v)-1); continue;
         }

         else if($token != 'date1' && $token != 'date2') {
            if(preg_match("/\d{4}\-\d{1,2}\-\d{1,2}/", $v) || preg_match("/\d{1,2}\/\d{1,2}\/\d{4}/", $v)) {
               $ndate++;
               $a["date$ndate"] = $v;
            }
         }

         if($token == 'match') { $a['match'][] = $v; }
         else if(in_array($token, ['date1', 'date2', 'ref', 'tag', 'note', 'id', 'raw'])) { $a[$token] .= ' ' . $v; }

         else if($token == 'direction') {
            if($v == 'inbound') { $a['direction'] = "0"; }
            else if($v == 'outbound') { $a['direction'] = 2; }
            else if($v == 'internal') { $a['direction'] = 1; }
         }

         else if($token == 'size') {
            $o = substr($v, 0, 1);
            if($o == '<' || $o == '>') {
               $v = substr($v, 1, strlen($v));
               $o1 = substr($v, 0, 1);
               if($o1 == '=') {
                  $v = substr($v, 1, strlen($v));
                  $o .= $o1;
               }
            }
            else { $o = ''; }

            $s = explode("k", $v);
            if($s[0] != $v) { $v = $s[0] * 1000; }

            $s = explode("M", $v);
            if($s[0] != $v) { $v = $s[0] * 1000000; }

            $a['size'] .= ' ' . $o . $v;
         }

      }

      $a['sort'] = $data['sort'];
      $a['order'] = $data['order'];

      return $a;
   }


   private function get_sphinx_id_list($s = '', $sphx_table = '', $field = '', $page = 0) {
      $id_list = '';

      $session = Registry::get('session');

      $pagelen = get_page_length();
      $offset = $page * $pagelen;

      $s = $this->fixup_sphinx_operators($s);

      if(RT) {
         $idfield = 'mid';
      } else {
         $idfield = 'iid';
      }

      $q = $this->sphx->query("SELECT $idfield FROM $sphx_table WHERE uid=" . $session->get("uid") . " AND MATCH('@$field $s') LIMIT $offset,$pagelen OPTION max_matches=" . MAX_SEARCH_HITS);

      foreach($q->rows as $a) {
         $id_list .= "," . $a[$idfield];
      }

      if($id_list) { $id_list = substr($id_list, 1, strlen($id_list)); }

      if($id_list == '') { $id_list = "-1"; }
      return array($q->total_found, $q->num_rows, $id_list);
   }


   private function get_sphinx_id_list_by_extra_folders($extra_folders = '', $page = 0) {
      $id_list = '';
      $q = '';
      $__folders = array();

      $session = Registry::get('session');

      $pagelen = get_page_length();
      $offset = $page * $pagelen;

      $s = explode(" ", $extra_folders);
      foreach($s as $k => $v) {
         if(in_array($v, $session->get("extra_folders")) && is_numeric($v)) {
            array_push($__folders, $v);
            if($q) { $q .= ",?"; }
            else { $q = "?"; }
         }
      }


      $q = $this->db->query("SELECT iid FROM " . TABLE_FOLDER_MESSAGE . " WHERE folder_id IN ($q) $offset,$pagelen", $__folders);

      foreach($q->rows as $a) {
         $id_list .= "," . $a['iid'];
      }

      if($id_list) { $id_list = substr($id_list, 1, strlen($id_list)); }

      return array($q->total_found, $q->num_rows, $id_list);
   }


   private function get_meta_data($ids = array(), $sortorder = '') {
      $messages = array();
      $rcpt = $srcpt = array();
      $tag = array();
      $note = array();
      $private = [];
      $deleted = [];
      $marked_for_removal = [];
      $q = '';
      global $SUPPRESS_RECIPIENTS;

      if(count($ids) == 0) return $messages;

      // TODO: check if data in cache

      $session = Registry::get('session');

      $q = get_q_string($ids);

      $query = $this->db->query("SELECT `id`, `to` FROM `" . TABLE_RCPT . "` WHERE `id` IN ($q)", $ids);

      if(isset($query->rows)) {
         foreach($query->rows as $r) {
            if(!isset($rcpt[$r['id']])) { $rcpt[$r['id']] = []; }

            if(Registry::get('auditor_user') == 1 || !in_array($r['to'], $SUPPRESS_RECIPIENTS)) {
               array_push($rcpt[$r['id']], $r['to']);
            }
         }
      }


      $query = $this->db->query("SELECT `id`, `from`, `subject`, `piler_id`, `reference`, `retained`, `size`, `spam`, `sent`, `arrived`, `attachments` FROM `" . TABLE_META . "` WHERE `id` IN ($q) $sortorder", $ids);

      if(isset($query->rows)) {

         $s = $this->db->query("SELECT `id` FROM `" . TABLE_PRIVATE . "` WHERE id IN ($q)", $ids);

         foreach ($s->rows as $p) {
            $private[$p['id']] = 1;
         }

         if(ENABLE_DELETE) {
            $s = $this->db->query("SELECT `id`, `deleted` FROM `" . TABLE_DELETED . "` WHERE id IN ($q)", $ids);
            foreach ($s->rows as $p) {
               if($p['id'] == 1) {
                  $deleted[$p['id']] = 1;
               }
               $marked_for_removal[$p['id']] = 1;
            }
         }

         if(RT) {
            $id_field = 'mid';
            $ids_str = implode(",", $ids);
            $tags = $this->sphx->query("SELECT mid, tag FROM " . SPHINX_TAG_INDEX . " WHERE uid=" . (int)$session->get("uid") . " AND mid IN ($ids_str)");
         } else {
            $id_field = 'id';
            array_unshift($ids, (int)$session->get("uid"));
            $tags = $this->db->query("SELECT `id`, `tag` FROM `" . TABLE_TAG . "` WHERE `uid`=? AND `id` IN ($q)", $ids);
         }

         foreach ($tags->rows as $t) {
            $tag[$t[$id_field]] = $t['tag'];
         }

         if(RT) {
            $notes = $this->sphx->query("SELECT mid, note FROM " . SPHINX_NOTE_INDEX . " WHERE uid=" . (int)$session->get("uid") . " AND mid IN ($ids_str)");
         } else {
            $notes = $this->db->query("SELECT `id`, `note` FROM " . TABLE_NOTE . " WHERE `uid`=? AND `id` IN ($q)", $ids);
         }

         foreach ($notes->rows as $n) {
            $note[$n[$id_field]] = $n['note'];
         }

         $lang = Registry::get('language');


         foreach($query->rows as $m) {
            // We mark it as deleted even if it's only marked for removal
            if(ENABLE_DELETE == 1 && ($m['retained'] < NOW || isset($deleted[$m['id']])) ) $m['deleted'] = 1; else $m['deleted'] = 0;
            if(ENABLE_DELETE == 1 && isset($marked_for_removal[$m['id']])) $m['marked_for_removal'] = 1; else $m['marked_for_removal'] = 0;

            $m['shortfrom'] = make_short_string($m['from'], MAX_CGI_FROM_SUBJ_LEN);
            $m['from'] = escape_gt_lt_quote_symbols($m['from']);

            isset($rcpt[$m['id']]) ? $m['to'] = $rcpt[$m['id']] : $m['to'] = array('');
            $m['shortto'] = make_short_string($this->get_preferred_recipient($m['to']), MAX_CGI_FROM_SUBJ_LEN);
            $m['to'] = escape_gt_lt_quote_symbols($m['to']);


            if($m['subject'] == "") { $m['subject'] = "&lt;" . $lang->data['text_no_subject'] . "&gt;"; }

            $m['subject'] = escape_gt_lt_quote_symbols($m['subject']);
            $m['shortsubject'] = make_short_string($m['subject'], MAX_CGI_FROM_SUBJ_LEN);

            $m['date'] = date(DATE_TEMPLATE_SEARCH_HITS, $m['sent']);
            $m['preview_date'] = date(DATE_PREVIEW_TEMPLATE, $m['sent']);
            $m['size'] = nice_size($m['size']);

            in_array($m['from'], $session->get("emails")) ? $m['yousent'] = 1 : $m['yousent'] = 0;

            if(isset($tag[$m['id']])) { $m['tag'] = $tag[$m['id']]; } else { $m['tag'] = ''; }
            if(isset($note[$m['id']])) { $m['note'] = $note[$m['id']]; } else { $m['note'] = ''; }

            $m['note'] = preg_replace("/\"/", "*", strip_tags($m['note']));
            $m['tag'] = preg_replace("/\"/", "*", strip_tags($m['tag']));

            if(isset($private[$m['id']])) { $m['private'] = 1; } else { $m['private'] = 0; }

            array_push($messages, $m);
         }

      }

      // TODO: add caching if necessary

      return $messages;
   }


   private function get_preferred_recipient($arr = []) {
      $result = '';

      $session = Registry::get('session');
      $group_emails = $session->get('group_emails');
      $user_emails = $session->get('user_emails');

      if(count($arr) < 2 || (!$group_emails && !$user_emails) ) { return $arr[0]; }

      foreach ($arr as $a) {
         if($result == '' && in_array($a, $group_emails)) { $result = $a; }
         if(in_array($a, $user_emails)) { $result = $a; }
      }

      if($result == '') { $result = $arr[0]; }

      return $result;
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

      if(isset($query->row['from'])) {
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


   private function get_all_your_address($session_var) {
      $s = '';

      $session = Registry::get('session');

      $emails = $session->get($session_var);

      if(!$emails) { return $s; }

      foreach($emails as $k => $v) {
         if($s) { $s .= '| ' .  $this->fix_email_address_for_sphinx($v); }
         else { $s = $this->fix_email_address_for_sphinx($v); }
      }

      return $s;
   }


   private function get_wildcard_domains($arr=[]) {
      $query_suffix = '';
      $results = $arr;

      $session = Registry::get('session');

      $wildcard_domains = $session->get('wildcard_domains');

      if($wildcard_domains) {
         $q = str_repeat('?,', count($wildcard_domains));
         $q = trim($q, ',');
         $results = array_merge($results, $wildcard_domains, $wildcard_domains);
         $query_suffix = "OR fromdomain IN ($q) OR todomain IN ($q)";
      }

      return [$results, $query_suffix];
   }


   public function check_your_permission_by_id($id = '') {
      $q = '';
      $arr = $a = array();

      if($id == '') { return 0; }

      if(Registry::get('data_officer') == 1) {
         $query = $this->db->query("SELECT id FROM " . TABLE_DELETED . " WHERE deleted=-1 AND id=?", array($id));
         if(!isset($query->row['id'])) {
            return 0;
         }
      }

      if((Registry::get('auditor_user') == 1 || Registry::get('data_officer') == 1) && RESTRICTED_AUDITOR == 0) { return 1; }

      $session = Registry::get('session');

      array_push($arr, $id);

      if((Registry::get('auditor_user') == 1 || Registry::get('data_officer') == 1) && RESTRICTED_AUDITOR == 1) {
         if(validdomain($session->get("domain")) == 1) {
            $q .= ",?";
            array_push($a, $session->get("domain"));
         }

         $auditdomains = $session->get("auditdomains");

         foreach($auditdomains as $k => $v) {
            if(validdomain($v) == 1 && !in_array($v, $a)) {
               $q .= ",?";
               array_push($a, $v);
            }
         }
      }
      else {

         $query = $this->db->query("SELECT id FROM " . TABLE_PRIVATE . " WHERE id=?", array($id));
         if(isset($query->row['id'])) {
            return 0;
         }

         $emails = $session->get("emails");

         foreach($emails as $k => $v) {
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
         if(isset($query->row['folder']) && in_array($query->row['folder'], $session->get("folders"))) { return 1; }
      }
      else {
         if(Registry::get('auditor_user') == 1 && RESTRICTED_AUDITOR == 1) {
            $query = $this->db->query("SELECT id FROM " . VIEW_MESSAGES . " WHERE id=? AND ( `fromdomain` IN ($q) OR `todomain` IN ($q) )", $arr);
         } else {
            [$arr, $query_suffix] = $this->get_wildcard_domains($arr);
            $query = $this->db->query("SELECT id FROM " . VIEW_MESSAGES . " WHERE id=? AND ( `from` IN ($q) OR `to` IN ($q) $query_suffix )", $arr);
         }

         if(isset($query->row['id'])) { return 1; }
      }

      return 0;
   }


   public function check_your_permission_by_id_list($id = array()) {
      $q = $q2 = '';
      $arr = $parr = $a = $result = array();

      if(count($id) < 1) { return $result; }

      $session = Registry::get('session');

      $arr = $id;

      for($i=0; $i<count($id); $i++) {
         $q2 .= ",?";
      }


      $q2 = preg_replace("/^\,/", "", $q2);

      if(Registry::get('auditor_user') == 1 && RESTRICTED_AUDITOR == 1) {
         if(validdomain($session->get("domain")) == 1) {
            $q .= ",?";
            array_push($a, $session->get("domain"));
         }

         $auditdomains = $session->get("auditdomains");

         foreach($auditdomains as $k => $v) {
            if(validdomain($v) == 1 && !in_array($v, $a)) {
               $q .= ",?";
               array_push($a, $v);
            }
         }
      }
      else {
         if(Registry::get('auditor_user') == 0) {
            $emails = $session->get("emails");

            foreach($emails as $k => $v) {
               if(validemail($v) == 1) {
                  $q .= ",?";
                  array_push($a, $v);
               }
            }
         }
      }

      $q = preg_replace("/^\,/", "", $q);


      if(Registry::get('auditor_user') == 1 && RESTRICTED_AUDITOR == 0 && ENABLE_FOLDER_RESTRICTIONS == 0) {
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

               $query = $this->db->query("SELECT id FROM " . TABLE_PRIVATE . " WHERE `id` IN ($q2)", $id);
               if($query->num_rows > 0) {
                  foreach ($query->rows as $r) {
                     array_push($parr, $r['id']);
                  }
               }

               [$arr, $query_suffix] = $this->get_wildcard_domains($arr);

               $query = $this->db->query("SELECT id FROM `" . VIEW_MESSAGES . "` WHERE `id` IN ($q2) AND ( `from` IN ($q) OR `to` IN ($q) $query_suffix)", $arr);
            }

         }
      }

      if($query->num_rows > 0) {
         foreach ($query->rows as $q) {
            if(ENABLE_FOLDER_RESTRICTIONS == 1) {
               if(in_array($q['folder'], $session->get("folders"))) { array_push($result, $q['id']); }
            }
            else if(!in_array($q['id'], $result) && !in_array($q['id'], $parr)) {
               array_push($result, $q['id']);
            }
         }
      }

      return $result;
   }


   public function fix_email_address_for_sphinx($email = '') {
      if(strlen($email) > MAX_EMAIL_LEN) {
         return md5($email . ' ');
      }

      $email = preg_replace("/\|@/", "|", $email);
      return preg_replace("/[\@\.\+\-\_]/", "X", $email);
   }


   public function get_search_terms() {
      $session = Registry::get('session');

      $query = $this->db->query("SELECT term, ts FROM " . TABLE_SEARCH . " WHERE email=? ORDER BY ts DESC", array($session->get("email")));
      if(isset($query->rows)) { return $query->rows; }

      return array();
   }


   public function add_search_term($term = '') {
      if($term == '') { return 0; }

      $session = Registry::get('session');

      parse_str($term, $s);
      if(!isset($s['search']) || $s['search'] == '') { return 0; }

      if($this->update_search_term($term) == 0) {
         AUDIT(ACTION_SAVE_SEARCH, '', '', '', $term);
         $query = $this->db->query("INSERT INTO " . TABLE_SEARCH . " (email, ts, term) VALUES(?,?,?)", array($session->get("email"), time(), $term));
      }

      return 1;
   }


   public function update_search_term($term = '') {
      if($term == '') { return 0; }

      AUDIT(ACTION_SEARCH, '', '', '', $term);

      $session = Registry::get('session');

      $query = $this->db->query("UPDATE " . TABLE_SEARCH . " SET ts=? WHERE term=? AND email=?", array(time(), $term, $session->get("email")));

      return $this->db->countAffected();
   }


   public function remove_search_term($ts = 0) {
      $session = Registry::get('session');

      $query = $this->db->query("DELETE FROM " . TABLE_SEARCH . " WHERE email=? AND ts=?", array($session->get("email"), $ts));
   }


   private function fixup_meta_characters($s = '') {
      if($s == '') { return $s; }

      $s = preg_replace("/\'/", ' ', $s);
      $s = preg_replace("/\./", ' ', $s);

      return $s;
   }


   private function fixup_sphinx_operators($s = '') {
      if($s == '') { return $s; }

      $s = preg_replace("/ OR /", "|", $s);
      $s = preg_replace("/(\-)/", " ", $s);
      $s = preg_replace("/\'/", '"', $s);
      $a = explode(" ", $s);
      $s = '';

      foreach($a as $k => $v) {

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


}
