<?php

class Request {
   public $get = array();
   public $post = array();
   public $cookie = array();
   public $files = array();
   public $server = array();
   public $fixed_request_uri = "";


   public function __construct() {
       $this->get    =& $this->clean($_GET);
       $this->post   =& $this->clean($_POST);
       $this->cookie =& $this->clean($_COOKIE);
       $this->files  =& $this->clean($_FILES);
       $this->server =& $this->clean($_SERVER);

       if(isset($_SERVER['REQUEST_URI'])) {
          $this->fixed_request_uri = preg_replace("/\/([\w]+)\.php\?{0,1}/", "", $_SERVER['REQUEST_URI']);
       }

   }


   public function clean($data) {
      if (is_array($data)) {
         foreach ($data as $key => $value) {
            $data[$key] =& $this->clean($value);
         }
      } else {
          //$data = htmlspecialchars($data, ENT_QUOTES, 'UTF-8');
      }

      return $data;
   }


}


?>
