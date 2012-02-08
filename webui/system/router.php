<?php

class Router {
   protected $class;
   protected $method;
   protected $args = array();


   private function sanitize_path($path){
      return str_replace('../', '', $path); 
   }


   public function __construct($route, $args = array()) {

      $path = $this->sanitize_path($route);

      $file = DIR_APPLICATION . $path . '.php';

      if(is_file($file)){
         $this->class = $path;
      }

      if($args){
         $this->args = $args;
      }

      $this->method = 'index';
   }


   public function getClass(){
      return $this->class;
   }


   public function getMethod(){
      return $this->method;
   }


   public function getArgs(){
      return $this->args;
   }


}


?>
