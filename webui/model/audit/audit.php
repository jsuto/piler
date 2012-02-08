<?php

class ModelAuditAudit extends Model {

   public function search_audit($data = array()) {
      $where = '';
      $arr = $results = array();
      $from = 0;
      $sort = "ts";
      $order = "DESC";
      $sortorder = "ORDER BY ts DESC";
      $q = '';


      if($data['sort'] == "user") { $sort = "email"; }
      if($data['sort'] == "ipaddr") { $sort = "ipaddr"; }
      if($data['sort'] == "ref") { $sort = "meta_id"; }
      if($data['sort'] == "action") { $sort = "action"; }
      if($data['sort'] == "description") { $sort = "description"; }

      if($data['order'] == 1) { $order = "ASC"; }

      $sortorder = "ORDER BY `$sort` $order";


      if(isset($data['action']) && $data['action'] != ACTION_ALL) {
         $where .= " AND action=?";
         array_push($arr, $data['action']);
      }

      if(isset($data['ipaddr'])) {
         $where .= " AND ipaddr IN (" . $this->append_search_criteria($data['ipaddr'], &$arr) . ")";
      }

      if(isset($data['user'])) {
         $where .= " AND email IN (" . $this->append_search_criteria($data['user'], &$arr) . ")";
      }

      if(isset($data['ref'])) {
         $where .= " AND ref IN (" . $this->append_search_criteria($data['ref'], &$arr) . ")";
      }


      $date = fixup_date_condition('ts', $data['date1'], $data['date2']);


      if($date) { $where .= " AND $date "; }

      if($where) {
         $where = " WHERE " . substr($where, 5, strlen($where));
      }

      $from = $data['page_len'] * $data['page'];


      $query = $this->db->query("SELECT COUNT(*) AS count FROM " . TABLE_AUDIT . " $where", $arr);

      $n = $query->row['count'];

      if($n > 0) {
         if($n > MAX_AUDIT_HITS) { $n = MAX_AUDIT_HITS; }

         $query = $this->db->query("SELECT * FROM " . TABLE_AUDIT . " $where $sortorder LIMIT $from," . $data['page_len'], $arr);

 
         if(isset($query->rows)) {

            foreach($query->rows as $a) {
               if($a['meta_id'] > 0) { $q .= "," . $a['meta_id']; }
            }

            if($q) {
               $q = substr($q, 1, strlen($q));
               $Q = $this->db->query("SELECT id, piler_id FROM " . TABLE_META . " WHERE id IN($q)");

               foreach($Q->rows as $a) {
                  $m[$a['id']] = $a['piler_id'];
               }
            }

            foreach($query->rows as $a) {
               $results[] = array(
                                    'id' => $a['meta_id'],
                                    'piler_id' => isset($m[$a['meta_id']]) ? $m[$a['meta_id']] : '',
                                    'action' => $a['action'],
                                    'email' => $a['email'],
                                    'date' => date(AUDIT_DATE_FORMAT, $a['ts']),
                                    'ipaddr' => $a['ipaddr'],
                                    'description' => $a['description'],
                                    'shortdescription' => make_short_string($a['description'], MAX_CGI_FROM_SUBJ_LEN)
                                  );

            }
         }
      }

      return array($n, $results);
   }


   private function append_search_criteria($s = '', $arr = array()) {
      $q = "";

      $a = explode("*", $s);

      for($i=0; $i<count($a); $i++) {
         if($a[$i]) {
            array_push($arr, $a[$i]);
            $q .= ",?";
         }
      }

      $q = substr($q, 1, strlen($q));

      return $q;
   }


}

?>
