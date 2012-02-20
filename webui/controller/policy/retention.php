<?php


class ControllerPolicyRetention extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "policy/retention.tpl";
      $this->layout = "common/layout";


      $request = Registry::get('request');

      $db = Registry::get('db');

      $this->load->model('policy/retention');

      $this->document->title = $this->data['text_retention_rules'];

      $this->data['rules'] = array();

      if(Registry::get('admin_user') == 0) {
         die("go away");
      }

      if($_SERVER['REQUEST_METHOD'] == 'POST') {
         $rc = $this->model_policy_retention->add_new_rule($this->request->post);
      }

      $this->data['rules'] = $this->model_policy_retention->get_rules();


      $this->render();
   }


}

?>
