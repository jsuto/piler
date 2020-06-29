<?php


class ControllerDomainDomain extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "domain/list.tpl";
      $this->layout = "common/layout";


      $request = Registry::get('request');
      $db = Registry::get('db');

      $ldap_id = 0;


      $this->load->model('domain/domain');
      if(ENABLE_SAAS == 1) {
         $this->load->model('saas/ldap');
         $this->data['ldap'] = $this->model_saas_ldap->search();
         if ( isset($this->request->post['ldap_id']) ) {
            $ldap_id = $this->request->post['ldap_id'];
         } else {
            $ldap_id = 0;
         }
      }

      $this->document->title = $this->data['text_domain'];


      $this->data['username'] = Registry::get('username');

      $this->data['page'] = 0;
      $this->data['page_len'] = get_page_length();

      $this->data['total'] = 0;

      $this->data['domains'] = array();

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
               if($this->model_domain_domain->addDomain($this->request->post['domain'], $this->request->post['mapped'], $ldap_id) == 1) {
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

         /* get list of domains */
         $this->data['domains'] = $this->model_domain_domain->getDomains($this->data['search']);

      }
      else {
         $this->template = "common/error.tpl";
         $this->data['errorstring'] = $this->data['text_you_are_not_admin'];
      }


      $this->data['prev_page'] = $this->data['page'] - 1;
      $this->data['next_page'] = $this->data['page'] + 1;

      $this->data['total'] = floor(count($this->data['domains']) / $this->data['page_len']);


      $this->render();
   }


   private function validate() {

      if(!isset($this->request->post['domain']) || strlen($this->request->post['domain']) < 3) {
         $this->data['text_field_length'] = str_replace("?",3,$this->data['text_field_length']);
         $this->error['domain'] = $this->data['text_field_length'];
      }
      else {
         $domains = explode("\n", $this->request->post['domain']);
         foreach ($domains as $domain) {
            $domain = rtrim($domain);
            if(!validdomain($domain) ) {
               $this->error['domain'] = $this->data['text_field_domain'];
            }
         }
      }

      if(!isset($this->request->post['mapped']) || strlen($this->request->post['mapped']) < 3) {
         $this->data['text_field_length'] = str_replace("?",3,$this->data['text_field_length']);
         $this->error['mapped'] = $this->data['text_field_length'];
      } elseif( !validdomain($this->request->post['mapped']) ) {
         $this->error['mapped'] = $this->data['text_field_domain'];
      }

      if (!$this->error) {
         return true;
      } else {
         return false;
      }

   }



}
