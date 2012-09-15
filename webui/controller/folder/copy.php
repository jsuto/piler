<?php


class ControllerFolderCopy extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "search/save.tpl";
      $this->layout = "common/layout-empty";


      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('folder/folder');

      $this->model_folder_folder->copy_message_to_folder_by_id($this->request->post['folder_id'], $this->request->post['id']);


      //$this->render();
   }

}

?>
