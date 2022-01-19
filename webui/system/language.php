<?php

class Language {
   public $data = array();


   public function __construct () {
      global $langs;

      $lang = '';
      $session = Registry::get('session');

      if($session->get("lang")) {
         $file = DIR_LANGUAGE . $session->get("lang") . '/messages.php';
         define('LANG', $session->get("lang"));
      }
      else {
         $pref_langs = $this->get_preferred_languages();

         foreach($pref_langs as $k => $v) {
            if(in_array($v, $langs)) {
               $lang = $v;
               define('LANG', $lang);
               break;
            }
         }

         if($lang == '') { $lang = DEFAULT_LANG; }

         $session->set("lang", $lang);

         $file = DIR_LANGUAGE . $lang . '/messages.php';
      }

      if(file_exists($file)) {
         $_ = array();

         require($file);

         $this->data = array_merge($this->data, $_);
      }
      else {
         exit('Error: Could not load language ' . $file . '!');
      }

   }


   private function get_preferred_languages() {
      $langs = array();

      if(isset($_SERVER['HTTP_ACCEPT_LANGUAGE'])) {

         $l = explode(";", $_SERVER['HTTP_ACCEPT_LANGUAGE']);

         foreach($l as $k => $v) {

            $a = explode(",", $v);

            if(isset($a[0]) && substr($a[0], 0, 2) != 'q=') {
               array_push($langs, $a[0]);
            }
            if(isset($a[1])) { array_push($langs, $a[1]); }
         }
      }

      array_push($langs, DEFAULT_LANG);

      return $langs;
   }


   public function get($key) {
      return (isset($this->data[$key]) ? $this->data[$key] : $key);
   }


}






?>
