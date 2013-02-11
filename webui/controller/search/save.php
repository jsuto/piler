<?php


class ControllerSearchSave extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "search/save.tpl";
      $this->layout = "common/layout-empty";


      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('search/search');

      if(isset($this->request->post['save'])) {
         $a = preg_replace("/\&save=1$/", "", http_build_query($this->request->post));

         $this->model_search_search->add_search_term($a);
      }

   }

}

?>
