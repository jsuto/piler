<?php

class ModelAuditAudit extends Model {

   public function search_audit($data = array(), $page = 0) {
      $where = '';
      $arr = $results = array();
      $from = 0;
      $sort = 'ts';
      $order = 'DESC';
      $sortorder = 'ORDER BY ts DESC';
      $date1 = $date2 = 0;
      $match = '';
      $q = '';

      $pagelen = get_page_length();
      $offset = $page * $pagelen;

      $session = Registry::get('session');

      if($data['sort'] == 'user') { $sort = 'email'; }
      if($data['sort'] == 'ipaddr') { $sort = 'ipaddr'; }
      if($data['sort'] == 'ref') { $sort = 'meta_id'; }
      if($data['sort'] == 'action') { $sort = 'action'; }
      if($data['sort'] == 'description') { $sort = 'description'; }

      if($data['order'] == 1) { $order = 'ASC'; }

      $sortorder = "ORDER BY `$sort` $order";

      // TODO: validate $data[] fields

      if(isset($data['action']) && $data['action'] && $data['action'] != ACTION_ALL) {
         $where .= ' AND action=' . $data['action'];
      }

      if(isset($data['ipaddr']) && $data['ipaddr']) {
         $match .= ' @ipaddr ' . $data['ipaddr'];
      }

      if(isset($data['user']) && $data['user']) {
         $match .= ' @email ' . str_replace('@', '\\\\@', $data['user']);
      }

      if(isset($data['description']) && $data['description']) {
         $match .= ' @description ' . $data['description'];
      }

      if(isset($data['ref']) && $data['ref']) {
         $where .= ' AND meta_id=' . $data['ref'];
      }

      if(Registry::get('admin_user') == 0 && RESTRICTED_AUDITOR == 1) {
         $auditdomains = $session->get('auditdomains');

         foreach($auditdomains as $k => $v) {
            if($q) { $q .= ','; }
            $q .= '?';
            array_push($arr, $v);
         }

         $where .= " AND domain IN ($q) ";

         reset($session->get('auditdomains'));
      }


      if(isset($data['date1'])) { $date1 = $data['date1']; }
      if(isset($data['date2'])) { $date2 = $data['date2']; }

      $date = fixup_date_condition('ts', $date1, $date2);

      if($date) { $where .= " AND $date "; }
      if($match) { $where .= " AND MATCH(' $match ')"; }

      if($where) {
         $where = ' WHERE ' . substr($where, 5, strlen($where));
      }

      $from = $data['page_len'] * $data['page'];

      $query = $this->sphx->query('SELECT * FROM ' . SPHINX_AUDIT_INDEX . " $where $sortorder LIMIT $offset,$pagelen OPTION max_matches=" . MAX_SEARCH_HITS, $arr);

      $total_found = $query->total_found;
      $current_hits = $query->num_rows;

      $this->session->set("audit_query", array('where' => $where, 'sortorder' => $sortorder, 'arr' => $arr));

      foreach($query->rows as $a) {
         $a['description'] = str_replace('"', "'", $a['description']);

         $results[] = array(
            'id' => $a['meta_id'],
            'piler_id' => isset($m[$a['meta_id']]) ? $m[$a['meta_id']] : '',
            'action' => $a['action'],
            'email' => $a['email'],
            'date' => date(DATE_TEMPLATE . ' H:i', $a['ts']),
            'ipaddr' => DEMO_MODE == 1 ? anonimize_ip_addr($a['ipaddr']) : $a['ipaddr'],
            'description' => $a['description'],
            'shortdescription' => make_short_string($a['description'], MAX_CGI_FROM_SUBJ_LEN)
         );
      }

      return array($current_hits, $total_found, $results);
   }


   public function print_audit_to_csv() {
      $actions = array_flip(Registry::get('actions'));

      $a = $this->session->get('audit_query');

      if(isset($a['where']) && isset($a['sortorder']) && isset($a['arr'])) {
         print 'Date' . DELIMITER . 'ID' . DELIMITER . 'User' . DELIMITER . 'IP-address' . DELIMITER . 'Action' . DELIMITER . 'Piler ID' . DELIMITER . "Description\n";

	 $query = $this->sphx->query('SELECT * FROM ' . SPHINX_AUDIT_INDEX . ' ' . $a['where'] . ' ' . $a['sortorder'] . ' LIMIT 0,' . MAX_SEARCH_HITS . ' OPTION max_matches=' . MAX_SEARCH_HITS, $a['arr']);

         foreach($query->rows as $q) {
            if(DEMO_MODE == 1) { $q['ipaddr'] = anonimize_ip_addr($q['ipaddr']); }

            print date(DATE_TEMPLATE . ' H:i:s', $q['ts']) . DELIMITER . $q['id'] . DELIMITER . $q['email'] . DELIMITER . $q['ipaddr'] . DELIMITER . $actions[$q['action']] . DELIMITER . $q['meta_id'] . DELIMITER . $q['description'] . "\n";
         }
      }
   }


   public function can_download() {

      if(MAX_DOWNLOAD_PER_HOUR <= 0 || Registry::get('auditor_user') == 1) { return 1; }

      $session = Registry::get('session');

      $email = str_replace('@', '\\\\@', $session->get('email'));

      $query = $this->sphx->query('SELECT COUNT(*) AS num FROM ' . SPHINX_AUDIT_INDEX . " WHERE MATCH(' @email $email' ) AND ts > ? AND action=?", array($email, NOW-3600, ACTION_DOWNLOAD_MESSAGE));

      if($query->row['num'] <= MAX_DOWNLOAD_PER_HOUR) { return 1; }

      return 0;
   }


   public function can_restore() {

      if(MAX_RESTORE_PER_HOUR <= 0 || Registry::get('auditor_user') == 1) { return 1; }

      $session = Registry::get('session');

      $email = str_replace('@', '\\\\@', $session->get('email'));

      $query = $this->sphx->query('SELECT COUNT(*) AS num FROM ' . SPHINX_AUDIT_INDEX . " WHERE MATCH(' @email $email' ) AND ts > ? AND action=?", array($email, NOW-3600, ACTION_RESTORE_MESSAGE));

      if($query->row['num'] <= MAX_RESTORE_PER_HOUR) { return 1; }

      return 0;
   }

}
