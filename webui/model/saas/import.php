<?php

class ModelSaasImport extends Model
{

   public function get($id = -1) {

      if($id >= 0) {
         $query = $this->db->query("SELECT * FROM " . TABLE_IMPORT . " WHERE id=?", array($id));
         if($query->num_rows > 0) { return $query->row; }
      }

      $query = $this->db->query("SELECT * FROM " . TABLE_IMPORT . " ORDER BY id ASC");

      if($query->num_rows > 0) { return $query->rows; }

      return array();
   }


   public function delete($id = 0, $description = '') {
      if($id == 0) { return 0; }

      $query = $this->db->query("DELETE FROM " . TABLE_IMPORT . " WHERE id=?", array($id));

      $rc = $this->db->countAffected();

      LOGGER("remove import entry: #$id, $description  (rc=$rc)");

      return $rc;
   }


   public function add($arr = array()) {
      if(!isset($arr['type']) || !isset($arr['username'])) { return 0; }

      $query = $this->db->query("INSERT INTO " . TABLE_IMPORT . " (type, username, password, server) VALUES (?,?,?,?)", array($arr['type'], $arr['username'], $arr['password'], $arr['server']));

      $rc = $this->db->countAffected();

      if($rc == 1){ return 1; }

      return 0;
   }


   public function update($arr = array()) {
      if(!isset($arr['id']) || !isset($arr['username']) || !isset($arr['password'])) { return 0; }

      $query = $this->db->query("UPDATE " . TABLE_IMPORT . " SET type=?, server=?, username=?, password=? WHERE id=?", array($arr['type'], $arr['server'], $arr['username'], $arr['password'], $arr['id']));

      return $this->db->countAffected();
   }


}

?>
