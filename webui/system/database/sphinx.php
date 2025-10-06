<?php

class Sphinx {
   private $link;
   private $prefix;
   private $affected;

   public function __construct($hostname, $username, $password, $database, $prefix = NULL) {

      list($host, $port) = explode(":", $hostname);

      try {
         $this->link = new PDO("mysql:host=$host;port=$port;dbname=$database;charset=" . DB_CHARSET, $username, $password,
                               [PDO::ATTR_PERSISTENT => true]
         );
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
      $query->num_rows = 0;
      $query->rows = [];
      $query->total_found = 0;

      $time_start = microtime(true);

      $i = 0;
      $data = array();

      $s = $this->link->prepare($sql);
      if(!$s) { return $query; }

      try {
         $s->execute($arr);
      } catch(PDOException $exception) {
         syslog(LOG_INFO, "ERROR: " . $exception->getMessage());
         return $query;
      }

      $this->affected = $s->rowCount();

      $R = $s->fetchAll();

      foreach($R as $k => $v) {
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

      // Use the commented out line if you still have sphinx 2.0.x
      // and use select * from in model/search/search.php in line 232
      $meta = $this->link->prepare("SHOW META LIKE 'total_found'");
      //$meta = $this->link->prepare("SHOW META");

      $meta->execute();
      $R = $meta->fetchAll();
      foreach($R as $k => $v) {
         if($v[0] == "total_found") { $query->total_found = $v[1]; }
      }

      if(LOG_LEVEL >= NORMAL) { syslog(LOG_INFO, sprintf("sphinx query: '%s' in %.2f s, %d hits, %d total found", $query->query, $query->exec_time, $query->num_rows, $query->total_found)); }

      return $query;
   }


   public function countAffected() {
      return $this->affected;
   }


   public function getLastId() {
      return mysql_insert_id($this->link);
   }


   public function __destruct() { }

}
