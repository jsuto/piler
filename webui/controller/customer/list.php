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

      $this->data['search'] = '';

      if(isset($this->request->post['search'])) { $this->data['search'] = $this->request->post['search']; }
      else if(isset($this->request->get['search'])) { $this->data['search'] = $this->request->get['search']; }


      /* check if we are admin */
      
      if(Registry::get('admin_user') == 1) {

         if($this->request->server['REQUEST_METHOD'] == 'POST') {
            if($this->validate() == true) {
            
               if(isset($this->request->post['id'])) {
                  if($this->model_saas_customer->update($this->request->post) == 1) {
                     $this->data['x'] = $this->data['text_successfully_modified'];
                  } else {
                     $this->data['errorstring'] = $this->data['text_failed_to_modify'];
                     // set customer ID to be the submitted id
                     if (isset($this->request->post['id'])) { $this->data['id'] = $this->request->post['id']; }
                  }
               }
               else {
                  if($this->model_saas_customer->add($this->request->post) == 1) {
                     $this->data['x'] = $this->data['text_successfully_added'];
                  } else {
                     $this->data['errorstring'] = $this->data['text_failed_to_add'];
                  }
               }
            }
            else {            
                $this->data['errorstring'] = $this->data['text_error_message'];
                $this->data['errors'] = $this->error;
                // set customer ID to be the submitted id
                if (isset($this->request->post['id'])) { $this->data['id'] = $this->request->post['id']; print_r($this->data['id']);}
            } 
         }

         $this->data['domains'] = $this->model_domain_domain->get_mapped_domains();

         if($this->data['id'] != -1) {
            $this->data['a'] = $this->model_saas_customer->get($this->data['id']);  
         }
         else {
            $this->data['entries'] = $this->model_saas_customer->search($this->data['search']);
         }
         
         if ( isset($this->data['errorstring']) ) {
            // use posted values if they differ from database values (ie - form was submitted but failed validation)
            if (isset($this->request->post['domain'])) { $this->data['a']['domain'] = $this->request->post['domain'];}
            if (isset($this->request->post['branding_text'])) { $this->data['a']['branding_text'] = $this->request->post['branding_text'];}
            if (isset($this->request->post['branding_url'])) { $this->data['a']['branding_url'] = $this->request->post['branding_url'];}
            if (isset($this->request->post['branding_logo'])) { $this->data['a']['branding_logo'] = $this->request->post['branding_logo'];}
            if (isset($this->request->post['support_link'])) { $this->data['a']['support_link'] = $this->request->post['support_link'];}
            if (isset($this->request->post['text_colour'])) { $this->data['a']['text_colour'] = $this->request->post['text_colour'];}
            if (isset($this->request->post['background_colour'])) { $this->data['a']['background_colour'] = $this->request->post['background_colour'];}
         }
         
      }
      else {
         $this->template = "common/error.tpl";
         $this->data['errorstring'] = $this->data['text_you_are_not_admin'];
      }


      $this->render();
   }


   private function validate() {
      // domain is required and must be 1 or more characters in length to meet this
      if(!isset($this->request->post['domain']) || strlen($this->request->post['domain']) < 1) {
         $this->error['domain'] = $this->data['text_invalid_data'];
      }
      // branding text is required and must be 1 or more characters in length to meet this
      if(!isset($this->request->post['branding_text']) || strlen($this->request->post['branding_text']) < 1) {
         $this->error['branding_text'] = $this->data['text_field_required'];
      }
      // if colour is provided it must be in the format #fcfcfc or #fcf
      if(isset($this->request->post['text_colour']) && strlen($this->request->post['text_colour']) > 0 && !preg_match('/^#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})$/', $this->request->post['text_colour'])) {
         $this->error['text_colour'] = $this->data['text_field_colour'];
      }
      if(isset($this->request->post['background_colour']) && strlen($this->request->post['background_colour']) > 0 && !preg_match('/^#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})$/', $this->request->post['background_colour'])) {
         $this->error['background_colour'] = $this->data['text_field_colour'];
      }

      if (!$this->error) {
         return true;
      } else {
         return false;
      }

   }



}

?>
