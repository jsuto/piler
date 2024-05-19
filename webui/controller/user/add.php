<?php


class ControllerUserAdd extends Controller {
   private $error = array();
   private $domains = array();

   public function index(){

      $this->id = "content";
      $this->template = "user/add.tpl";
      $this->layout = "common/layout";

      $session = Registry::get('session');
      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('user/user');
      $this->load->model('group/group');
      $this->load->model('folder/folder');

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

               $session->set("last_domain", $this->request->post['domain']);

               if($ret == 1){
                  $this->data['x'] = $this->data['text_successfully_added'];
		  $this->data['next_user_id'] = $this->model_user_user->get_next_uid();
		  header('Location:' . SITE_URL . 'index.php?route=user/list');
               } else {
                  $this->data['errorstring'] = $this->data['text_failed_to_add'] . ": " . $ret;
               }
            }
            else {
               $this->data['errorstring'] = $this->data['text_error_message'];
               $this->data['errors'] = $this->error;
            }

            if($ret == 0) {

               $this->data['post'] = $this->request->post;
               $this->data['next_user_id'] = $this->model_user_user->get_next_uid();

            }
         }
         else {
            $this->data['next_user_id'] = $this->model_user_user->get_next_uid();
            // not sure these are needed
            $this->data['groups'] = $this->model_group_group->get_groups();
            $this->data['folders'] = $this->model_folder_folder->get_folders();
         }
      }
      else {
         $this->template = "common/error.tpl";
         $this->data['errorstring'] = $this->data['text_you_are_not_admin'];
      }

      $this->data['last_domain'] = $session->get("last_domain");

      $this->render();
   }


private function validate() {
      //password is required and must be greater than the MIN_PASSWORD_LENGTH
      if(!isset($this->request->post['password'])) {
         $this->error['password'] = $this->data['text_missing_password'];
      } elseif (strlen(@$this->request->post['password']) < MIN_PASSWORD_LENGTH) {
         $this->error['password'] = $this->data['text_too_short_password'];
      }
      //password2 is required and must be greater than the MIN_PASSWORD_LENGTH
      if(!isset($this->request->post['password2'])) {
         $this->error['password2'] = $this->data['text_missing_password'];
      } elseif (strlen(@$this->request->post['password2']) < MIN_PASSWORD_LENGTH) {
         $this->error['password2'] = $this->data['text_too_short_password'];
      }
      //passwords must match (put here to override the password2 missing message, if also present)
      if($this->request->post['password'] != $this->request->post['password2']) {
         $this->error['password2'] = $this->data['text_password_mismatch'];
      }
      //uid is required and must be numeric & 0 or greater
      if(!isset($this->request->post['uid']) || !is_numeric($this->request->post['uid']) || $this->request->post['uid'] < 0) {
         $this->error['uid'] = $this->data['text_invalid_uid'];
      }
      //email address is required and must be in the proper format
      if(!isset($this->request->post['email']) || strlen($this->request->post['email']) < 3) {
         $this->error['email'] = $this->data['text_invalid_email'];
      }
      else {
         $emails = explode("\n", $this->request->post['email']);
         foreach ($emails as $email) {
            $email = strtolower(rtrim($email));
            if($email == '') { continue; }

            $ret = checkemail($email, $this->domains);
            if($ret == 0) {
               $this->error['email'] = $this->data['text_invalid_email'] . ": *$email*";
            }
            else if($ret == -1) {
               $this->error['email'] = $this->data['text_email_in_unknown_domain'] . ": *$email*";
            }
         }
      }
      //username is required and must be greater than 2 chars
      if(!isset($this->request->post['username']) || strlen($this->request->post['username']) < 2) {
         $this->error['username'] = $this->data['text_invalid_username'];
      }
      //username is required and must be unique
      if(isset($this->request->post['username']) && $this->model_user_user->get_uid_by_name($this->request->post['username']) > 0) {
         $this->error['username'] = $this->data['text_existing_user'];
      }
      //primary domain is required
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
