<?php


class ControllerImportJobs extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "import/jobs.tpl";
      $this->layout = "common/layout";


      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('saas/import');

      $this->document->title = $this->data['text_import'] . " jobs";


      $this->data['username'] = Registry::get('username');


      $this->data['page'] = 0;
      $this->data['page_len'] = get_page_length();

      $this->data['total'] = 0;

      $this->data['entries'] = array();

      $this->data['id'] = -1;

      if(isset($this->request->get['id'])) { $this->data['id'] = $this->request->get['id']; }

      $this->data['import_status'] = Registry::get('import_status');

      /* check if we are admin */

      if(Registry::get('admin_user') == 1) {

         if($this->request->server['REQUEST_METHOD'] == 'POST') {

            if($this->validate() == true) {

               if(isset($this->request->post['id'])) {
                  if($this->model_saas_import->update($this->request->post) == 1) {
                     $this->data['x'] = $this->data['text_successfully_modified'];
                  } else {
                     $this->data['errorstring'] = $this->data['text_failed_to_modify'];
                     // set ID to be the submitted id
                     if (isset($this->request->post['id'])) { $this->data['id'] = $this->request->post['id']; }
                  }
               }
               else {
                  if($this->model_saas_import->add($this->request->post) == 1) {
                     $this->data['x'] = $this->data['text_successfully_added'];
                  } else {
                     $this->data['errorstring'] = $this->data['text_failed_to_add'];
                  }
               }
            }
            else {
                $this->data['errorstring'] = $this->data['text_error_message'];
                $this->data['errors'] = $this->error;
               // set ID to be the submitted id
               if (isset($this->request->post['id'])) { $this->data['id'] = $this->request->post['id']; }
            } 
         }

         if(isset($this->request->get['id'])) {
            $this->data['a'] = $this->model_saas_import->get($this->data['id']);
         }
         else {
            $this->data['entries'] = $this->model_saas_import->get();
         }
         
         if ( isset($this->data['errorstring']) ) {
            // use posted values if they differ from database values (ie - form was submitted but failed validation)
            if (isset($this->request->post['server'])) { $this->data['a']['server'] = $this->request->post['server'];}
            if (isset($this->request->post['username'])) { $this->data['a']['username'] = $this->request->post['username'];}
            if (isset($this->request->post['type'])) { $this->data['a']['type'] = $this->request->post['type'];}
         }
         
      }
      else {
         $this->template = "common/error.tpl";
         $this->data['errorstring'] = $this->data['text_you_are_not_admin'];
      }


      $this->render();
   }


   private function validate() {

      if(!isset($this->request->post['type']) || strlen($this->request->post['type']) < 1) {
         $this->error['type'] = $this->data['text_field_required'];
      }

      if(!isset($this->request->post['server']) || strlen($this->request->post['server']) < 1) {
         $this->error['server'] = $this->data['text_field_required'];
      }

      if(!isset($this->request->post['username']) || strlen($this->request->post['username']) < 1) {
         $this->error['username'] = $this->data['text_field_required'];
      }

      if(!isset($this->request->post['password']) || strlen($this->request->post['password']) < 1) {
         $this->error['password'] = $this->data['text_field_required'];
      }

      if (!$this->error) {
         return true;
      } else {
         return false;
      }

   }



}

?>
