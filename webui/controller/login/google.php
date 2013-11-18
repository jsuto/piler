<?php


class ControllerLoginGoogle extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "login/login.tpl";
      $this->layout = "common/layout";


      $request = Registry::get('request');

      $db = Registry::get('db');
      $session = Registry::get('session');

      $this->load->model('user/auth');
      $this->load->model('user/user');
      $this->load->model('user/prefs');
      $this->load->model('user/google');
      $this->load->model('domain/domain');
      $this->load->model('folder/folder');

      $this->document->title = $this->data['text_login'];


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

      $oauth2 = new apiOauth2Service($client);


      if(isset($_GET['code'])) {
         $client->authenticate();
         $session->set("access_token", $client->getAccessToken());
         header('Location: ' . GOOGLE_REDIRECT_URL);
      }

      if($session->get("access_token")) {
         $client->setAccessToken($session->get("access_token"));
      }


      if($client->getAccessToken()) {
         $session->set("access_token", $client->getAccessToken());

         $token = json_decode($session->get("access_token"));

         if(isset($token->{'access_token'}) && isset($token->{'refresh_token'})) {
            $account = $oauth2->userinfo->get();

            $this->model_user_google->check_for_account($account);

            $this->model_user_google->update_tokens($account['email'], $account['id'], $token);

            header("Location: " . SITE_URL . "search.php");
            exit;
         }

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
