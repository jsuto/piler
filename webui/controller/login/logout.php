<?php


class ControllerLoginLogout extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "login/logout.tpl";
      $this->layout = "common/layout";

      $request = Registry::get('request');

      $db = Registry::get('db');

      $this->load->model('user/auth');

      $this->document->title = $this->data['text_logout'];

      logout();

      $this->render();
   }


}

?>
