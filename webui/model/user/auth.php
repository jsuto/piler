<?php

class ModelUserAuth extends Model {

   public function apply_user_auth_session($data = array()) {
      $session = Registry::get('session');

      $session->set("username", $data['username']);
      $session->set("uid", $data['uid']);
      $session->set("admin_user", $data['admin_user']);
      $session->set("email", $data['username']);
      $session->set("domain", $data['domain']);
      $session->set("realname", $data['realname']);

      $session->set("auditdomains", $data['auditdomains']);
      $session->set("emails", $data['emails']);

      $session->set("folders", $data['folders']);
   }


   public function checkLogin($username = '', $password = '') {
      $session = Registry::get('session');
      $ok = 0;

      $data = array();

      $data['username'] = '';
      $data['uid'] = -1;
      $data['admin_user'] = 0;
      $data['email'] = '';
      $data['domain'] = '';
      $data['realname'] = '';
      $data['auditdomains'] = array();
      $data['emails'] = array();
      $data['folders'] = array();

      if($username == '' || $password == '') { return 0; }

      if(ENABLE_LDAP_AUTH == 1) {
         $ok = $this->checkLoginAgainstLDAP($username, $password, $data);
         if($ok == 1) {
            if(CUSTOM_EMAIL_QUERY_FUNCTION && function_exists(CUSTOM_EMAIL_QUERY_FUNCTION)) {
               call_user_func(CUSTOM_EMAIL_QUERY_FUNCTION, $username);
            }

            return $ok;
         }
      }

      if(ENABLE_IMAP_AUTH == 1) {
         require 'Zend/Mail/Protocol/Imap.php';
         $ok = $this->checkLoginAgainstIMAP($username, $password, $data);

         if($ok == 1) {
            if(CUSTOM_EMAIL_QUERY_FUNCTION && function_exists(CUSTOM_EMAIL_QUERY_FUNCTION)) {
               call_user_func(CUSTOM_EMAIL_QUERY_FUNCTION, $username);
            }

            return $ok;
         }
      }

      if(ENABLE_POP3_AUTH == 1) {
         require 'Zend/Mail/Protocol/Pop3.php';
         $ok = $this->checkLoginAgainstPOP3($username, $password, $data);

         if($ok == 1) {
            if(CUSTOM_EMAIL_QUERY_FUNCTION && function_exists(CUSTOM_EMAIL_QUERY_FUNCTION)) {
               call_user_func(CUSTOM_EMAIL_QUERY_FUNCTION, $username);
            }

            return $ok;
         }
      }

      // fallback local auth

      $query = $this->db->query("SELECT u.username, u.uid, u.realname, u.dn, u.password, u.isadmin, u.domain FROM " . TABLE_USER . " u, " . TABLE_EMAIL . " e WHERE e.email=? AND e.uid=u.uid", array($username));

      if(!isset($query->row['password'])) { return 0; }

      $pass = crypt($password, $query->row['password']);

      if($pass == $query->row['password']){
         $ok = 1;

         AUDIT(ACTION_LOGIN, $username, '', '', 'successful auth against user table');
      }
      else {
         AUDIT(ACTION_LOGIN_FAILED, $username, '', '', 'failed auth against user table');
      }


      if($ok == 1) {
         $data['username'] = $username;
         $data['uid'] = $query->row['uid'];
         $data['admin_user'] = $query->row['isadmin'];
         $data['email'] = $username;
         $data['domain'] = $query->row['domain'];
         $data['realname'] = $query->row['realname'];

         $data['auditdomains'] = $this->model_user_user->get_users_all_domains($query->row['uid']);

         if(CUSTOM_EMAIL_QUERY_FUNCTION && function_exists(CUSTOM_EMAIL_QUERY_FUNCTION)) {
            call_user_func(CUSTOM_EMAIL_QUERY_FUNCTION, $username);
         }
         else {
            $data['emails'] = $this->model_user_user->get_users_all_email_addresses($query->row['uid']);
         }

         $extra_emails = $this->model_user_user->get_email_addresses_from_groups($data['emails']);
         $data['emails'] = array_merge($data['emails'], $extra_emails);

         $data['folders'] = $this->model_folder_folder->get_folder_id_array_for_user($query->row['uid'], $data['admin_user']);

         $session->set("auth_data", $data);

         $this->is_ga_code_needed($username);

         $this->is_four_eye_auth_needed($data['admin_user']);

         return 1;
      }

      return 0;
   }


