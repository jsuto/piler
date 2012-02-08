<?php


class ControllerHealthHealth extends Controller {

   public function index(){

      $this->id = "content";
      $this->template = "health/health.tpl";
      $this->layout = "common/layout-health";


      $request = Registry::get('request');
      $language = Registry::get('language');

      $this->document->title = $language->get('text_health_monitor');


      /* check if we are admin */

      if(Registry::get('admin_user') != 1 && Registry::get('readonly_admin') != 1 && Registry::get('auditor_admin') != 1) {
         $this->template = "common/error.tpl";
         $this->data['errorstring'] = $this->data['text_you_are_not_admin'];
      }


      $this->render();
   }

}

?>
