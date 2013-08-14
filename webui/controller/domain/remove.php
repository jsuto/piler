<?php


class ControllerDomainRemove extends Controller {
   private $error = array();
   private $domains = array();
   private $d = array();

   public function index(){

      $this->id = "content";
      $this->template = "domain/remove.tpl";
      $this->layout = "common/layout";


      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('domain/domain');

      $this->document->title = $this->data['text_domain'];


      $this->data['username'] = Registry::get('username');

      $this->data['domain'] = @$this->request->get['name'];
      $this->data['confirmed'] = (int)@$this->request->get['confirmed'];


      if($this->validate() == true) {

         if($this->data['confirmed'] == 1) {
            $ret = $this->model_domain_domain->deleteDomain($this->data['domain']);
            if($ret == 1){
               $this->data['x'] = $this->data['text_successfully_removed'];
            }
            else {
               $this->data['x'] = $this->data['text_failed_to_remove'];
            }
         }
      }
      else {
         $this->template = "common/error.tpl";
         $this->data['errorstring'] = array_pop($this->error);
      }



      $this->render();
   }


   private function validate() {

      if(Registry::get('admin_user') == 0) {
         $this->error['admin'] = $this->data['text_you_are_not_admin'];
      }

      if(!isset($this->request->get['name']) || strlen($this->request->get['name']) < 3 || ($this->request->get['name'] != "local" && !preg_match('/^[a-z0-9-]+(\.[a-z0-9-]+)*(\.[a-z]{2,5})$/', $this->request->get['name'])) ) {
         $this->error['domain'] = $this->data['text_invalid_data'];
      }


      if (!$this->error) {
         return true;
      } else {
         return false;
      }

   }


}

?>
