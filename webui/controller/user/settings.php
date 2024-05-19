<?php


class ControllerUserSettings extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "user/settings.tpl";
      $this->layout = "common/layout";


      $request = Registry::get('request');
      $session = Registry::get('session');

      $db = Registry::get('db');

      $this->load->model('user/auth');
      $this->load->model('user/prefs');
      $this->load->model('user/user');
      $this->load->model('group/group');

      require(DIR_BASE . 'system/helper/PHPGangsta_GoogleAuthenticator.php');

      $this->data['ga'] = $this->model_user_prefs->get_ga_settings($session->get('username'));

      $this->document->title = $this->data['text_settings'];

      $d = $r = '';
      $auditemails = $auditdomains = $auditgroups = $auditfolders = '';

      $auditemails = implode(", ", $session->get("emails"));

      $_auditdomains = $session->get("auditdomains");

      foreach($_auditdomains as $d) {
         $auditdomains .= ', ' . $d;
      }
      $auditdomains = preg_replace("/^,\s/", "", $auditdomains);

      $auditgroups = preg_replace("/\n/", ", ", $this->model_group_group->get_groups_by_email($session->get("emails")));

      $folders = $session->get("folders");

      foreach ($folders as $r) {
         $auditfolders .= ', ' . $r;
      }
      $auditfolders = preg_replace("/^,\s/", "", $auditfolders);

      if($auditemails) { $this->data['emails'] = $auditemails; } else { $this->data['emails'] = $this->data['text_none_found']; }
      if($auditdomains) { $this->data['domains'] = $auditdomains; } else { $this->data['domains'] = $this->data['text_none_found']; }
      if($auditgroups) { $this->data['groups'] = $auditgroups; } else { $this->data['groups'] = $this->data['text_none_found']; }
      if($auditfolders) { $this->data['folders'] = $auditfolders; } else { $this->data['folders'] = $this->data['text_none_found']; }

      $this->data['wildcard_domains'] = $session->get("wildcard_domains");
      if($this->data['wildcard_domains']) {
         $this->data['wildcard_domains'] = implode(", ", $this->data['wildcard_domains']);
      }

      if(isset($this->request->post['pagelen']) && isset($this->request->post['theme'])) {
         $this->model_user_prefs->set_user_preferences(Registry::get('username'), $this->request->post);

         AUDIT(ACTION_CHANGE_USER_SETTINGS, '', '', '', 'pagelen:' . $this->request->post['pagelen'] . ', theme:' . $this->request->post['theme'] . ', lang:' . $this->request->post['lang']);

         if(isAdminUser() == 1) {
            header("Location: " . SITE_URL . "index.php?route=health/health");
            return;
         }

         header("Location: " . SITE_URL . "search.php");
         return;
      }


      if($this->request->server['REQUEST_METHOD'] == 'POST' && PASSWORD_CHANGE_ENABLED == 1 && $this->validate() == true) {

         if($this->model_user_auth->change_password(Registry::get('username'), $this->request->post['password']) == 1) {
            $this->data['x'] = $this->data['text_password_changed'];
         }
         else {
            $this->data['x'] = $this->data['text_failed_to_change_password'];
         }
      }


      $this->data['page_len'] = get_page_length();
      $this->data['theme'] = $session->get("theme");
      $this->data['lang'] = $session->get("lang");

      $this->render();
   }


   private function validate() {

      if(!isset($this->request->post['password']) || !isset($this->request->post['password2']) ) {
         $this->error['password'] = $this->data['text_missing_password'];
      }

      if(strlen(@$this->request->post['password']) < MIN_PASSWORD_LENGTH || strlen(@$this->request->post['password2']) < MIN_PASSWORD_LENGTH) {
         $this->error['password'] = $this->data['text_invalid_password'];
      }

      if($this->request->post['password'] != $this->request->post['password2']) {
         $this->error['password'] = $this->data['text_password_mismatch'];
      }


      if (!$this->error) {
         return true;
      } else {
         return false;
      }

   }



}
