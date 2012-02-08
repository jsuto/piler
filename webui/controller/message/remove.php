<?php


class ControllerMessageRemove extends Controller {

   public function index(){

      $this->id = "content";
      $this->template = "message/remove.tpl";
      $this->layout = "common/layout-empty";

      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('search/search');
      $this->load->model('search/message');

      $this->load->model('user/user');

      $this->document->title = $this->data['text_message'];

      $this->data['id'] = @$this->request->get['id'];

      if(HOLD_EMAIL == 1) {
         AUDIT(ACTION_UNAUTHORIZED_REMOVE_MESSAGE, '', '', $this->data['id'], '');
         die("not authorized to remove id: " . $this->data['id']);
      }


      if(!verify_piler_id($this->data['id'])) {
         AUDIT(ACTION_UNKNOWN, '', '', $this->data['id'], 'unknown piler id: ' . $this->data['id']);
         die("invalid id: " . $this->data['id']);
      }

      if(Registry::get('admin_user') == 0) {
         AUDIT(ACTION_UNAUTHORIZED_REMOVE_MESSAGE, '', '', $this->data['id'], '');
         die("no permission for " . $this->data['id']);
      }


      AUDIT(ACTION_REMOVE_MESSAGE, '', '', $this->data['id'], '');


      if($this->model_search_search->remove_message($this->data['id']) == 1) {
         $this->data['data'] = $this->data['text_marked_for_removal'];
      } else {
         $this->data['data'] = $this->data['text_failed_to_mark_for_removal'];
         AUDIT(ACTION_REMOVE_MESSAGE, '', '', $this->data['id'], 'failed');
      }

      $this->render();
   }


}

?>
