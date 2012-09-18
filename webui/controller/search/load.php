<?php


class ControllerSearchLoad extends Controller {
   private $error = array();


   public function index(){

      $this->id = "content";
      $this->template = "search/load.tpl";
      $this->layout = "common/layout-empty";


      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('search/search');
      $this->load->model('search/message');
      $this->load->model('user/user');

      $this->data['terms'] = $this->model_search_search->get_search_terms();

      $this->render();
   }

}


?>
