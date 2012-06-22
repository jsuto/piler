<?php


class ModelUserImport extends Model {


   public function query_remote_users($host) {
      $data = array();

      LOGGER("running query_remote_users() ...");

      $attrs = array("cn", "mail", "mailAlternateAddress", "memberdn", "memberaddr");
      $mailAttr = 'mail';
      $mailAttrs = array("mail", "mailalternateaddress");

      $memberAttrs = array("memberdn");

      $ldap = new LDAP($host['ldap_host'], $host['ldap_binddn'], $host['ldap_bindpw']);
      if($ldap->is_bind_ok() == 0) {
         LOGGER($host['ldap_binddn'] . ": failed bind to " . $host['ldap_host']);
         return array();
      }

      LOGGER($host['ldap_binddn'] . ": successful bind to " . $host['ldap_host']);
      LOGGER("LDAP type: " . $host['type']);

      if($host['type'] == "AD") {
         $attrs = array("cn", "proxyaddresses", "member");

         $mailAttr = "proxyaddresses";
         $mailAttrs = array("proxyaddresses");

         $memberAttrs = array("member");
      }


      $query = $ldap->query($host['ldap_basedn'], "$mailAttr=*", $attrs );
      LOGGER("LDAP query: $mailAttr=* for basedn:" . $host['ldap_basedn']);

      foreach ($query->rows as $result) {
         $emails = "";

         if(!isset($result['cn']) || !isset($result['dn']) ) { continue; }

         foreach($mailAttrs as $__mail_attr) {

            if(isset($result[$__mail_attr]) ) {

               if(is_array($result[$__mail_attr]) ) {

                  for($i = 0; $i < $result[$__mail_attr]['count']; $i++) {
                     LOGGER("found email entry: " . $result['dn'] . " => $__mail_attr:" . $result[$__mail_attr][$i]);
                     if(preg_match("/^smtp\:/i", $result[$__mail_attr][$i])) {
                        $emails .= strtolower(preg_replace("/^smtp\:/i", "", $result[$__mail_attr][$i])) . "\n";
                     }
                  }
               }
               else {
                  LOGGER("found email entry: " . $result['dn'] . " => $__mail_attr:" . $result[$__mail_attr]);
                  $emails .= strtolower(preg_replace("/smtp\:/i", "", $result[$__mail_attr])) . "\n";
               }

            }

         }

         $__emails = explode("\n", $emails);


         $members = "";

         foreach($memberAttrs as $__member_attr) {

            if(isset($result[$__member_attr]) ) {
               if(is_array($result[$__member_attr]) ) {
                  for($i = 0; $i < $result[$__member_attr]['count']; $i++) {
                     LOGGER("found member entry: " . $result['dn'] . " => $__member_attr:" . $result[$__member_attr][$i]);
                     $members .= $result[$__member_attr][$i] . "\n";
                  }
               }
               else {
                  LOGGER("found member entry: " . $result['dn'] . " => $__member_attr:" . $result[$__member_attr]);
                  $members .= $result[$__member_attr] . "\n";
               }
            }

         }

         $data[] = array(
                         'username'     => preg_replace("/\n{1,}$/", "", $__emails[0]),
                         'realname'     => $result['cn'],
                         'dn'           => $result['dn'],
                         'emails'       => preg_replace("/\n{1,}$/", "", $emails),
                         'members'      => preg_replace("/\n{1,}$/", "", $members)
                        );

      }

      LOGGER("found " . count($data) . " users");

      return $data;
   }



   public function fill_remote_table($host = array(), $domain = '') {
      if($domain == '') { return 0; }

      /*
       * if the 't_remote' table has no entry for your domain and we read some users
       * let's put the connection info to the 't_remote' table needed for proxying
       * the authentication requests
       */

      $query = $this->db->query("SELECT COUNT(*) AS num FROM " . TABLE_REMOTE . " WHERE remotedomain=?", array($domain));

      if(isset($query->row['num'])) {

         if($query->row['num'] == 0) {
            $query = $this->db->query("INSERT INTO " . TABLE_REMOTE . " (remotedomain, remotehost, basedn, binddn) VALUES(?,?,?,?)", array($domain, $host['ldap_host'], $host['ldap_basedn'], $host['ldap_binddn']));
         }
         else {
            $query = $this->db->query("UPDATE " . TABLE_REMOTE . " SET remotehost=?, basedn=?, binddn=? WHERE remotedomain=?", array($host['ldap_host'], $host['ldap_basedn'], $host['ldap_binddn'], $domain));

         }

         LOGGER("SQL exec:" . $query->query);

      }

      return 1;
   }



