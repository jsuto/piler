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

//print_r($this->request->post); exit;

      if(isset($this->request->post['save'])) {
         $a = preg_replace("/\&save=1$/", "", http_build_query($this->request->post));

         $this->model_search_search->add_search_term($a);
      }


      $this->data['searchterms'] = $this->model_search_search->get_search_terms();
//print_r($this->data['searchterms']); exit;

      $this->data['n'] = count($this->data['searchterms']);

      $this->data['select_size'] = $this->data['n'] + 1;
      if($this->data['select_size'] > 7) { $this->data['select_size'] = 7; }

      $this->render();
   }

}

?>
