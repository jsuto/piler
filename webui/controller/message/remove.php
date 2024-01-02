<?php


class ControllerMessageRemove extends Controller {

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

      if(!isset($this->request->get['id']) || $this->request->get['id'] == '') { die("no id parameter given"); }
      if(!isset($this->request->get['confirmed']) || $this->request->get['confirmed'] != 1) { die("not confirmed"); }

      $id = (int)$this->request->get['id'];

      // FIXME: For the moment data officer has no permission to see the message
      if(!$this->model_search_search->check_your_permission_by_id($id)) {
         die("no permission for $id");
      }

      $this->data['username'] = Registry::get('username');

      if(Registry::get('data_officer') == 0) {
         die("go away");
      }

      // Shouldn't we ask for a token or something as well?

      AUDIT(ACTION_REMOVE_MESSAGE, '', '', $id, '');
      $db->query("UPDATE " . TABLE_META . " SET retained=? WHERE id=?", [NOW, $id]);
      $db->query("UPDATE " . TABLE_DELETED . " SET deleted=1 WHERE id=?", [$id]);
      if(RT) {
         $sphxrw = Registry::get('sphxrw');
         $sphxrw->query("DELETE FROM " . SPHINX_MAIN_INDEX . " WHERE id=", (int)$this->data['id']);
      }

      syslog(LOG_INFO, $this->data['username'] . " removed message: $id");

      $this->data['x'] = $this->data['text_successfully_removed'];

      $this->render();
   }


}
