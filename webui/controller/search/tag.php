<?php


class ControllerSearchTag extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "search/tag.tpl";
      $this->layout = "common/layout-empty";


      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('search/search');
      $this->load->model('search/message');

      if(isset($this->request->post['tag']) && isset($this->request->post['idlist'])) {
         $idlist = explode(",", $this->request->post['idlist']);

         if(count($idlist) > 0) {

            $q = '';

            $ids = $this->model_search_search->check_your_permission_by_id_list($idlist);

            for($i=0; $i<count($ids); $i++) { $q .= ",?"; }
            $q = preg_replace("/^\,/", "", $q);

            $this->model_search_message->bulk_add_message_tag($ids, $_SESSION['uid'], urldecode($this->request->post['tag']), $q);
         }
      }
   }

}

?>
