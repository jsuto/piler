<?php


class ControllerAuditAudit extends Controller {

   public function index(){

      $this->id = "content";
      $this->template = "audit/audit.tpl";
      $this->layout = "common/layout";

      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('audit/audit');


      /*if(isset($this->request->post['searchterm'])) {
         $this->fixup_post_request();

         $a = preg_replace("/\&loaded=1$/", "", $this->request->post['searchterm']);
      }*/


      $this->render();
   }


   private function fixup_post_request() {
      $i = 0;
      $a = array();

      $this->data['blocks'] = array();

      $this->data['searchterm'] = $this->request->post['searchterm'];

      parse_str($this->request->post['searchterm'], $a);

      foreach($a['f'] as $f) {
         $val = array_shift($a['v']);

         if($val == '') { continue; }

         if($i == 0) {
            $this->data['key0'] = 0;

            if($f == 'user') { $this->data['key0'] = 0; }
            else if($f == 'ipaddr') { $this->data['key0'] = 1; }
            else if($f == 'ref') { $this->data['key0'] = 2; }

            $this->data['val0'] = $val;
         }

         $i++;
      }

      if(isset($a['date1'])) { $this->data['date1'] = $a['date1']; }
      if(isset($a['date2'])) { $this->data['date2'] = $a['date2']; }

      if(isset($a['action'])) { $this->data['action'] = $a['action']; }
 
   }





}

?>
