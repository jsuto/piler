<?php

class ControllerSystemSettings extends Controller {

   public function index() {
      $session = Registry::get('session');

      $out = [
         'preview' => PREVIEW,
      ];

      print json_encode($out, JSON_INVALID_UTF8_IGNORE);
   }
}
