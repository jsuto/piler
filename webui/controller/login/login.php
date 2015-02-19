<?php


class ControllerLoginLogin extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "login/login.tpl";
      $this->layout = "common/layout-empty";


      if(Registry::get('username')) {
         header("Location: search.php");
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


      if($this->request->server['REQUEST_METHOD'] == 'POST' && $this->validate() == true) {

         if($this->model_user_auth->checkLogin($this->request->post['username'], $_POST['password']) == 1) {

            if($session->get("ga_block") == 1) {
               header("Location: " . SITE_URL . "index.php?route=login/ga");
               exit;
            }
            else {
               $data = $session->get("auth_data");
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


      if(ENABLE_GOOGLE_LOGIN == 1) {
         $client = new apiClient();
         $client->setApplicationName(GOOGLE_APPLICATION_NAME);

         $client->setScopes(array(
            'https://www.googleapis.com/auth/userinfo.email',
            'https://www.googleapis.com/auth/userinfo.profile',
            'https://mail.google.com/',
         ));

         $client->setClientId(GOOGLE_CLIENT_ID);
         $client->setClientSecret(GOOGLE_CLIENT_SECRET);
         $client->setRedirectUri(GOOGLE_REDIRECT_URL);
         $client->setDeveloperKey(GOOGLE_DEVELOPER_KEY);

         $this->data['auth_url'] = $client->createAuthUrl();
      }


      $this->render();
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
