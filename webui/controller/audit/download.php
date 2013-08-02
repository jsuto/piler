<?php


class ControllerAuditDownload extends Controller {

   public function index(){

      $this->id = "content";
      $this->template = "message/headers.tpl";
      $this->layout = "common/layout-empty";

      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('audit/audit');

      if(Registry::get('admin_user') == 0 && Registry::get('auditor_user') == 0) {
         die("go away");
      }

      $this->document->title = $this->data['text_message'];

      header("Cache-Control: public, must-revalidate");
      header("Pragma: no-cache");
      header("Content-Type: application/octet-stream");
      header("Content-Disposition: attachment; filename=audit-" . time() . ".csv");
      header("Content-Transfer-Encoding: binary\n");

      $this->model_audit_audit->print_audit_to_csv();
   }


}

?>
