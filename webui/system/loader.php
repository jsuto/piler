<?php


class Loader {

   public function __get($key) {
      return Registry::get($key);
   }


   public function __set($key, $value) {
      Registry::set($key, $value);
   }


   public function library($library) {
      $file = DIR_SYSTEM . $library . '.php';

      if (file_exists($file)) {
         include_once($file);
      } else {
         exit('Error: Could not load library ' . $library . '!');
      }
   }


   public function model($model) {
      $file  = DIR_MODEL . $model . '.php';

      $x = explode("/", $model);
      if(count($x) > 2){
         unset($x[1]);
         $model = join("/", $x);
      }

      $class = 'Model' . preg_replace('/[^a-zA-Z0-9]/', '', $model);

      if (file_exists($file)) {
         include_once($file);

         Registry::set('model_' . str_replace('/', '_', $model), new $class());
      } else {
         exit('Error: Could not load model ' . $model . '!');
      }
   }


   public function database($driver, $hostname, $username, $password, $database, $prefix = NULL, $charset = 'UTF8') {
      $file  = DIR_SYSTEM . 'database/' . $driver . '.php';
      $class = 'Database' . preg_replace('/[^a-zA-Z0-9]/', '', $driver);

      if (file_exists($file)) {
         include_once($file);

         Registry::set(str_replace('/', '_', $driver), new $class());
      } else {
         exit('Error: Could not load database ' . $drvier . '!');
      }
   }


   public function helper($helper) {
      $file = DIR_SYSTEM . 'helper/' . $helper . '.php';

      if (file_exists($file)) {
         include_once($file);
      } else {
         exit('Error: Could not load helper ' . $helper . '!');
      }
   }


   public function config($config) {
      $this->config->load($config);
   }


   public function language($language) {
      $this->language->load($language);
   }

}
