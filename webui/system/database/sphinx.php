<?php

class Sphinx {
   private $link;
   private $prefix;


   public function __construct($hostname, $username, $password, $database, $prefix = NULL) {

      if (!$this->link = mysql_connect($hostname, $username, $password)) {
         exit('Error: Could not make a database connection using ' . $username . '@' . $hostname);
      }

      $this->prefix = $prefix;

      mysql_query("SET NAMES 'utf8'", $this->link);
      mysql_query("SET CHARACTER SET utf8", $this->link);
   }


   public function query($sql) {
      $query = new stdClass();

      $query->query = $sql;
      $query->error = 0;
      $query->errmsg = "";

      $time_start = microtime(true);

      $resource = mysql_query(str_replace('#__', $this->prefix, $sql), $this->link);

      if($resource){
         if(is_resource($resource)){
            $i = 0;

            $data = array();

            while ($result = mysql_fetch_assoc($resource)) {
               $data[$i] = $result;

               $i++;
            }

            mysql_free_result($resource);

            $query->row      = isset($data[0]) ? $data[0] : array();
            $query->rows     = $data;
            $query->num_rows = $i;

            unset($data);

            $time_end = microtime(true);

            $query->exec_time = $time_end - $time_start;

            return $query;	
         }
         else {
            return $query;
         }
      }
      else {
         $_SESSION['error'] = 'Error: ' . mysql_error() . '<br />Error No: ' . mysql_errno() . '<br />' . $sql;

         $query->errmsg = 'Error: ' . mysql_error() . '<br />Error No: ' . mysql_errno() . '<br />' . $sql;
         $query->error = 1;

         return $query;
      }

   }


   public function countAffected() {
      return mysql_affected_rows($this->link);
   }


   public function getLastId() {
      return mysql_insert_id($this->link);
   }	


   public function __destruct() {
      mysql_close($this->link);
   }


}


?>
