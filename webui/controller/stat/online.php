<?php


class ControllerStatOnline extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "stat/online.tpl";
      $this->layout = "common/layout";


      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('saas/customer');

      $this->document->title = $this->data['text_statistics'];

      if(Registry::get('admin_user') == 0) {
         die("go away");
      }

      $this->data['users'] = $this->model_saas_customer->get_online_users();

      $this->render();
   }


}

?>
