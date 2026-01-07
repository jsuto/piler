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
      $ok = 0;

      $imap_server = array();

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

      if($username == '' || $password == '') { return $ok; }


      if(CUSTOM_PRE_AUTH_FUNCTION && function_exists(CUSTOM_PRE_AUTH_FUNCTION)) {
         call_user_func(CUSTOM_PRE_AUTH_FUNCTION, $username);
      }

      // Check the fallback login first to prevent sending local account
      // data (admin@local or auditor@local passwords) to remote imap, etc. servers.

      $ok = $this->checkFallbackLogin($username, $password, $data);
      if($ok == 1) { return $ok; }


      if(ENABLE_LDAP_AUTH == 1) {
         $ok = $this->checkLoginAgainstLDAP($username, $password, $data);
         if($ok == 1) {
            return $ok;
         }
      }

      if(ENABLE_IMAP_AUTH == 1) {
         require 'Zend/Mail/Protocol/Imap.php';

         if(!isset($imap_server['IMAP_HOST'])) { $imap_server['IMAP_HOST'] = IMAP_HOST; }
         if(!isset($imap_server['IMAP_PORT'])) { $imap_server['IMAP_PORT'] = IMAP_PORT; }
         if(!isset($imap_server['IMAP_SSL'])) { $imap_server['IMAP_SSL'] = IMAP_SSL; }

         $ok = $this->checkLoginAgainstIMAP($imap_server, $username, $password, $data);

         if($ok == 1) {
            return $ok;
         }
      }

      if(ENABLE_POP3_AUTH == 1) {
         require 'Zend/Mail/Protocol/Pop3.php';
         $ok = $this->checkLoginAgainstPOP3($username, $password, $data);

         if($ok == 1) {
            return $ok;
         }
      }

      return $ok;
   }


   // fallback local auth

   private function checkFallbackLogin($username = '', $password = '', $data = array()) {
      $ok = 0;
      $session = Registry::get('session');

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
            $data['emails'] = call_user_func(CUSTOM_EMAIL_QUERY_FUNCTION, $username);
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

            if(LOG_LEVEL >= NORMAL) { syslog(LOG_INFO, "ldap auth result against " . $param['ldap_host'] . " / " . $param['ldap_type'] . ": $ret"); }

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

            if(LOG_LEVEL >= NORMAL) { syslog(LOG_INFO, "ldap auth against '" . $ldap_host . "', dn: '" . $a['dn'] . "', result: " . $ldap_auth->is_bind_ok()); }

            if($ldap_auth->is_bind_ok()) {

               $a['dn'] = $this->escapeLdapFilter($a['dn']);

               $query = $ldap->query($ldap_base_dn, "(|(&(objectClass=$ldap_account_objectclass)($ldap_mail_attr=$username_prefix$username))(&(objectClass=$ldap_distributionlist_objectclass)($ldap_distributionlist_attr=$username_prefix$username)" . ")(&(objectClass=$ldap_distributionlist_objectclass)($ldap_distributionlist_attr=" . $a['dn'] . ")))", array());

               if($this->check_ldap_membership($ldap_auditor_member_dn, $query->rows) == 1) { $role = 2; }
               if($this->check_ldap_membership($ldap_admin_member_dn, $query->rows) == 1) { $role = 1; }

               $emails = $this->get_email_array_from_ldap_attr($query->rows, $ldap_distributionlist_objectclass);

               if(CUSTOM_EMAIL_QUERY_FUNCTION && function_exists(CUSTOM_EMAIL_QUERY_FUNCTION)) {
                  $emails = call_user_func(CUSTOM_EMAIL_QUERY_FUNCTION, $emails);
               }

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
      else if(LOG_LEVEL >= NORMAL) {
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


   public function get_email_array_from_ldap_attr($e = array(), $group_object_class) {
      global $mailattrs;
      $data = [];
      $group_emails = [];
      $user_emails = [];

      foreach($e as $a) {
         $group_object = 0;

         if($group_object_class && in_array($group_object_class, $a['objectclass'])) {
            $group_object = 1;
         }

         if(LOG_LEVEL >= DEBUG) { syslog(LOG_INFO, "checking ldap entry dn: " . $a['dn'] . ", cn: " . $a['cn']); }

         foreach ($mailattrs as $mailattr) {
            if(isset($a[$mailattr])) {

               if(is_array($a[$mailattr])) {
                  for($i = 0; $i < $a[$mailattr]['count']; $i++) {

                     if(LOG_LEVEL >= DEBUG) { syslog(LOG_INFO, "checking entry: " . $a[$mailattr][$i]); }

                     $a[$mailattr][$i] = strtolower($a[$mailattr][$i]);

                     if(strchr($a[$mailattr][$i], '@')) {

                        if(preg_match("/^([\w]+)\:/i", $a[$mailattr][$i], $p)) {
                           if(isset($p[0]) && $p[0] != "smtp:") { continue; }
                        }

                        $email = preg_replace("/^([\w]+)\:/i", "", $a[$mailattr][$i]);
                        if(validemail($email)) {
                           if(!in_array($email, $data)) { array_push($data, $email); }

                           if($group_object) {
                              if(!in_array($email, $group_emails)) { array_push($group_emails, $email); }
                           } else {
                              if(!in_array($email, $user_emails)) { array_push($user_emails, $email); }
                           }
                        }
                     }
                  }
               }
               else {
                  if(LOG_LEVEL >= DEBUG) { syslog(LOG_INFO, "checking entry #2: " . $a[$mailattr]); }

                  $email = strtolower(preg_replace("/^([\w]+)\:/i", "", $a[$mailattr]));
                  if(validemail($email)) {
                     if(!in_array($email, $data)) { array_push($data, $email); }

                     if($group_object) {
                        if(!in_array($email, $group_emails)) { array_push($group_emails, $email); }
                     } else {
                        if(!in_array($email, $user_emails)) { array_push($user_emails, $email); }
                     }
                  }
               }
            }
         }
      }

      $session = Registry::get('session');

      $session->set("user_emails", $user_emails);
      $session->set("group_emails", $group_emails);

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
         $uid = $this->model_user_user->get_next_uid();
         $query = $this->db->query("INSERT INTO " . TABLE_EMAIL . " (uid, email) VALUES(?,?)", array($uid, $email));
      }

      $data['uid'] = $uid;

      $a = explode("@", $email);
      $data['domain'] = $a[1];

      return $data;
   }


   private function checkLoginAgainstIMAP($imap_server = array(), $username = '', $password = '', $data = array()) {
      $rc = 0;
      $session = Registry::get('session');
      $emails = array($username);

      /*
       * usernames without the domain part are allowed, though
       * they won't see any emails unless a post auth hook is run
       * to assign some email addresses to them
       */

      $login = $username;

      if(STRIP_DOMAIN_NAME_FROM_USERNAME == 1) {
         $a = explode("@", $username);
         $login = $a[0];
      }

      try {
         $imap = new Zend_Mail_Protocol_Imap($imap_server['IMAP_HOST'], $imap_server['IMAP_PORT'], $imap_server['IMAP_SSL']);
         if($imap->login($login, $password)) {
            $imap->logout();

            if(MAILCOW_API_KEY) {
               $userinfo = $this->get_mailcow_userinfo($username);
               $emails = $userinfo['emails'];
               $realname = $userinfo['realname'];
            }

            if(CUSTOM_EMAIL_QUERY_FUNCTION && function_exists(CUSTOM_EMAIL_QUERY_FUNCTION)) {
               $emails = call_user_func(CUSTOM_EMAIL_QUERY_FUNCTION, $username);
            }

            $extra_emails = $this->model_user_user->get_email_addresses_from_groups($emails);
            $emails = array_merge($emails, $extra_emails);

            $data = $this->fix_user_data($username, $emails[0], $emails, 0);

            $data['folders'] = $this->model_folder_folder->get_folder_id_array_for_user($data['uid'], 0);

            $this->is_ga_code_needed($username);

            $session->set("auth_data", $data);

            $session->set("password", $password);

            $rc = 1;
         }
      }
      catch (Zend_Mail_Protocol_Exception $e) {}

      return $rc;
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

                  if(CUSTOM_EMAIL_QUERY_FUNCTION && function_exists(CUSTOM_EMAIL_QUERY_FUNCTION)) {
                     $emails = call_user_func(CUSTOM_EMAIL_QUERY_FUNCTION, $username);
                  }

                  $extra_emails = $this->model_user_user->get_email_addresses_from_groups($emails);
                  $emails = array_merge($emails, $extra_emails);

                  $data = $this->fix_user_data($username, $emails[0], $emails, 0);

                  $data['folders'] = $this->model_folder_folder->get_folder_id_array_for_user($data['uid'], 0);

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


   public function get_sso_user() {
      if(!isset($_SERVER['REMOTE_USER']) || $_SERVER['REMOTE_USER'] == '') { return ''; }

      // check if REMOTE_USER format is DOMAIN\user
      $u = explode("\\", $_SERVER['REMOTE_USER']);
      if(isset($u[1])) { return $u[1]; }

      // or REMOTE_USER might be in the form of user@domain
      $u = explode("@", $_SERVER['REMOTE_USER']);
      if(isset($u[0])) { return $u[0]; }

      return $_SERVER['REMOTE_USER'];
   }


   public function check_ntlm_auth() {
      $ldap_auditor_member_dn = LDAP_AUDITOR_MEMBER_DN;
      $ldap_admin_member_dn = LDAP_ADMIN_MEMBER_DN;

      $role = 0;

      $sso_user = $this->get_sso_user();
      if($sso_user == '') { return 0; }


      if(LOG_LEVEL >= NORMAL) { syslog(LOG_INFO, "sso login: $sso_user"); }

      $ldap = new LDAP(LDAP_HOST, LDAP_HELPER_DN, LDAP_HELPER_PASSWORD);

      if($ldap->is_bind_ok()) {

         $query = $ldap->query(LDAP_BASE_DN, "(&(objectClass=user)(samaccountname=" . $sso_user . "))", array());

         if(isset($query->row['dn'])) {
            $a = $query->row;

            if(is_array($a['mail'])) { $username = $a['mail'][0]; } else { $username = $a['mail']; }
            $username = strtolower(preg_replace("/^smtp\:/i", "", $username));

            if($username == '') {
               if(LOG_LEVEL >= NORMAL) { syslog(LOG_INFO, "no email address found for " . $a['dn']); }
               return 0;
            }

            $ldap_mail_attr = LDAP_MAIL_ATTR;

            if(LDAP_MAIL_ATTR == 'proxyAddresses') {
               $ldap_mail_attr = 'proxyAddresses=smtp:';
            }
            else {
               $ldap_mail_attr .= '=';
            }

            $query = $ldap->query(LDAP_BASE_DN, "(|(&(objectClass=user)(" . $ldap_mail_attr . "$username))(&(objectClass=group)(member=$username))(&(objectClass=group)(member=" . stripslashes($a['dn']) . ")))", array());

            $emails = $this->get_email_array_from_ldap_attr($query->rows, LDAP_DISTRIBUTIONLIST_OBJECTCLASS);

            if(CUSTOM_EMAIL_QUERY_FUNCTION && function_exists(CUSTOM_EMAIL_QUERY_FUNCTION)) {
               $emails = call_user_func(CUSTOM_EMAIL_QUERY_FUNCTION, $emails);
            }

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
      else {
         syslog(LOG_INFO, LDAP_HELPER_DN . " cannot bind to " . LDAP_HOST);
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

      $n = (int)$session->get('failed_logins') + 1;
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

      $query = $this->db->query("UPDATE " . TABLE_USER . " SET password=? WHERE uid=(SELECT uid FROM " . TABLE_EMAIL . " WHERE email=?)",
                                [encrypt_password($password), $username]);

      $rc = $this->db->countAffected();

      return $rc;
   }


   /*
    * For more explanation, see https://bitbucket.org/jsuto/piler/issues/679/get-mailing-list-members-from-active
    * Credits: Thoth
    */

   public function escapeLdapFilter($str = '') {
      // The characters that need to be escape.
      //
      // NOTE: It's important that the slash is the first character replaced.
      // Otherwise the slash added by other replacements will then be
      // replaced as well, resulted in double-escaping all characters
      // replaced before the slashes were replaced.
      //
      $metaChars = array(
                          chr(0x5c), // \
                          chr(0x2a), // *
                          chr(0x28), // (
                          chr(0x29), // )
                          chr(0x00) // NUL
                       );

        // Build the list of the escaped versions of those characters.

        $quotedMetaChars = array();

        foreach ($metaChars as $key => $value) {
           $quotedMetaChars[$key] = '\\' .
           str_pad(dechex(ord($value)), 2, '0', STR_PAD_LEFT);
        }

        // Make all the necessary replacements in the input string and return
        // the result.

        return str_replace($metaChars, $quotedMetaChars, $str);
    }


   private function mailcow_query($path = '') {
      if($path === '') { return []; }

      $ch = curl_init();

      $uri = MAILCOW_HOST . $path;

      $headers = [
         'Accept: application/json',
         'X-API-Key: ' . MAILCOW_API_KEY
      ];

      curl_setopt($ch, CURLOPT_URL, $uri);
      curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
      curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);

      if(ENABLE_SSL_VERIFY == 0) {
         curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
         curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, false);
      }

      $ret = curl_exec($ch);

      curl_close($ch);

      $ret = json_decode($ret, true);

      return $ret;
   }


   public function get_mailcow_userinfo($username = '') {
      $emails = [$username];

      // Get all aliases
      $aliases = $this->mailcow_query('/api/v1/get/alias/all');

      // TODO: Cache the results?

      foreach($aliases as $alias) {
         if(isset($alias['active']) && $alias['active'] !== 1) {
            continue;
         }

         if(isset($alias['active_int']) && $alias['active_int'] !== 1) {
            continue;
         }

         array_push($emails, strtolower($alias['address']));

         //syslog(LOG_INFO, 'mailcow alias: ' . $alias['address']);
      }

      // Get user's real name

      $user = $this->mailcow_query('/api/v1/get/mailbox/' . $username);

      return [
         'realname' => $user['name'],
         'emails'   => $emails
      ];
   }
}
