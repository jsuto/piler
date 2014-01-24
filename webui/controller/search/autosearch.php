<?php


class ControllerSearchAutosearch extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "search/autosearch.tpl";
      $this->layout = "common/layout";


      $request = Registry::get('request');
      $db = Registry::get('db');

      $ldap_id = 0;


      $this->load->model('search/auto');

      $this->document->title = $this->data['text_automated_search'];


      $this->data['username'] = Registry::get('username');

      $this->data['page'] = 0;
      $this->data['page_len'] = get_page_length();

      $this->data['total'] = 0;

      $this->data['domains'] = array();

      /* get search term if there's any */

      $this->data['search'] = '';

      $this->data['remove'] = 0;
      $this->data['removed_query'] = '';

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
               if($this->model_search_auto->add($this->request->post) == 1) {
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

         if(isset($this->request->get['remove']) && $this->request->get['remove'] == 1 && $this->request->get['id'] > 0) {
            $this->model_search_auto->remove($this->request->get['id']);

            $this->data['remove'] = 1;
            $this->data['removed_query'] = $this->request->get['name'];
         }


         $this->data['data'] = $this->model_search_auto->get($this->data['search']);
      }
      else {
         $this->template = "common/error.tpl";
         $this->data['errorstring'] = $this->data['text_you_are_not_admin'];
      }


      $this->data['prev_page'] = $this->data['page'] - 1;
      $this->data['next_page'] = $this->data['page'] + 1;

      $this->data['total'] = floor(count($this->data['data']) / $this->data['page_len']);


      $this->render();
   }


   private function validate() {

      if(!isset($this->request->post['query']) || strlen($this->request->post['query']) < 3) {
         $this->data['text_field_length'] = str_replace("?",3,$this->data['text_field_length']);
      }

      if (!$this->error) {
         return true;
      } else {
         return false;
      }

   }



}

?>
