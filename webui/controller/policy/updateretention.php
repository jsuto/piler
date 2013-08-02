<?php


class ControllerPolicyUpdateRetention extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "policy/updateretention.tpl";
      $this->layout = "common/layout";


      $request = Registry::get('request');

      $db = Registry::get('db');

      $this->load->model('policy/retention');

      $this->document->title = $this->data['text_retention_rules'];

      if(Registry::get('admin_user') == 0) {
         die("go away");
      }

      $this->data['n'] = 0;
      $this->data['domain'] = '';
      $this->data['days'] = 0;

      if(isset($this->request->get['domain']) && isset($this->request->get['days'])) {
         $this->data['domain'] = $this->request->get['domain'];
         $this->data['days'] = $this->request->get['days'];

         $this->data['n'] = $this->model_policy_retention->update_retention_time($this->data);
      }

      $this->render();
   }

}

?>
