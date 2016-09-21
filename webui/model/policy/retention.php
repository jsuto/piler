<?php


class ModelPolicyRetention extends Model {

   public function get_rules($s = '') {
      if($s) {
         $query = $this->db->query("SELECT * FROM " . TABLE_RETENTION_RULE . " WHERE domain LIKE ? OR `from` LIKE ? OR `subject` LIKE ? OR `body` LIKE ? ORDER BY domain, id", array('%' . $s . '%', '%' . $s . '%', '%' . $s . '%', '%' . $s . '%'));
      } else {
         $query = $this->db->query("SELECT * FROM " . TABLE_RETENTION_RULE . " ORDER BY domain, id");
      }

      if(isset($query->rows)) { return $query->rows; }

      return array();
   }


   public function get_rule($id = 0) {
      $query = $this->db->query("SELECT * FROM " . TABLE_RETENTION_RULE . " WHERE id=?", array($id));

      if(isset($query->row)) { return $query->row; }

      return array();
   }


   public function add_new_rule($data = array()) {
      $domain = '';

      if(isset($data['domain'])) { $domain = $data['domain']; }

      $query = $this->db->query("INSERT INTO " . TABLE_RETENTION_RULE . " (`domain`,`from`,`to`,`subject`,`body`,`_size`,`size`,`attachment_name`,`attachment_type`,`_attachment_size`,`attachment_size`,`spam`,`days`) VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?)", array($domain, $data['from'], $data['to'], $data['subject'], $data['body'], $data['_size'], $data['size'], $data['attachment_name'], $data['attachment_type'], $data['_attachment_size'], $data['attachment_size'], $data['spam'], $data['days']) );

      return $this->db->countAffected();
   }


   public function remove_rule($id = 0) {
      $query = $this->db->query("DELETE FROM " .  TABLE_RETENTION_RULE . " WHERE id=?", array($id));

      return $this->db->countAffected();
   }


   public function update_retention_time($data = array()) {
      if(DEFAULT_RETENTION <= 0) { return 0; }

      if(!isset($data['domain']) || !isset($data['days']) || $data['domain'] == '' || $data['days'] < 1) { return 0; }

      $default_retention = DEFAULT_RETENTION * 86400;

      $retained = NOW + 86400 * $data['days'];
      $start_ts = NOW - 2*$default_retention;

      $query = $this->db->query("UPDATE " . VIEW_MESSAGES . " SET retained = ? WHERE arrived > ? AND (todomain=? OR fromdomain=?)", array($retained, $start_ts, $data['domain'], $data['domain']));

      if(LOG_LEVEL >= NORMAL) { syslog(LOG_INFO, sprintf("update retention date: domain='%s', days=%d, hits=%d, exec time=%.2f sec", $data['domain'], $data['days'], $this->db->countAffected(), $query->exec_time)); }

      return $this->db->countAffected();
   }


}

?>
