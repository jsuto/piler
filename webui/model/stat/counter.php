<?php

class ModelStatCounter extends Model {

   public function get_counters(){
      $counter = array();
      $asize = 0;

      if(MEMCACHED_ENABLED) {
         $memcache = Registry::get('memcache');

         $counter = $memcache->get(Registry::get('counters'));

         if(isset($counter[MEMCACHED_PREFIX . 'counters_last_update'])) {
            if(isset($counter[MEMCACHED_PREFIX . 'size'])) { $asize = $counter[MEMCACHED_PREFIX . 'size']; }
            if(isset($counter[MEMCACHED_PREFIX . 'stored_size'])) { $ssize = $counter[MEMCACHED_PREFIX . 'stored_size']; }
            unset($counter[MEMCACHED_PREFIX . 'size']);
            unset($counter[MEMCACHED_PREFIX . 'stored_size']);

            return array ($asize, $ssize, $counter);
         }
      }

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
