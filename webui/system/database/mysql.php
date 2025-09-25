<?php


class MySQL {
   private $link;
   private $affected;

   public function __construct($hostname, $username, $password, $database, $prefix = NULL) {

      try {
         $this->link = new PDO("mysql:host=$hostname;dbname=$database", $username, $password,
                                  array(
                                     PDO::MYSQL_ATTR_INIT_COMMAND => "SET NAMES " . DB_CHARSET,
                                     PDO::MYSQL_ATTR_INIT_COMMAND => "SET CHARACTER SET " . DB_CHARSET,
                                     PDO::ATTR_PERSISTENT => true
                                  )
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

      if(LOG_LEVEL >= DEBUG) { syslog(LOG_INFO, "sql=$sql, arr=" . implode(";", $arr)); }

      $query->error = 1;
      $query->errmsg = "Error";
      $query->query = $sql;

      $time_start = microtime(true);

      $i = 0;
      $data = array();

      $s = $this->link->prepare($sql);
      if(!$s) { return $query; }

      try {
         $s->execute($arr);
      }
      catch(PDOException $exception) { }

      $this->affected = $s->rowCount();

      $R = $s->fetchAll(PDO::FETCH_ASSOC);

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

      return $query;
   }


   public function countAffected() {
      return $this->affected;
   }


   public function getLastId() {
      return $this->link->lastInsertId();
   }


   public function __destruct() { }

}


?>
