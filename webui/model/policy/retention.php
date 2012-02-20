<?php


class ModelPolicyRetention extends Model {

   public function get_rules() {
      $query = $this->db->query("SELECT * FROM " . TABLE_RETENTION_RULE . " ORDER BY id");

      if(isset($query->rows)) { return $query->rows; }

      return array();
   }


   public function get_rule($id = 0) {
      $query = $this->db->query("SELECT * FROM " . TABLE_RETENTION_RULE . " WHERE id=?", array($id));

      if(isset($query->row)) { return $query->row; }

      return array();
   }


   public function add_new_rule($data = array()) {

      $query = $this->db->query("INSERT INTO " . TABLE_RETENTION_RULE . " (`from`,`to`,`subject`,`_size`,`size`,`attachment_type`,`_attachment_size`,`attachment_size`,`spam`,`days`) VALUES(?,?,?,?,?,?,?,?,?,?)", array($data['from'], $data['to'], $data['subject'], $data['_size'], $data['size'], $data['attachment_type'], $data['_attachment_size'], $data['attachment_size'], $data['spam'], $data['days']) );
      return $this->db->countAffected();
   }


   public function remove_rule($id = 0) {
      $query = $this->db->query("DELETE FROM " .  TABLE_RETENTION_RULE . " WHERE id=?", array($id));
      return $this->db->countAffected();
   }


}

?>
