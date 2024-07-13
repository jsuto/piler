<?php


class ControllerFolderList extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "folder/list.tpl";
      $this->layout = "common/layout";


      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('folder/folder');

      $this->document->title = $this->data['text_folders'];

      if(isset($this->request->post['name']) && $this->request->post['name']) {
         $this->model_folder_folder->add_folder($this->request->post['name']);

         Header('Location: ' . SITE_URL . 'folders.php');
         return;
      }


      if(isset($this->request->get['id']) && $this->request->get['id'] > 0) {
         $this->model_folder_folder->remove_folder($this->request->get['id']);

         Header('Location: ' . SITE_URL . 'folders.php');
         return;
      }


      $this->data['page_len'] = get_page_length();

      $this->data['extra_folders'] = $this->model_folder_folder->get_folders_for_user();

      $this->render();
   }


   private function validate() {

      if(!isset($this->request->post['password']) || !isset($this->request->post['password2']) ) {
         $this->error['password'] = $this->data['text_missing_password'];
      }

      if(strlen(@$this->request->post['password']) < MIN_PASSWORD_LENGTH || strlen(@$this->request->post['password2']) < MIN_PASSWORD_LENGTH) {
         $this->error['password'] = $this->data['text_invalid_password'];
      }

      if($this->request->post['password'] != $this->request->post['password2']) {
         $this->error['password'] = $this->data['text_password_mismatch'];
      }


      if (!$this->error) {
         return true;
      } else {
         return false;
      }

   }



}

?>
