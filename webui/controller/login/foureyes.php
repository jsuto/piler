<?php


class ControllerLoginFoureyes extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "login/foureyes.tpl";
      $this->layout = "common/layout-empty";


      if(Registry::get('username')) {
         header('Location: ' . SITE_URL . 'search.php');
         exit;
      }


      $request = Registry::get('request');
      $session = Registry::get('session');

      $db = Registry::get('db');

      $this->load->model('user/auth');
      $this->load->model('user/user');
      $this->load->model('user/prefs');
      $this->load->model('domain/domain');
      $this->load->model('folder/folder');

      if(ENABLE_SAAS == 1) {
         $this->load->model('saas/ldap');
         $this->load->model('saas/customer');
      }

      $this->data['title'] = $this->data['text_login'];
      $this->data['title_prefix'] = TITLE_PREFIX;

      $this->data['failed_login_count'] = $this->model_user_auth->get_failed_login_count();

      $data = $session->get("auth_data");

      if($this->request->server['REQUEST_METHOD'] == 'POST' && $this->validate() == true) {

         if($this->model_user_auth->checkLogin($this->request->post['username'], $_POST['password']) == 1) {

            // check if the 2nd login is indeed an admin

            $data2 = $session->get("auth_data");
            if(!isset($data2['admin_user']) || $data2['admin_user'] != 1) {
               syslog(LOG_INFO, "user " . $data2['username'] . " is not an admin user");

               $this->model_user_auth->increment_failed_login_count($this->data['failed_login_count']);
               $this->data['failed_login_count']++;

               $session->set("auth_data", $data);
            }
            else {

               $session->remove("four_eyes");

               $this->model_user_auth->apply_user_auth_session($data);
               $session->remove("auth_data");

               $this->model_user_prefs->get_user_preferences($session->get('username'));

               if(ENABLE_SAAS == 1) {
                  $this->model_saas_customer->online($session->get('email'));
               }

               LOGGER('logged in');

               if(isAdminUser() == 1) {
                  header("Location: " . SITE_URL . "index.php?route=health/health");
                  exit;
               }

               header("Location: " . SITE_URL . "search.php");
               exit;
            }

         }
         else {
            $this->model_user_auth->increment_failed_login_count($this->data['failed_login_count']);
            $this->data['failed_login_count']++;
         }

         $this->data['x'] = $this->data['text_invalid_email_or_password'];

      }


      $this->render();
   }


   private function check_admin_account() {

      if($this->model_user_auth->checkLogin($this->request->post['username2'], $_POST['password2']) != 1 || isAdminUser() != 1) {
         $this->error['username'] = 'failed admin login';
         return 0;
      }
      else {
         $this->model_user_auth->reset_user_auth_session();
         return 1;
      }

   }


   private function validate() {

      if(strlen($this->request->post['username']) < 2){
         $this->error['username'] = $this->data['text_invalid_username'];
      }


      if(CAPTCHA_FAILED_LOGIN_COUNT > 0 && $this->data['failed_login_count'] > CAPTCHA_FAILED_LOGIN_COUNT) {
         require_once $_SERVER['DOCUMENT_ROOT'] . '/securimage/securimage.php';
         $image = new Securimage();

         if($image->check($this->request->post['captcha']) != true) {
            $this->error['captcha'] = 'captcha error';
         }
      }


      if (!$this->error) {
         return true;
      } else {
         return false;
      }

   }


}

?>
