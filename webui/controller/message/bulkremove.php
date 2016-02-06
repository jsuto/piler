<?php


class ControllerMessageBulkremove extends Controller {

   public function index(){

      $this->id = "content";
      $this->template = "message/bulkremove.tpl";
      $this->layout = "common/layout-empty";

      $session = Registry::get('session');
      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('search/search');
      $this->load->model('search/message');
      $this->load->model('user/user');


      $this->document->title = $this->data['text_message'];

      if(!isset($this->request->post['idlist']) || $this->request->post['idlist'] == '') { die("no idlist parameter given"); }

      $idlist = $this->model_search_search->check_your_permission_by_id_list(explode(",", $this->request->post['idlist']));

      $this->data['removed'] = 0;

      $this->data['username'] = Registry::get('username');

      if(Registry::get('auditor_user') == 0) {
         die("go away");
      }

      foreach($idlist as $id) {

         AUDIT(ACTION_REMOVE_MESSAGE, '', '', $id, '');

         $db->query("UPDATE " . TABLE_META . " SET retained=? WHERE id=?", array(NOW, $id));

         syslog(LOG_INFO, $this->data['username'] . " removed message: $id");

         $this->data['removed']++;
      }

      $this->render();
   }


}

?>
