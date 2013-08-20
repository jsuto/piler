<?php


class ControllerLoginSSO extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "login/login.tpl";
      $this->layout = "common/layout";


      $request = Registry::get('request');

      $db = Registry::get('db');

      $this->load->model('user/auth');
      $this->load->model('user/user');
      $this->load->model('user/prefs');
      $this->load->model('folder/folder');
      $this->load->model('domain/domain');

      $this->document->title = $this->data['text_login'];

      if($this->model_user_auth->check_ntlm_auth() == 1) {
         header("Location: " . SITE_URL . "search.php");
         exit;
      }

      die("permission denied");
   }

}

?>
