<?php


class ControllerAuditAudit extends Controller {

   public function index(){

      $this->id = "content";
      $this->template = "audit/audit.tpl";
      $this->layout = "common/layout-audit";

      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('audit/audit');


      $this->render();
   }

}

?>
