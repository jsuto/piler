<?php

class ModelSearchAuto extends Model {



   public function get($s = '') {
      $data = array();

      $query = $this->db->query("SELECT * FROM " . TABLE_AUTOSEARCH . " ORDER BY id ASC");

      if(isset($query->rows)) {
         foreach($query->rows as $q) {

            $data[] = array('id' => $q['id'], 'query' => $q['query']);
         }
      }

      return $data;
   }


   public function add($data = array()) {
      if(!isset($data['query'])) { return 0; }

      $query = $this->db->query("INSERT INTO " . TABLE_AUTOSEARCH . " (query) VALUES (?)", array($data['query']));
      $rc = $this->db->countAffected();

      LOGGER("add new automated search query: " . $data['query'] . " (rc=$rc)");

      return $rc;
   }


   public function remove($id = 0) {
      if($id == 0) { return 0; }

      $query = $this->db->query("DELETE FROM " . TABLE_AUTOSEARCH . " WHERE id=?", array($id));

      $rc = $this->db->countAffected();

      LOGGER("remove autosearch: $id (rc=$rc)");

      return $rc;
   }


}


?>
