<?php

class DB {
   private $driver;
   private $prefix;

   public function __construct($driver, $hostname, $username, $password, $database, $prefix = NULL) {
      if (!@require_once(DIR_DATABASE . $driver . '.php')) {
         exit('Error: Could not load database file ' . $driver . '!');
      }

      $this->driver = new $driver($hostname, $username, $password, $database, $prefix);
      $this->database = $database;
   }


   public function select_db($db) {
      $this->driver->select_db($db);
   }


   public function query($sql, $arr = array()) {
      return $this->driver->query($sql, $arr);
   }


   public function countAffected() {
      return $this->driver->countAffected();
   }


   public function getLastId() {
      return $this->driver->getLastId();
   }

}