   private function checkLoginAgainstLDAP($username = '', $password = '', $data = array()) {
      $a = array();
      $ret = 0;

      if(ENABLE_SAAS == 1) {
         $params = $this->model_saas_ldap->get_ldap_params_by_email($username);
         foreach($params as $param) {
            $ret = $this->checkLoginAgainstLDAP_real($username, $password, $data, $param);

            syslog(LOG_INFO, "ldap auth result against " . $param['ldap_host'] . " / " . $param['ldap_type'] . ": $ret");

            if($ret == 1) { return $ret; }
         }
      }
      else {
         $ret = $this->checkLoginAgainstLDAP_real($username, $password, $data);
      }

      return $ret;
   }


   private function checkLoginAgainstLDAP_real($username = '', $password = '', $data = array(), $a = array()) {
      $session = Registry::get('session');

      $ldap_type = '';
      $ldap_host = LDAP_HOST;
      $ldap_base_dn = LDAP_BASE_DN;
      $ldap_helper_dn = LDAP_HELPER_DN;
      $ldap_helper_password = LDAP_HELPER_PASSWORD;
      $ldap_auditor_member_dn = LDAP_AUDITOR_MEMBER_DN;
      $ldap_admin_member_dn = LDAP_ADMIN_MEMBER_DN;

      $role = 0;
      $username_prefix = '';

      if(count($a) >= 6) {
         $ldap_type = $a['ldap_type'];
         $ldap_host = $a['ldap_host'];
         $ldap_base_dn = $a['ldap_base_dn'];
         $ldap_helper_dn = $a['ldap_bind_dn'];
         $ldap_helper_password = $a['ldap_bind_pw'];
         $ldap_auditor_member_dn = $a['ldap_auditor_member_dn'];

         $ldap_mail_attr = $a['ldap_mail_attr'];
         $ldap_account_objectclass = $a['ldap_account_objectclass'];
         $ldap_distributionlist_attr = $a['ldap_distributionlist_attr'];
         $ldap_distributionlist_objectclass = $a['ldap_distributionlist_objectclass'];
      }

      if($ldap_type != LDAP_TYPE_GENERIC) {
         list($ldap_mail_attr, $ldap_account_objectclass, $ldap_distributionlist_attr, $ldap_distributionlist_objectclass) = get_ldap_attribute_names($ldap_type);
      }

      if($ldap_mail_attr == 'proxyAddresses') { $username_prefix = 'smtp:'; }

      if($ldap_host == '' || $ldap_helper_password == '') { return 0; }

      $ldap = new LDAP($ldap_host, $ldap_helper_dn, $ldap_helper_password);

      if($ldap->is_bind_ok()) {

         $query = $ldap->query($ldap_base_dn, "(&(objectClass=$ldap_account_objectclass)($ldap_mail_attr=$username_prefix$username))", array());

         if(isset($query->row['dn']) && $query->row['dn']) {
            $a = $query->row;

            $ldap_auth = new LDAP($ldap_host, $a['dn'], $password);

            if(ENABLE_SYSLOG == 1) { syslog(LOG_INFO, "ldap auth against '" . $ldap_host . "', dn: '" . $a['dn'] . "', result: " . $ldap_auth->is_bind_ok()); }

            if($ldap_auth->is_bind_ok()) {

               $a['dn'] = stripslashes($a['dn']);
               $a['dn'] = preg_replace("/\(/", '\(', $a['dn']);
               $a['dn'] = preg_replace("/\)/", '\)', $a['dn']);

               $query = $ldap->query($ldap_base_dn, "(|(&(objectClass=$ldap_account_objectclass)($ldap_mail_attr=$username_prefix$username))(&(objectClass=$ldap_distributionlist_objectclass)($ldap_distributionlist_attr=$username_prefix$username)" . ")(&(objectClass=$ldap_distributionlist_objectclass)($ldap_distributionlist_attr=" . $a['dn'] . ")))", array());

               if($this->check_ldap_membership($ldap_auditor_member_dn, $query->rows) == 1) { $role = 2; }
               if($this->check_ldap_membership($ldap_admin_member_dn, $query->rows) == 1) { $role = 1; }

               $emails = $this->get_email_array_from_ldap_attr($query->rows);

               $extra_emails = $this->model_user_user->get_email_addresses_from_groups($emails);
               $emails = array_merge($emails, $extra_emails);

               $data = $this->fix_user_data($a['cn'], $username, $emails, $role);

               $session->set("auth_data", $data);

               $this->is_ga_code_needed($username);

               $this->is_four_eye_auth_needed($role);

               AUDIT(ACTION_LOGIN, $username, '', '', 'successful auth against LDAP');

               return 1;
            }
            else {
               AUDIT(ACTION_LOGIN_FAILED, $username, '', '', 'failed auth against LDAP');
            }
         }
      }
      else if(ENABLE_SYSLOG == 1) {
         syslog(LOG_INFO, "cannot bind to '" . $ldap_host . "' as '" . $ldap_helper_dn . "'");
      }

      return 0;
   }


