<?php

class Sphinx {
   private $link;
   private $prefix;

   public function __construct($hostname, $username, $password, $database, $prefix = NULL) {

      list($host, $port) = explode(":", $hostname);

      try {
         $this->link = new PDO("mysql:host=$host;port=$port;dbname=$database;charset=utf8", $username, $password);
      }
      catch(PDOException $exception) {
         exit('Error: ' . $exception->getMessage() . " on database: $database<br />");
      }


      $this->affected = 0;
   }


   public function select_db($database) { }


   public function query($sql, $arr = array()) {
      $query = new stdClass();

      $query->error = 1;
      $query->errmsg = "Error";
      $query->query = $sql;

      $time_start = microtime(true);

      $i = 0;
      $data = array();

      $s = $this->link->prepare($sql);
      if(!$s) { return $query; }

      $s->execute($arr);

      $this->affected = $s->rowCount();

      $R = $s->fetchAll();

      while(list ($k, $v) = each($R)){
         $data[$i] = $v;
         $i++;
      }

      $query->row      = isset($data[0]) ? $data[0] : array();
      $query->rows     = $data;
      $query->num_rows = $i;

      $query->error = 0;
      $query->errmsg = "";

      unset($data);

      $time_end = microtime(true);

      $query->exec_time = $time_end - $time_start;

      return $query;
   }


   public function countAffected() {
      return mysql_affected_rows($this->link);
   }


   public function getLastId() {
      return mysql_insert_id($this->link);
   }	


   public function __destruct() { }

}


?>
