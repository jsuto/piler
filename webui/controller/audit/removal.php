<?php


class ControllerAuditRemoval extends Controller {

   public function index(){

      $this->id = "content";
      $this->template = "audit/removal.tpl";
      $this->layout = "common/layout-audit-removal";

      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('audit/removal');

      if(Registry::get('admin_user') == 0 && Registry::get('auditor_user') == 0 && Registry::get('data_officer') == 0) {
         die("go away");
      }

      $this->data['data'] = $this->model_audit_removal->get_pending_removals();

      $this->render();
   }

}
