<?php


class ControllerLdapList extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "ldap/list.tpl";
      $this->layout = "common/layout";


      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('saas/ldap');

      $this->document->title = $this->data['text_ldap'];


      $this->data['username'] = Registry::get('username');


      $this->data['page'] = 0;
      $this->data['page_len'] = get_page_length();

      $this->data['total'] = 0;

      $this->data['entries'] = array();


      /* check if we are admin */

      if(Registry::get('admin_user') == 1) {

         if($this->request->server['REQUEST_METHOD'] == 'POST') {
            if($this->validate() == true) {

               if($this->model_saas_ldap->add($this->request->post) == 1) {
                  $this->data['x'] = $this->data['text_successfully_added'];
               } else {
                  $this->template = "common/error.tpl";
                  $this->data['errorstring'] = $this->data['text_failed_to_add'];
               }
            }
            else {
               $this->template = "common/error.tpl";
               $this->data['errorstring'] = array_pop($this->error);
            } 
         }

         $this->data['entries'] = $this->model_saas_ldap->get();

      }
      else {
         $this->template = "common/error.tpl";
         $this->data['errorstring'] = $this->data['text_you_are_not_admin'];
      }


      $this->render();
   }


   private function validate() {

      if(!isset($this->request->post['description']) || strlen($this->request->post['description']) < 1) {
         $this->error['description'] = $this->data['text_invalid_data'];
      }

      if(!isset($this->request->post['ldap_host']) || strlen($this->request->post['ldap_host']) < 1) {
         $this->error['ldap_host'] = $this->data['text_invalid_data'];
      }

      if(!isset($this->request->post['ldap_base_dn']) || strlen($this->request->post['ldap_base_dn']) < 1) {
         $this->error['ldap_base_dn'] = $this->data['text_invalid_data'];
      }

      if(!isset($this->request->post['ldap_bind_dn']) || strlen($this->request->post['ldap_bind_dn']) < 1) {
         $this->error['ldap_bind_dn'] = $this->data['text_invalid_data'];
      }

      if(!isset($this->request->post['ldap_bind_pw']) || strlen($this->request->post['ldap_bind_pw']) < 1) {
         $this->error['ldap_bind_pw'] = $this->data['text_invalid_data'];
      }

      if (!$this->error) {
         return true;
      } else {
         return false;
      }

   }



}

?>
