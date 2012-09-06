<?php


class ControllerMessageNote extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "message/note.tpl";
      $this->layout = "common/layout-empty";


      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('search/search');
      $this->load->model('search/message');

      if(isset($this->request->post['note']) && isset($this->request->post['id'])) {

         if($this->model_search_search->check_your_permission_by_id($this->request->post['id']) == 1) {
            $this->model_search_message->add_message_note($this->request->post['id'], $_SESSION['uid'], $this->request->post['note']);
         }
      }
   }

}

?>
