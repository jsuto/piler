<?php


class ControllerMessageFolder extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "message/note.tpl";
      $this->layout = "common/layout-empty";

      $session = Registry::get('session');
      $request = Registry::get('request');
      $db = Registry::get('db');
      $sphx = Registry::get('sphx');

      $this->load->model('search/search');
      $this->load->model('folder/folder');

      if(isset($this->request->post['folder_id']) && isset($this->request->post['id'])) {

         if($this->model_search_search->check_your_permission_by_id($this->request->post['id']) == 1) {
            $this->model_folder_folder->update_message_folder($this->request->post['id'], $this->request->post['folder_id']);
         }
      }
   }

}

?>
