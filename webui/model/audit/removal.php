<?php

class ModelAuditRemoval extends Model {

   public function get_pending_removals($page_len = 0) {
      $limit = '';
      $from = (int)$page * (int)$page_len;

      if($page_len > 0) { $limit = " LIMIT " . (int)$from . ", " . (int)$page_len; }

      $query = $this->db->query("SELECT * FROM " . TABLE_DELETED . " WHERE deleted=-1 ORDER BY date1 DESC $limit");
      
      return $query->rows;
   }

}
