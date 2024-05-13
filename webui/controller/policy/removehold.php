<?php


class ControllerPolicyRemovehold extends Controller {
   private $error = array();
   private $domains = array();
   private $d = array();

   public function index(){

      $this->id = "content";
      $this->template = "policy/removehold.tpl";
      $this->layout = "common/layout";


      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('policy/hold');

      $this->document->title = $this->data['text_legal_hold'];


      $this->data['username'] = Registry::get('username');

      $this->data['email'] = @$this->request->get['email'];
      $this->data['confirmed'] = (int)@$this->request->get['confirmed'];


      if($this->validate() == true) {

         if($this->data['confirmed'] == 1) {
            $ret = $this->model_policy_hold->delete_email($this->data['email']);
            if($ret == 1){
               $this->data['x'] = $this->data['text_successfully_removed'];
               header("Location: " . SITE_URL . "index.php?route=policy/legalhold");
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

      if(!isset($this->request->get['email']) || strlen($this->request->get['email']) < 3 || !validemail($this->request->get['email'])) {
         $this->error['email'] = $this->data['text_invalid_data'];
      }


      if (!$this->error) {
         return true;
      } else {
         return false;
      }

   }


}

?>
