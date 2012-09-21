<?php

class Language {
   public $data = array();


   public function __construct () {
      global $langs;

      $lang = '';

      if(isset($_SESSION['lang'])) {
         $file = DIR_LANGUAGE . $_SESSION['lang'] . '/messages.php';
      }
      else {
         $pref_langs = $this->get_preferred_languages();

         while(list($k, $v) = each($pref_langs)) {
            if(in_array($v, $langs)) {
               $lang = $v;
               define('LANG', $lang);
               break;
            }
         }

         if($lang == '') { $lang = DEFAULT_LANG; }

         $_SESSION['lang'] = $lang;

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

         while(list($k, $v) = each($l)) {
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
