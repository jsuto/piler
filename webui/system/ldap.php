<?php


class LDAPDB {
   private $driver;


   public function __construct($driver, $ldaphost, $binddn, $bindpw) {
      if (!@require_once(DIR_DATABASE . $driver . '.php')) {
         exit('Error: Could not load database file ' . $driver . '!');
      }

      $this->driver = new $driver($ldaphost, $binddn, $bindpw);
   }


   public function ldap_query($basedn, $filter, $justthese) {
      return $this->driver->query($basedn, $filter, $justthese);
   }


   public function ldap_add($dn, $entry) {
      return @$this->driver->add($dn, $entry);
   }


   public function ldap_modify($dn, $entry) {
      return @$this->driver->modify($dn, $entry);
   }


   public function ldap_rename($dn, $newrdn, $newparent) {
      return @$this->driver->rename($dn, $newrdn, $newparent);
   }


   public function ldap_replace($dn, $entry) {
      return @$this->driver->replace($dn, $entry);
   }


   public function ldap_delete($dn) {
      return @$this->driver->delete($dn);
   }


}


?>