   private function check_ldap_membership($ldap_auditor_member_dn = '', $e = array()) {
      if($ldap_auditor_member_dn == '') { return 0; }

      foreach($e as $a) {
         foreach (array("memberof", "dn") as $memberattr) {
            if(isset($a[$memberattr])) {

               if(isset($a[$memberattr]['count']) && $a[$memberattr]['count'] > 0) {

                  for($i = 0; $i < $a[$memberattr]['count']; $i++) {
                     if($a[$memberattr][$i] == $ldap_auditor_member_dn) {
                        return 1;
                     }
                  }
               }
               else {
                  if($a[$memberattr] == $ldap_auditor_member_dn) {
                     return 1;
                  }
               }
            }
         }
      }

      return 0;
   }


   public function get_email_array_from_ldap_attr($e = array()) {
      $data = array();

      foreach($e as $a) {
         //syslog(LOG_INFO, "checking ldap entry dn: " . $a['dn'] . ", cn: " . $a['cn']);

         foreach (array("mail", "mailalternateaddress", "proxyaddresses", "zimbraMailForwardingAddress", "member", "memberOfGroup") as $mailattr) {
            if(isset($a[$mailattr])) {

               if(is_array($a[$mailattr])) {
                  for($i = 0; $i < $a[$mailattr]['count']; $i++) {

                     //syslog(LOG_INFO, "checking entry: " . $a[$mailattr][$i]);

                     $a[$mailattr][$i] = strtolower($a[$mailattr][$i]);

                     if(strchr($a[$mailattr][$i], '@')) {

                        if(preg_match("/^([\w]+)\:/i", $a[$mailattr][$i], $p)) {
                           if(isset($p[0]) && $p[0] != "smtp:") { continue; }
                        }

                        $email = preg_replace("/^([\w]+)\:/i", "", $a[$mailattr][$i]);
                        if(validemail($email) && !in_array($email, $data)) { array_push($data, $email); }
                     }
                  }
               }
               else {
                  //syslog(LOG_INFO, "checking entry #2: " . $a[$mailattr]);

                  $email = strtolower(preg_replace("/^([\w]+)\:/i", "", $a[$mailattr]));
                  if(validemail($email) && !in_array($email, $data)) { array_push($data, $email); }
               }
            }
         }
      }

      return $data;
   }


   private function fix_user_data($name = '', $email = '', $emails = array(), $role = 0) {
      $data = array();

      $data['username'] = $email;
      $data['uid'] = -1;
      $data['admin_user'] = $role;
      $data['email'] = $email;
      $data['domain'] = '';
      $data['realname'] = $name;
      $data['auditdomains'] = $this->model_domain_domain->get_your_all_domains_by_email($email);
      $data['emails'] = $emails;
      $data['folders'] = array();

      $uid = $this->model_user_user->get_uid_by_email($email);
      if($uid < 1) {
         $uid = $this->model_user_user->get_next_uid(TABLE_EMAIL);
         $query = $this->db->query("INSERT INTO " . TABLE_EMAIL . " (uid, email) VALUES(?,?)", array($uid, $email));
      }

      $data['uid'] = $uid;

      $a = explode("@", $email);
      $data['domain'] = $a[1];

      return $data;
   }


   private function checkLoginAgainstIMAP($username = '', $password = '', $data = array()) {
      $session = Registry::get('session');
      $emails = array($username);

      if(!strchr($username, '@')) { return 0; }

      $login = $username;

      if(STRIP_DOMAIN_NAME_FROM_USERNAME == 1) {
         $a = explode("@", $username);
         $login = $a[0];
      }

      $imap = new Zend_Mail_Protocol_Imap(IMAP_HOST, IMAP_PORT, IMAP_SSL);
      if($imap->login($login, $password)) {
         $imap->logout();

         $extra_emails = $this->model_user_user->get_email_addresses_from_groups($emails);
         $emails = array_merge($emails, $extra_emails);

         $data['username'] = $username;
         $data['email'] = $username;
         $data['emails'] = $emails;
         $data['role'] = 0;

         $data = $this->fix_user_data($username, $username, $emails, 0);

         $this->is_ga_code_needed($username);

         $session->set("auth_data", $data);

         $session->set("password", $password);

         return 1;
      }

      return 0;
   }


