<?php


class ControllerLdapRemove extends Controller {
   private $error = array();
   private $domains = array();
   private $d = array();

   public function index(){

      $this->id = "content";
      $this->template = "ldap/remove.tpl";
      $this->layout = "common/layout";


      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('saas/ldap');

      $this->document->title = $this->data['text_ldap'];


      $this->data['username'] = Registry::get('username');

      $this->data['id'] = $this->request->get['id'];
      $this->data['description'] = $this->request->get['description'];
      $this->data['confirmed'] = (int)$this->request->get['confirmed'];


      if($this->validate() == true) {

         if($this->data['confirmed'] == 1) {
            $ret = $this->model_saas_ldap->delete($this->data['id'], $this->data['description']);
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

      if(!isset($this->request->get['description']) || strlen($this->request->get['description']) < 1) {
         $this->error['description'] = $this->data['text_invalid_data'];
      }

      if(!isset($this->request->get['id']) || !is_numeric($this->request->get['id'])) {
         $this->error['id'] = $this->data['text_invalid_data'];
      }

      if (!$this->error) {
         return true;
      } else {
         return false;
      }

   }


}

?>
