<?php

class ModelStatCounter extends Model {

   public function getCounters(){
      $counter = array();

      if(MEMCACHED_ENABLED) {
         $memcache = Registry::get('memcache');

         $counter = $memcache->get(Registry::get('counters'));

         if(isset($counter['_c:counters_last_update'])) { return $counter; }
      }

      $query = $this->db->query("SELECT * FROM " . TABLE_COUNTER);

      if($query->num_rows == 1) {
         $counter = $query->row;
      }

      return $counter;
   }


   public function resetCounters(){

      if(MEMCACHED_ENABLED) {
         $memcache = Registry::get('memcache');

         foreach (Registry::get('counters') as $counter) {
            $memcache->set($counter, 0);
         }
      }

      $query = $this->db->query("UPDATE " . TABLE_COUNTER . " set rcvd=0, virus=0, duplicate=0, ignore=0");

      return 0;
   }

}

?>
