<?php


class ControllerGroupEdit extends Controller {
   private $error = array();
   private $domains = array();

   public function index(){
      $this->data['id'] = 0;

      $this->id = "content";
      $this->template = "group/edit.tpl";
      $this->layout = "common/layout";


      $request = Registry::get('request');
      $db = Registry::get('db');
      $language = Registry::get('language');

      $this->load->model('group/group');


      $this->document->title = $language->get('text_edit_group');

      $this->data['domains'] = array();


      if(isset($this->request->get['id']) && is_numeric($this->request->get['id']) && $this->request->get['id'] > 0) {
         $this->data['id'] = $this->request->get['id'];
      }

      if(isset($this->request->post['id']) && is_numeric($this->request->post['id']) && $this->request->post['id'] > 0) {
         $this->data['id'] = $this->request->post['id'];
      }


      /* check if we are admin */

      if(Registry::get('admin_user') == 1) {

         if($this->request->server['REQUEST_METHOD'] == 'POST') {

            if($this->validate() == true){

               $ret = $this->model_group_group->update_group($this->request->post);

               if($ret == 1){
                  $this->data['x'] = $this->data['text_successfully_modified'];
                  header('Location: ' . SITE_URL . 'index.php?route=group/list');
               } else {
                  $this->template = "common/error.tpl";
                  $this->data['errorstring'] = $this->data['text_failed_to_modify'] . ": " . $ret;
               }

            }
            else {
               $this->data['errorstring'] = array_pop($this->error);
            }
         }
         else {
            $this->data['group'] = $this->model_group_group->get_domain_by_id($this->data['id']);
            $this->data['email'] = $this->model_group_group->get_emails_by_group_id($this->data['id']);
            $this->data['assigned_email'] = $this->model_group_group->get_assigned_emails_by_group_id($this->data['id']);
         }
      }
      else {
         $this->template = "common/error.tpl";
         $this->data['errorstring'] = $this->data['text_you_are_not_admin'];
      }




      $this->render();
   }


   private function validate() {

      if(!isset($this->request->post['groupname'])) {
         $this->error['group'] = $this->data['text_missing_data'];
      }

      if(!isset($this->request->post['email']) || $this->request->post['email'] == '') {
         $this->error['email'] = $this->data['text_missing_data'];
      }

      if(!isset($this->request->post['assigned_email']) || $this->request->post['assigned_email'] == '') {
         $this->error['assigned_email'] = $this->data['text_missing_data'];
      }

      if(!isset($this->request->post['id']) || !is_numeric($this->request->post['id']) || (int)$this->request->post['id'] < 0) {
         $this->error['id'] = $this->data['text_invalid_data'];
      }


      if (!$this->error) {
         return true;
      } else {
         return false;
      }

   }



}

?>
