<?php


class ControllerPolicyFolder extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "policy/folder.tpl";
      $this->layout = "common/layout";


      $request = Registry::get('request');

      $db = Registry::get('db');
      $lang = Registry::get('language');

      $this->load->model('policy/folder');
      $this->load->model('folder/folder');

      $this->document->title = $this->data['text_folder_rules'];

      $this->data['rules'] = array();

      $this->data['error'] = '';

      $this->data['search'] = '';

      if(isset($this->request->post['search'])) { $this->data['search'] = $this->request->post['search']; }
      else if(isset($this->request->get['search'])) { $this->data['search'] = $this->request->get['search']; }

      if(Registry::get('admin_user') == 0) {
         die("go away");
      }

      if($_SERVER['REQUEST_METHOD'] == 'POST') {
         if($this->validate() == true) {
            $rc = $this->model_policy_folder->add_new_rule($this->request->post);
         } else {
            $this->data['error'] = $lang->data['text_invalid_data'];
         }
      }

      $this->data['rules'] = $this->model_policy_folder->get_rules($this->data['search']);

      $this->data['folders'] = $this->model_folder_folder->get_top_level_folders();

      $this->render();
   }


   private function validate() {
      if($this->request->post['from'] == '' && $this->request->post['to'] == '' &&
         $this->request->post['subject'] == '' && $this->request->post['body'] == '' && $this->request->post['size'] == '' &&
         $this->request->post['attachment_name'] == '' && $this->request->post['attachment_type'] == '' && $this->request->post['attachment_size'] == '' &&
         $this->request->post['spam'] == -1
      ) {
         return false;
      }

      return true;
   }

}

?>
