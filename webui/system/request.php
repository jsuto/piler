<?php

class Request {
   public $get = array();
   public $post = array();
   public $cookie = array();
   public $files = array();
   public $server = array();
   public $fixed_request_uri = "";


   public function __construct() {
       $this->get    = $_GET;
       $this->post   = $_POST;
       $this->cookie = $_COOKIE;
       $this->files  = $_FILES;
       $this->server = $_SERVER;

       if(isset($_SERVER['REQUEST_URI'])) {
          $this->fixed_request_uri = preg_replace("/\/([\w]+)\.php\?{0,1}/", "", $_SERVER['REQUEST_URI']);
       }
   }

}


class Session {

   public function __construct() {
      session_start();
   }


   public function get($k = '') {
      if($k && isset($_SESSION[$k])) { return $_SESSION[$k]; }

      return '';
   }


   public function set($k = '', $v = '') {

      if($k) { $_SESSION[$k] = $v; }

   }


   public function remove($k = '') {
      if($k) { unset($_SESSION[$k]); }
   }


}


?>