   private function checkLoginAgainstPOP3($username = '', $password = '', $data = array()) {
      $rc = 0;
      $emails = array($username);

      try {
         $conn = new Zend_Mail_Protocol_Pop3(POP3_HOST, POP3_PORT, POP3_SSL);

         if($conn) {
            $s = $conn->connect(POP3_HOST);

            if($s) {

               try {
                  $conn->login($username, $password);

                  $extra_emails = $this->model_user_user->get_email_addresses_from_groups($emails);
                  $emails = array_merge($emails, $extra_emails);

                  $data = $this->fix_user_data($username, $username, $emails, 0);

                  $this->is_ga_code_needed($username);

                  $session = Registry::get('session');
                  $session->set("auth_data", $data);

                  $rc = 1;
               }
               catch (Zend_Mail_Protocol_Exception $e) {}
            }
         }
      }
      catch (Zend_Mail_Protocol_Exception $e) {}

      return $rc;
   }


   public function check_ntlm_auth() {
      $ldap_auditor_member_dn = LDAP_AUDITOR_MEMBER_DN;
      $ldap_admin_member_dn = LDAP_ADMIN_MEMBER_DN;

      $role = 0;

      if(!isset($_SERVER['REMOTE_USER']) || $_SERVER['REMOTE_USER'] == '') { return 0; }

      $u = explode("\\", $_SERVER['REMOTE_USER']);

      if(isset($u[1])) { $username = $u[1]; }
      else { $username = $_SERVER['REMOTE_USER']; }

      if(ENABLE_SYSLOG == 1) { syslog(LOG_INFO, "sso login: $username"); }

      $ldap = new LDAP(LDAP_HOST, LDAP_HELPER_DN, LDAP_HELPER_PASSWORD);

      if($ldap->is_bind_ok()) {

         $query = $ldap->query(LDAP_BASE_DN, "(&(objectClass=user)(samaccountname=" . $username . "))", array());

         if(isset($query->row['dn'])) {
            $a = $query->row;

            if(is_array($a['mail'])) { $username = $a['mail'][0]; } else { $username = $a['mail']; }
            $username = strtolower(preg_replace("/^smtp\:/i", "", $username));

            if($username == '') {
               syslog(LOG_INFO, "no email address found for " . $a['dn']);
               return 0;
            }

            $ldap_mail_attr = LDAP_MAIL_ATTR;
            if(LDAP_MAIL_ATTR == 'proxyAddresses') { $ldap_mail_attr = 'proxyAddresses=smtp:'; }

            $query = $ldap->query(LDAP_BASE_DN, "(|(&(objectClass=user)(" . $ldap_mail_attr . "$username))(&(objectClass=group)(member=$username))(&(objectClass=group)(member=" . stripslashes($a['dn']) . ")))", array());


            $emails = $this->get_email_array_from_ldap_attr($query->rows);

            $extra_emails = $this->model_user_user->get_email_addresses_from_groups($emails);
            $emails = array_merge($emails, $extra_emails);

            if(!in_array($username, $emails)) { array_push($emails, $username); }

            if($this->check_ldap_membership($ldap_auditor_member_dn, $query->rows) == 1) { $role = 2; }
            if($this->check_ldap_membership($ldap_admin_member_dn, $query->rows) == 1) { $role = 1; }

            $data = $this->fix_user_data($a['cn'], $username, $emails, $role);
            $this->apply_user_auth_session($data);

            $this->model_user_prefs->get_user_preferences($username);

            AUDIT(ACTION_LOGIN, $username, '', '', 'successful auth against LDAP');

            return 1;
         }

      }

      return 0; 
   }


   public function get_failed_login_count() {
      $session = Registry::get('session');

      $n = $session->get('failed_logins');
      if($n == '') { $n = 0; }

      return $n;
   }


   public function increment_failed_login_count($n = 0) {
      $session = Registry::get('session');

      $n = $session->get('failed_logins') + 1;
      $session->set('failed_logins', $n);
   }


   private function is_ga_code_needed($username = '') {
      $session = Registry::get('session');

      $query = $this->db->query("SELECT ga_enabled FROM " . TABLE_USER_SETTINGS . " WHERE username=?", array($username));

      if(isset($query->row['ga_enabled']) && $query->row['ga_enabled'] == 1) {
         $session->set("ga_block", 1);
      }
   }


   public function is_four_eye_auth_needed($admin_user = 0) {
      $session = Registry::get('session');

      if(1 == FOUR_EYES_LOGIN_FOR_AUDITOR && 2 == $admin_user) {
         $session->set("four_eyes", 1);
      }

   }


   public function change_password($username = '', $password = '') {
      if($username == "" || $password == ""){ return 0; }

      $query = $this->db->query("UPDATE " . TABLE_USER . " SET password=? WHERE username=?", array(crypt($password), $username));

      $rc = $this->db->countAffected();

      return $rc;
   }

}

?>
