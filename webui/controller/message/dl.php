<?php


class ControllerMessageDl extends Controller {

   public function index(){

      $this->id = "content";
      $this->template = "message/headers.tpl";
      $this->layout = "common/layout-empty";

      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('search/search');
      $this->load->model('search/message');
      $this->load->model('message/restore');

      $session = Registry::get('session');
      $a = $session->get('last_search');
      if($a) {
         $a = unserialize($a);
      }

      $idlist = $this->model_search_search->check_your_permission_by_id_list($a);
      print implode(",", $idlist);

   }


}

?>
