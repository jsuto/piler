<?php

class ModelPolicyHold extends Model {


   public function get_emails($s = '') {

      if($s) {
         $query = $this->db->query("SELECT email FROM " . TABLE_LEGAL_HOLD . " WHERE email LIKE ? ORDER BY email ASC", array('%' . $s . '%'));
      } else {
         $query = $this->db->query("SELECT email FROM " . TABLE_LEGAL_HOLD . " ORDER BY email ASC");
      }

      if(isset($query->rows)) { return $query->rows; }

      return array();
   }


   public function delete_email($email = '') {
      if($email == "") { return 0; }

      $query = $this->db->query("DELETE FROM " . TABLE_LEGAL_HOLD . " WHERE email=?", array($email));

      $rc = $this->db->countAffected();

      LOGGER("remove from legal hold: $email (rc=$rc)");

      return $rc;
   }


   public function add_email($email = '') {
      if($email == "") { return 0; }

      $email = strtolower($email);

      $query = $this->db->query("INSERT INTO " . TABLE_LEGAL_HOLD . " (email) VALUES (?)", array($email));
      $rc = $this->db->countAffected();
      if($rc == 1) {
         LOGGER("add legal hold: $email (rc=$rc)");
      }

      return $rc;
   }


}

?>
