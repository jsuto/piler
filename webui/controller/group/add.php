<?php


class ControllerGroupAdd extends Controller {
   private $error = array();
   private $domains = array();

   public function index(){

      $this->id = "content";
      $this->template = "group/add.tpl";
      $this->layout = "common/layout";


      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('group/group');

      $this->document->title = $this->data['text_group_management'];

      /* check if we are admin */

      if(Registry::get('admin_user') == 1) {

         if($this->request->server['REQUEST_METHOD'] == 'POST') {
            $ret = 0;

            if($this->validate() == true){
               $ret = $this->model_group_group->add_group($this->request->post);

               if($ret == 1){
                  $this->data['x'] = $this->data['text_successfully_added'];
               } else {
                  $this->data['errorstring'] = $this->data['text_failed_to_add'] . ": " . $ret;
               }
            }
            else {
               $this->data['errorstring'] = array_pop($this->error);
            }

            if($ret == 0) {

               $this->data['post'] = $this->request->post;

            }
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

      if (!$this->error) {
         return true;
      } else {
         return false;
      }

   }



}

?>
