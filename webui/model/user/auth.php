<?php

class ModelUserAuth extends Model {

   public function checkLogin($username = '', $password = '') {
      $ok = 0;

      if(ENABLE_IMAP_AUTH == 1) {
         require 'Zend/Mail/Protocol/Imap.php';
         $ok = $this->checkLoginAgainstIMAP($username, $password);
      }

      $query = $this->db->query("SELECT " . TABLE_USER . ".username, " . TABLE_USER . ".uid, " . TABLE_USER . ".realname, " . TABLE_USER . ".dn, " . TABLE_USER . ".password, " . TABLE_USER . ".isadmin, " . TABLE_USER . ".domain FROM " . TABLE_USER . ", " . TABLE_EMAIL . " WHERE " . TABLE_EMAIL . ".email=? AND " . TABLE_EMAIL . ".uid=" . TABLE_USER . ".uid", array($username));

      if(!isset($query->row['password'])) { return 0; }

      $pass = crypt($password, $query->row['password']);

      if($pass == $query->row['password']){
         $ok = 1;

         AUDIT(ACTION_LOGIN, $username, '', '', 'successful auth against user table');
      }
      else {
         AUDIT(ACTION_LOGIN_FAILED, $username, '', '', 'failed auth against user table');
      }

      if($ok == 0 && strlen($query->row['dn']) > 3) {
         $ok = $this->checkLoginAgainstLDAP($query->row, $password);
      }


      if($ok == 1) {
         $_SESSION['username'] = $query->row['username'];
         $_SESSION['uid'] = $query->row['uid'];
         $_SESSION['admin_user'] = $query->row['isadmin'];
         $_SESSION['email'] = $username;
         $_SESSION['domain'] = $query->row['domain'];
         $_SESSION['realname'] = $query->row['realname'];

         $_SESSION['auditdomains'] = $this->model_user_user->get_users_all_domains($query->row['uid']);
         $_SESSION['emails'] = $this->model_user_user->get_users_all_email_addresses($query->row['uid']);
         $_SESSION['folders'] = $this->model_folder_folder->get_all_folder_ids($query->row['uid']);
         $_SESSION['extra_folders'] = $this->model_folder_folder->get_all_extra_folder_ids($query->row['uid']);

         return 1;
      }


      return 0;
   }


   private function checkLoginAgainstLDAP($user = array(), $password = '') {
      if($password == '' || !isset($user['username']) || !isset($user['domain']) || !isset($user['dn']) || strlen($user['domain']) < 2){ return 0; }

      $query = $this->db->query("SELECT remotehost, basedn FROM " . TABLE_REMOTE . " WHERE remotedomain=?", array($user['domain']));

      if($query->num_rows != 1) { return 0; }

      $ldap = new LDAP($query->row['remotehost'], $user['dn'], $password);

      if($ldap->is_bind_ok()) {
         $this->change_password($user['username'], $password);

         AUDIT(ACTION_LOGIN, $user['username'], '', '', 'changed password in local table');

         return 1;
      }
      else {
         AUDIT(ACTION_LOGIN_FAILED, $user['username'], '', '', 'failed bind to ' . $query->row['remotehost'], $user['dn']);
      }

      return 0; 
   }


   private function checkLoginAgainstIMAP($username = '', $password = '') {
      $user = array();

      $imap = new Zend_Mail_Protocol_Imap(IMAP_HOST, IMAP_PORT, IMAP_SSL);
      if($imap->login($username, $password)) {
         $imap->logout();

         $query = $this->db->query("SELECT email, uid FROM " . TABLE_EMAIL . " WHERE email=?", array($username));
         if($query->num_rows == 0) {
            $a = explode("@", $username);

            $user['uid'] = $this->model_user_user->get_next_uid();
            $user['username'] = $username;
            $user['realname'] = $a[0];
            $user['password'] = generate_random_string(8);
            $user['domain'] = @$a[1];
            $user['isadmin'] = 0;
            $user['email'] = $username;

            $this->model_user_user->add_user($user);
         }

         return 1;
      }

      return 0;
   }


   public function check_ntlm_auth() {
      if(!isset($_SERVER['REMOTE_USER'])) { return 0; }

      $u = explode("\\", $_SERVER['REMOTE_USER']);

      if(!isset($u[1])) { return 0; }

      $query = $this->db->query("SELECT " . TABLE_USER . ".username, " . TABLE_USER . ".uid, " . TABLE_USER . ".realname, " . TABLE_USER . ".dn, " . TABLE_USER . ".isadmin, " . TABLE_USER . ".domain FROM " . TABLE_USER . " WHERE " . TABLE_USER . ".samaccountname=?", array($u[1]));

      if($query->num_rows == 1) {
         $_SESSION['username'] = $query->row['username'];
         $_SESSION['uid'] = $query->row['uid'];
         $_SESSION['admin_user'] = $query->row['isadmin'];
         $_SESSION['email'] = $this->model_user_user->get_primary_email_by_domain($query->row['uid'], $query->row['domain']);
         $_SESSION['domain'] = $query->row['domain'];
         $_SESSION['realname'] = $query->row['realname'];

         $_SESSION['emails'] = $this->model_user_user->get_users_all_email_addresses($query->row['uid']);
         $_SESSION['folders'] = $this->model_folder_folder->get_all_folder_ids($query->row['uid']);
         $_SESSION['extra_folders'] = $this->model_folder_folder->get_all_extra_folder_ids($query->row['uid']);

         return 1;
      }

      return 0; 
   }


   public function change_password($username = '', $password = '') {
      if($username == "" || $password == ""){ return 0; }

      $query = $this->db->query("UPDATE " . TABLE_USER . " SET password=? WHERE username=?", array(crypt($password), $username));

      $rc = $this->db->countAffected();

      return $rc;
   }

}

?>
