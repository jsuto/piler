<?php

class Front {
   protected $error;

   private function sanitize_path($path){
      return str_replace('../', '', $path); 
   }


   public function dispatch($action, $error){
      $this->error = $error;

      $action = $this->execute($action);

      if($action) $this->execute($error);
   }


   private function execute($action) {
      $file = DIR_APPLICATION . $this->sanitize_path($action->getClass()) . ".php";
      $class = 'Controller' . preg_replace("/[^a-zA-Z0-9]/", "", $action->getClass());

      $method = $action->getMethod();
      $args = $action->getArgs();


      $action = NULL;

      if(file_exists($file)){
         require_once($file);

         $controller = new $class();

         if(is_callable(array($controller, $method))){
            $action = call_user_func_array(array($controller, $method), $args);
         }
         else {
            $action = $this->error;

            $this->error = NULL;
         }
      }
      else {


         $action = $this->error;

         $this->error = NULL;
      }


      return $action;
   }


}

?>
