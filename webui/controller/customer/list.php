<?php


class ControllerCustomerList extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "customer/list.tpl";
      $this->layout = "common/layout";


      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('saas/customer');
      $this->load->model('domain/domain');

      $this->document->title = $this->data['text_customers'];


      $this->data['username'] = Registry::get('username');


      $this->data['page'] = 0;
      $this->data['page_len'] = get_page_length();

      $this->data['total'] = 0;

      $this->data['entries'] = array();

      $this->data['id'] = -1;

      if(isset($this->request->get['id'])) { $this->data['id'] = $this->request->get['id']; }

      /* check if we are admin */

      if(Registry::get('admin_user') == 1) {

         if($this->request->server['REQUEST_METHOD'] == 'POST') {
            if($this->validate() == true) {

               if(isset($this->request->post['id'])) {
                  if($this->model_saas_customer->update($this->request->post) == 1) {
                     $this->data['x'] = $this->data['text_successfully_modified'];
                  } else {
                     $this->template = "common/error.tpl";
                     $this->data['errorstring'] = $this->data['text_failed_to_modify'];
                  }
               }
               else {
                  if($this->model_saas_customer->add($this->request->post) == 1) {
                     $this->data['x'] = $this->data['text_successfully_added'];
                  } else {
                     $this->template = "common/error.tpl";
                     $this->data['errorstring'] = $this->data['text_failed_to_add'];
                  }
               }
            }
            else {
               $this->template = "common/error.tpl";
               $this->data['errorstring'] = array_pop($this->error);
            } 
         }

         $this->data['domains'] = $this->model_domain_domain->get_mapped_domains();

         if(isset($this->request->get['id'])) {
            $this->data['a'] = $this->model_saas_customer->get($this->request->get['id']);
         }
         else {
            $this->data['entries'] = $this->model_saas_customer->get();
         }

      }
      else {
         $this->template = "common/error.tpl";
         $this->data['errorstring'] = $this->data['text_you_are_not_admin'];
      }


      $this->render();
   }


   private function validate() {

      if(!isset($this->request->post['domain']) || strlen($this->request->post['domain']) < 1) {
         $this->error['domain'] = $this->data['text_invalid_data'];
      }

      if(!isset($this->request->post['branding_text']) || strlen($this->request->post['branding_text']) < 1) {
         $this->error['branding_text'] = $this->data['text_invalid_data'];
      }

      if (!$this->error) {
         return true;
      } else {
         return false;
      }

   }



}

?>
