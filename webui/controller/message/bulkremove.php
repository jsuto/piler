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
      //$this->load->model('message/remove');

      $this->load->model('user/user');


      $this->document->title = $this->data['text_message'];

      if(!isset($this->request->post['idlist']) || $this->request->post['idlist'] == '') { die("no idlist parameter given"); }

      $idlist = $this->model_search_search->check_your_permission_by_id_list(explode(",", $this->request->post['idlist']));


      $this->data['username'] = Registry::get('username');

      $this->model_search_message->connect_to_pilergetd();

      foreach($idlist as $id) {

         AUDIT(ACTION_REMOVE_MESSAGE, '', '', $id, '');

         $piler_id = $this->model_search_message->get_piler_id_by_id($id);

syslog(LOG_INFO, "removing $piler_id");

         $x = 1;

         if($x == 1) { $this->data['removed']++; }
      }

      $this->model_search_message->disconnect_from_pilergetd();

      $this->render();
   }


}

?>