   public function process_users($users = array(), $globals = array()) {
      $late_add = array();
      $uids = array();
      $exclude = array();
      $newuser = 0;
      $deleteduser = 0;
      $n = 0;

      LOGGER("running process_users() ...");

      /* build a list of DNs to exclude from the import */

      while (list($k, $v) = each($globals)) {
         if(preg_match("/^reject_/", $k)) {
            $exclude[$v] = $v;
         }
      }


      foreach ($users as $_user) {
         if(strlen($_user['dn']) > DN_MAX_LEN) { LOGGER("ERR: too long entry: " . $_user['dn']); }

         if(in_array($_user['dn'], $exclude) ) {
            LOGGER("excluding from import:" . $_user['dn']);
            continue;
         }

         /* Does this DN exist in the user table ? */

         $__user = $this->model_user_user->get_user_by_dn($_user['dn']);

         if(isset($__user['uid'])) {

            array_push($uids, $__user['uid']);


            /* if so, then verify the email addresses */

            $changed = 0;
            $emails = $this->model_user_user->get_emails_by_uid($__user['uid']);

            /* first let's add the new email addresses */

            $ldap_emails = explode("\n", $_user['emails']);
            $sql_emails = explode("\n", $emails);

            foreach ($ldap_emails as $email) {
               if(!in_array($email, $sql_emails)) {
                  $rc = $this->model_user_user->add_email($__user['uid'], $email);
                  $changed++;

                  /* in case of an error add it to the $late_add array() */

                  if($rc == 0) {
                     $late_add[] = array(
                                          'uid'   => $__user['uid'],
                                          'email' => $email
                                        );
                  }
               }
            }


            /* delete emails not present in the user's LDAP entry */

            foreach ($sql_emails as $email) {
               if(!in_array($email, $ldap_emails)) {
                  $rc = $this->model_user_user->remove_email($__user['uid'], $email);
                  $changed++;
               }
            }

            LOGGER($_user['dn'] . ": exists, changed=$changed");

            if($changed > 0) { $n++; }
         }
         else {

            /* or add the new user */

            $user = $this->createNewUserArray($_user['dn'], $_user['username'], $_user['realname'], $_user['emails'], $globals);
            array_push($uids, $user['uid']);

            $rc = $this->model_user_user->add_user($user);
            if($rc == 1) { $newuser++; }
         }
      }


      /* add the rest to the email table */

      foreach ($late_add as $new) {
         $rc = $this->model_user_user->add_email($new['uid'], $new['email']);
         if($rc == 1) { $newuser++; }
      }


      /* delete accounts not present in the LDAP directory */

      if(count($uids) > 0) {
         $uidlist = implode("','", $uids);
         $query = $this->db->query("SELECT uid, username FROM " . TABLE_USER . " WHERE domain=? AND dn != '*' AND dn LIKE '%" . $globals['ldap_basedn'] . "' AND dn is NOT NULL AND uid NOT IN ('$uidlist')", array($globals['domain']) );

         foreach ($query->rows as $deleted) {
            $deleteduser++;
            $this->model_user_user->deleteUser($deleted['uid']);
         }
      }


      /* try to add new membership entries */

      reset($users);

      foreach ($users as $user) {
         if($user['members']) {

            $group = $this->model_user_user->get_user_by_dn($user['dn']);

            $members = explode("\n", $user['members']);
            if(count($members) > 0) {

               if(isset($group['uid'])) {
                  $query = $this->db->query("DELETE FROM " . TABLE_EMAIL_LIST . " WHERE gid=?", array($group['uid']) );
               }

               foreach ($members as $member) {
                  if(validemail($member)) {
                     $__user = $this->model_user_user->get_user_by_email($member);
                  } else {
                     $__user = $this->model_user_user->get_user_by_dn($member);
                  }

                  if(isset($group['uid']) && isset($__user['uid'])) {
                     $query = $this->db->query("INSERT INTO " . TABLE_EMAIL_LIST . " (uid, gid) VALUES(?,?)", array((int)$__user['uid'], $group['uid']));
                  }

               }
            }
            
         }
      }

      return array($newuser, $deleteduser);
   }


   private function createNewUserArray($dn = '', $username = '', $realname = '', $emails = '', $globals = array()) {
      $user = array();

      $user['uid'] = $this->model_user_user->get_next_uid();
      $user['gid'] = $globals['gid'];

      $user['email'] = $emails;

      if(USE_EMAIL_AS_USERNAME == 1) {
         $email = explode("\n", $emails);
         $user['username'] = $email[0];
      }
      else {
         $user['username'] = $username . $user['uid'];
      }


      $user['password'] = '*';

      $user['realname'] = $realname;

      if($realname == '') { $user['realname'] = $username; }

      $user['domain'] = $globals['domain'];
      $user['dn'] = $dn;
      $user['policy_group'] = $globals['policy_group'];
      $user['isadmin'] = 0;
      $user['whitelist'] = '';
      $user['blacklist'] = '';

      return $user;
   }


   public function trash_password($users = array()) {
      foreach ($users as $user) {
         $query = $this->db->query("UPDATE " . TABLE_USER . " SET password='*' WHERE dn=?", array($user['dn']));
         $rc = $this->db->countAffected();
         LOGGER("setting default password for " . $user['dn'] . " (rc=$rc)");
      }
   }


   public function count_email_addresses() {
      $query = $this->db->query("SELECT COUNT(*) AS num FROM " . TABLE_EMAIL);

      if(isset($query->row['num'])) { return $query->row['num']; }

      return 0;
   }

}

?>
