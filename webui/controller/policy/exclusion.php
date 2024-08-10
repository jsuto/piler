<?php


class ControllerPolicyExclusion extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "policy/exclusion.tpl";
      $this->layout = "common/layout";


      $request = Registry::get('request');

      $db = Registry::get('db');
      $lang = Registry::get('language');

      $this->load->model('policy/exclusion');

      $this->document->title = $this->data['text_exclusion_rules'];

      $this->data['rules'] = array();

      $this->data['error'] = '';

      $this->data['search'] = '';

      $order = 'ASC';
      $sort = 'from';

      if(isset($this->request->post['search'])) { $this->data['search'] = $this->request->post['search']; }
      else if(isset($this->request->get['search'])) { $this->data['search'] = $this->request->get['search']; }

      if(isset($this->request->get['order'])) { $order = $this->request->get['order']; }
      if(isset($this->request->get['sort'])) { $sort = $this->request->get['sort']; }

      // toggle sort order
      $order == 'ASC' ? $order_to_display = 'DESC' : $order_to_display = 'ASC';

      $this->data['MEURL'] = $_SERVER['DOCUMENT_URI'] . '?route=policy/exclusion&order=' . $order_to_display;

      if(Registry::get('admin_user') == 0) {
         die("go away");
      }

      if($_SERVER['REQUEST_METHOD'] == 'POST') {
         if($this->validate() == true) {
            $rc = $this->model_policy_exclusion->add_new_rule($this->request->post);
         } else {
            $this->data['error'] = $lang->data['text_invalid_data'];
         }
      }

      $this->data['rules'] = htmlentities_on_array($this->model_policy_exclusion->get_rules($this->data['search'], $sort, $order));


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
