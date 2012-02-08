<?php


class ControllerDomainDomain extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "domain/list.tpl";
      $this->layout = "common/layout";


      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('domain/domain');

      $this->document->title = $this->data['text_domain'];


      $this->data['username'] = Registry::get('username');


      $this->data['page'] = 0;
      $this->data['page_len'] = get_page_length();

      $this->data['total'] = 0;

      $this->data['domains'] = array();

      /* get search term if there's any */

      if($this->request->server['REQUEST_METHOD'] == 'POST'){
         $this->data['search'] = @$this->request->post['search'];
      }
      else {
         $this->data['search'] = @$this->request->get['search'];
      }


      /* get page */

      if(isset($this->request->get['page']) && is_numeric($this->request->get['page']) && $this->request->get['page'] > 0) {
         $this->data['page'] = $this->request->get['page'];
      }



      /* check if we are admin */

      if(Registry::get('admin_user') == 1) {

         if($this->request->server['REQUEST_METHOD'] == 'POST') {
            if($this->validate() == true) {

               if($this->model_domain_domain->addDomain($this->request->post['domain'], $this->request->post['mapped']) == 1) {
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

         /* get list of current policies */

         $this->data['domains'] = $this->model_domain_domain->getDomains();

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
         $this->error['email'] = $this->data['text_invalid_data'];
      }
      else {
         $domains = explode("\n", $this->request->post['domain']);
         foreach ($domains as $domain) {
            $domain = rtrim($domain);
            if(!preg_match('/^[a-z0-9-]+(\.[a-z0-9-]+)*(\.[a-z]{2,5})$/', $domain) ) {
               $this->error['email'] = $this->data['text_invalid_data'] . ": $domain";
            }
         }
      }

      if(!isset($this->request->post['mapped']) || strlen($this->request->post['mapped']) < 3 || !preg_match('/^[a-z0-9-]+(\.[a-z0-9-]+)*(\.[a-z]{2,5})$/', $this->request->post['mapped']) ) {
         $this->error['domain'] = $this->data['text_invalid_data'] . ": " . $this->request->post['mapped'];
      }

      if (!$this->error) {
         return true;
      } else {
         return false;
      }

   }



}

?>
