<?php


class ControllerLoginGA extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "login/ga.tpl";
      $this->layout = "common/layout-empty";


      $request = Registry::get('request');
      $session = Registry::get('session');

      $db = Registry::get('db');

      $this->load->model('user/auth');
      $this->load->model('user/user');
      $this->load->model('user/prefs');

      if(ENABLE_SAAS == 1) {
         $this->load->model('saas/ldap');
         $this->load->model('saas/customer');
      }

      require(DIR_BASE . 'system/helper/PHPGangsta_GoogleAuthenticator.php');

      $this->data['title'] = $this->data['text_login'];
      $this->data['title_prefix'] = TITLE_PREFIX;

      $this->data['failed_login_count'] = $this->model_user_auth->get_failed_login_count();


      if($this->request->server['REQUEST_METHOD'] == 'POST' && $this->validate() == true) {

         $GA = new PHPGangsta_GoogleAuthenticator();

         $settings = $this->model_user_prefs->get_ga_settings($session->get('username'));

         if(strlen($this->request->post['ga_code']) > 5 && $GA->verifyCode($settings['ga_secret'], $this->request->post['ga_code'], 2)) {

            $session->set("ga_block", "");

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
         else {
            $this->model_user_auth->increment_failed_login_count($this->data['failed_login_count']);
            $this->data['failed_login_count']++;
         }

         $this->data['x'] = $this->data['text_invalid_pin_code'];

      }

      $this->render();
   }


   private function validate() {

      if(!isset($this->request->post['ga_code'])){
         $this->error['ga_code'] = $this->data['text_invalid_data'];
      }


      if (!$this->error) {
         return true;
      } else {
         return false;
      }

   }


}

?>
