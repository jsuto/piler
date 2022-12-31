<?php


class ControllerMessageNote extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "message/note.tpl";
      $this->layout = "common/layout-empty";

      $session = Registry::get('session');
      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('search/search');
      $this->load->model('search/message');

      if(isset($this->request->post['note']) && isset($this->request->post['id'])) {

         if($this->model_search_search->check_your_permission_by_id($this->request->post['id']) == 1) {
            if(RT) {
               $this->model_search_message->add_message_rt_note($this->request->post['id'], $session->get("uid"), urldecode($this->request->post['note']));
            } else {
               $this->model_search_message->add_message_note($this->request->post['id'], $session->get("uid"), urldecode($this->request->post['note']));
            }
         }
      }
   }

}
