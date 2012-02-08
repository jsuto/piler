<?php


class ControllerPolicyArchiving extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "policy/archiving.tpl";
      $this->layout = "common/layout";


      $request = Registry::get('request');

      $db = Registry::get('db');

      $this->load->model('policy/archiving');

      $this->document->title = $this->data['text_archiving_rules'];

      $this->data['rules'] = array();


      if($_SERVER['REQUEST_METHOD'] == 'POST') {
         $rc = $this->model_policy_archiving->add_new_rule($this->request->post);

      }

      $this->data['rules'] = $this->model_policy_archiving->get_rules();

//print_r($this->data['rules']);


      $this->render();
   }


}

?>
