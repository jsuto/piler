<?php

class ModelStatCounter extends Model {

   public function get_counters(){
      $counter = array();
      $asize = 0;

      if(MEMCACHED_ENABLED) {
         $memcache = Registry::get('memcache');

         $counter = $memcache->get(Registry::get('counters'));

         if(isset($counter[MEMCACHED_PREFIX . 'counters_last_update'])) {
            if(isset($counter[MEMCACHED_PREFIX . 'size'])) { $asize = nice_size($counter[MEMCACHED_PREFIX . 'size'], ' '); }
            unset($counter[MEMCACHED_PREFIX . 'size']);

            return array ($asize, $counter);
         }
      }

      $query = $this->db->query("SELECT * FROM " . TABLE_COUNTER);

      if($query->num_rows == 1) {
         $asize = nice_size($query->row['size'], ' ');
         unset($query->row['size']);

         $counter = $query->row;
      }

      return array ($asize, $counter);
   }


   public function reset_counters(){

      if(MEMCACHED_ENABLED) {
         $memcache = Registry::get('memcache');

         $c = Registry::get('counters');
         unset($c[MEMCACHED_PREFIX . 'size']);

         foreach ($c as $counter) {
            $memcache->set($counter, 0);
         }
      }

      $query = $this->db->query("UPDATE " . TABLE_COUNTER . " set `rcvd`=0, `virus`=0, `duplicate`=0, `ignore`=0");

      return 0;
   }

}

?>
