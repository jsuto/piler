<?php

class ModelDomainDomain extends Model {

   public function getDomains() {

      $query = $this->db->query("SELECT domain, mapped FROM " . TABLE_DOMAIN . " ORDER BY domain ASC");

      if(isset($query->rows)) { return $query->rows; }

      return array();
   }


   public function get_domains_by_string($s = '', $page = 0, $page_len = PAGE_LEN) {
      $from = (int)$page * (int)$page_len;

      if(strlen($s) < 1) { return array(); }

      $query = $this->db->query("SELECT domain FROM `" . TABLE_DOMAIN . "` WHERE domain LIKE ? ORDER BY domain ASC  LIMIT " . (int)$from . ", " . (int)$page_len, array($s . "%") );

      if(isset($query->rows)) { return $query->rows; }

      return array();
   }


   public function deleteDomain($domain = '') {
      if($domain == "") { return 0; }

      $query = $this->db->query("DELETE FROM " . TABLE_DOMAIN . " WHERE domain=?", array($domain));

      $rc = $this->db->countAffected();

      LOGGER("remove domain: $domain (rc=$rc)");

      return $rc;
   }


   public function addDomain($domain = '', $mapped = '') {
      if($domain == "" || $mapped == "") { return 0; }

      $domains = explode("\n", $domain);

      foreach ($domains as $domain) {
         $domain = rtrim($domain);
         $query = $this->db->query("INSERT INTO " . TABLE_DOMAIN . " (domain, mapped) VALUES (?,?)", array($domain, $mapped));

         $rc = $this->db->countAffected();

         LOGGER("add domain: $domain (rc=$rc)");

         if($rc != 1){ return 0; }
      }

      return 1;
   }


}

?>
