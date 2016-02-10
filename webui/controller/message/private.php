<?php


class ControllerMessagePrivate extends Controller {

   public function index(){

      $this->id = "content";
      $this->template = "message/headers.tpl";
      $this->layout = "common/layout-empty";

      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('search/search');
      $this->load->model('search/message');

      $this->load->model('user/user');

      $this->document->title = $this->data['text_message'];

      $this->data['id'] = $this->request->post['id'];

      if(!verify_piler_id($this->data['id'])) {
         AUDIT(ACTION_UNKNOWN, '', '', $this->data['id'], 'unknown id: ' . $this->data['id']);
         die("invalid id: " . $this->data['id']);
      }

      if(!$this->model_search_search->check_your_permission_by_id($this->data['id'])) {
         AUDIT(ACTION_UNAUTHORIZED_VIEW_MESSAGE, '', '', $this->data['id'], '');
         die("no permission for " . $this->data['id']);
      }

      AUDIT(ACTION_MARK_AS_PRIVATE, '', '', $this->data['id'], '');

      $this->data['username'] = Registry::get('username');

      if(Registry::get('auditor_user') == 1) {
syslog(LOG_INFO, "hokamoka:" . $this->data['id']);

         $this->model_search_message->mark_as_private($this->data['id']);
      }


   }


}

?>
