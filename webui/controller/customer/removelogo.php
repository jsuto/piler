<?php


class ControllerCustomerRemoveLogo extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";

      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('saas/customer');

      $this->data['username'] = Registry::get('username');

      $this->data['id'] = $this->request->get['id'];
      $this->data['confirmed'] = (int)$this->request->get['confirmed'];


      if($this->validate() == true) {
         $ret = $this->model_saas_customer->delete_logo($this->data['id']);
         header('Location: ' . SITE_URL . 'index.php?route=customer/list&id=' . $this->data['id']);
      }
   }


   private function validate() {

      if(Registry::get('admin_user') == 0) {
         $this->error['admin'] = $this->data['text_you_are_not_admin'];
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
