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

      $db = Registry::get('db');

      $this->load->model('user/auth');
      $this->load->model('user/user');
      $this->load->model('user/prefs');
      $this->load->model('folder/folder');

      if(ENABLE_SAAS == 1) {
         $this->load->model('saas/ldap');
      }

      $this->document->title = $this->data['text_login'];

      if($this->request->server['REQUEST_METHOD'] == 'POST' && $this->validate() == true) {

         if($this->model_user_auth->checkLogin($this->request->post['username'], $_POST['password']) == 1) {

            $this->model_user_prefs->get_user_preferences($_SESSION['username']);

            LOGGER('logged in');

            if(isAdminUser() == 1) {
               header("Location: " . SITE_URL . "index.php?route=health/health");
               exit;
            }

            if(isset($_POST['relocation']) && $_POST['relocation']) {
               header("Location: " . SITE_URL . $_POST['relocation']);
            } else {
               header("Location: " . SITE_URL . "search.php");
            }

            //header("Location: " . SITE_URL . "search.php");

            exit;
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


      if (!$this->error) {
         return true;
      } else {
         return false;
      }

   }


}

?>
