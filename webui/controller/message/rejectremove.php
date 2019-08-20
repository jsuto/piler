<?php


class ControllerMessageRejectRemove extends Controller {

   public function index(){

      $this->id = "content";
      $this->template = "audit/remove.tpl";
      $this->layout = "common/layout";

      $session = Registry::get('session');
      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('search/search');
      $this->load->model('search/message');
      $this->load->model('user/user');

      $this->document->title = $this->data['text_message'];

      if(!isset($this->request->post['id']) || $this->request->post['id'] == '') { die("no id parameter given"); }
      if(!isset($this->request->post['confirmed']) || $this->request->post['confirmed'] != 1) { die("not confirmed"); }
      if(!isset($this->request->post['reason2']) || $this->request->post['reason2'] == '') { die("no reason2 parameter given"); }

      $id = (int)$this->request->post['id'];

      if(!$this->model_search_search->check_your_permission_by_id($id)) {
         die("no permission for $id");
      }

      $this->data['username'] = Registry::get('username');

      if(Registry::get('data_officer') == 0) {
         die("go away");
      }

      // Shouldn't we ask for a token or something as well?

      $db->query("UPDATE " . TABLE_DELETED . " SET deleted=0, date2=?, reason2=? WHERE id=?", [NOW, $this->request->post['reason2'], $id]);
      syslog(LOG_INFO, $this->data['username'] . " rejected removing message: $id");

      $this->render();
   }

}
