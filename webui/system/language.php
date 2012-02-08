<?php

class Language {
   public $data = array();


   public function __construct () {
      if(isset($_SESSION['lang'])) {
         $file = DIR_LANGUAGE . $_SESSION['lang'] . '/messages.php';
      } else {
         $file = DIR_LANGUAGE . LANG . '/messages.php';
      }

      if (file_exists($file)) {
         $_ = array();

         require($file);

         $this->data = array_merge($this->data, $_);
      }
      else {
         exit('Error: Could not load language ' . $file . '!');
      }

   }


   public function get($key) {
      return (isset($this->data[$key]) ? $this->data[$key] : $key);
   }


}






?>
