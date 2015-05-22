<?php

class ModelStatCounter extends Model {

   public function get_counters(){
      $counter = array();
      $asize = $ssize = 0;

      $query = $this->db->query("SELECT * FROM " . TABLE_COUNTER);

      if($query->num_rows == 1) {
         $asize = $query->row['size'];
         $ssize = $query->row['stored_size'];

         unset($query->row['size']);
         unset($query->row['stored_size']);

         $counter = $query->row;
      }

      return array ($asize, $ssize, $counter);
   }


}

?>
