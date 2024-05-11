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
      if(!isset($this->request->post['reason']) || $this->request->post['reason'] == '') { die("no reason parameter given"); }

      $idlist = $this->model_search_search->check_your_permission_by_id_list(explode(",", $this->request->post['idlist']));

      $this->data['removed'] = 0;

      $this->data['username'] = Registry::get('username');

      if(Registry::get('auditor_user') == 0) {
         die("go away");
      }

      if(NEED_TO_APPROVE_DELETE) {
         $deleted = -1;
      } else {
         $deleted = 1;
      }

      foreach($idlist as $id) {
         $db->query("INSERT INTO " . TABLE_DELETED . " (id, requestor, reason1, date1, deleted) VALUES(?,?,?,?,?)", [$id, $this->data['username'], $this->request->post['reason'], NOW, $deleted]);

         if(NEED_TO_APPROVE_DELETE) {
            AUDIT(ACTION_MARK_MESSAGE_FOR_REMOVAL, '', '', $id, '');
            syslog(LOG_INFO, $this->data['username'] . " marked message for removal: $id");
         } else {
            AUDIT(ACTION_REMOVE_MESSAGE, '', '', $id, '');
            $db->query("UPDATE " . TABLE_META . " SET retained=? WHERE id=?", [NOW, $id]);
            if(RT) {
               $sphxrw = Registry::get('sphxrw');
               $sphxrw->query("DELETE FROM " . SPHINX_MAIN_INDEX . " WHERE id=" . (int)$id);
            }

            syslog(LOG_INFO, $this->data['username'] . " removed message: $id");
         }

         $this->data['removed']++;
      }

      $this->render();
   }


}
