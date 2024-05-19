<?php


class ControllerPolicyLegalhold extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "policy/legalhold.tpl";
      $this->layout = "common/layout";


      $request = Registry::get('request');
      $db = Registry::get('db');

      $ldap_id = 0;


      $this->load->model('policy/hold');

      $this->document->title = $this->data['text_legal_hold'];


      $this->data['username'] = Registry::get('username');

      $this->data['page'] = 0;
      $this->data['page_len'] = get_page_length();

      $this->data['total'] = 0;

      $this->data['emails'] = array();

      /* get search term if there's any */

      $this->data['search'] = '';

      if(isset($this->request->post['search'])) { $this->data['search'] = $this->request->post['search']; }
      else if(isset($this->request->get['search'])) { $this->data['search'] = $this->request->get['search']; }



      /* get page */

      if(isset($this->request->get['page']) && is_numeric($this->request->get['page']) && $this->request->get['page'] > 0) {
         $this->data['page'] = $this->request->get['page'];
      }



      /* check if we are admin */

      if(Registry::get('admin_user') == 1) {

         if($this->request->server['REQUEST_METHOD'] == 'POST') {
            if($this->validate() == true) {
               if($this->model_policy_hold->add_email($this->request->post['email']) == 1) {
                  $this->data['x'] = $this->data['text_successfully_added'];
               } else {
                  $this->data['errorstring'] = $this->data['text_failed_to_add'];
               }
            }
            else {
               $this->data['errorstring'] = $this->data['text_error_message'];
               $this->data['errors'] = $this->error;
               $this->data['post'] = $this->request->post;
            }
         }

         /* get list of emails */
         $this->data['emails'] = $this->model_policy_hold->get_emails($this->data['search']);

      }
      else {
         $this->template = "common/error.tpl";
         $this->data['errorstring'] = $this->data['text_you_are_not_admin'];
      }


      $this->data['prev_page'] = $this->data['page'] - 1;
      $this->data['next_page'] = $this->data['page'] + 1;

      $this->data['total'] = floor(count($this->data['emails']) / $this->data['page_len']);


      $this->render();
   }


   private function validate() {

      if(!isset($this->request->post['email']) || strlen($this->request->post['email']) < 3 || !validemail($this->request->post['email'])) {
         $this->error['email'] = $this->data['text_invalid_data'];
      }

      if (!$this->error) {
         return true;
      } else {
         return false;
      }

   }



}
