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
         if($this->validate() == true) {
            $rc = $this->model_policy_retention->add_new_rule($this->request->post);
         } else {
            $this->data['error'] = $lang->data['text_invalid_data'];
         }

      }

      $this->data['rules'] = $this->model_policy_retention->get_rules();


      $this->render();
   }


   private function validate() {
      if($this->request->post['days'] == '' || $this->request->post['days'] < 1) { return false; }

      if($this->request->post['from'] == '' && $this->request->post['to'] == '' &&
         $this->request->post['subject'] == '' && $this->request->post['size'] == '' &&
         $this->request->post['attachment_type'] == '' && $this->request->post['attachment_size'] == '' &&
         $this->request->post['spam'] == -1
      ) {
         return false;
      }

      return true;
   }


}

?>
