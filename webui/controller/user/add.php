<?php


class ControllerUserAdd extends Controller {
   private $error = array();
   private $domains = array();

   public function index(){

      $this->id = "content";
      $this->template = "user/add.tpl";
      $this->layout = "common/layout";


      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('user/user');
      $this->load->model('group/group');

      $this->document->title = $this->data['text_add_new_user_alias'];

      $this->data['domains'] = array();

      /* check if we are admin */

      if(Registry::get('admin_user') == 1) {

         /* query available domains */

         $this->data['domains'] = $this->model_user_user->get_domains();

         $this->domains = $this->model_user_user->get_email_domains();


         if($this->request->server['REQUEST_METHOD'] == 'POST') {
            $ret = 0;

            if($this->validate() == true){
               $ret = $this->model_user_user->add_user($this->request->post);

               $_SESSION['last_domain'] = $this->request->post['domain'];

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
               $this->data['next_user_id'] = $this->model_user_user->get_next_uid();

            }
         }
         else {
            $this->data['next_user_id'] = $this->model_user_user->get_next_uid();
            $this->data['groups'] = $this->model_group_group->get_groups();
         }
      }
      else {
         $this->template = "common/error.tpl";
         $this->data['errorstring'] = $this->data['text_you_are_not_admin'];
      }




      $this->render();
   }


   private function validate() {

      if(!isset($this->request->post['password']) || !isset($this->request->post['password2']) ) {
         $this->error['password'] = $this->data['text_missing_password'];
      }

      if(strlen(@$this->request->post['password']) < MIN_PASSWORD_LENGTH || strlen(@$this->request->post['password2']) < MIN_PASSWORD_LENGTH) {
         $this->error['password'] = $this->data['text_too_short_password'];
      }

      if($this->request->post['password'] != $this->request->post['password2']) {
         $this->error['password'] = $this->data['text_password_mismatch'];
      }

      if(!isset($this->request->post['uid']) || !is_numeric($this->request->post['uid']) || $this->request->post['uid'] < 0) {
         $this->error['uid'] = $this->data['text_invalid_uid'];
      }

      if(!isset($this->request->post['email']) || strlen($this->request->post['email']) < 3) {
         $this->error['email'] = $this->data['text_invalid_email'];
      }
      else {
         $emails = explode("\n", $this->request->post['email']);
         foreach ($emails as $email) {
            $email = rtrim($email);
            $ret = checkemail($email, $this->domains);
            if($ret == 0) {
               $this->error['email'] = $this->data['text_invalid_email'] . ": $email";
            }
            else if($ret == -1) {
               $this->error['email'] = $this->data['text_email_in_unknown_domain'] . ": $email";
            }
         }
      }

      if(!isset($this->request->post['username']) || strlen($this->request->post['username']) < 2) {
         $this->error['username'] = $this->data['text_invalid_username'];
      }

      if(isset($this->request->post['username']) && $this->model_user_user->get_uid_by_name($this->request->post['username']) > 0) {
         $this->error['username'] = $this->data['text_existing_user'];
      }

      if(!isset($this->request->post['domain'])) {
         $this->error['domain'] = $this->data['text_missing_data'];
      }


      if (!$this->error) {
         return true;
      } else {
         return false;
      }

   }



}

?>
