<?php

class ModelUserUser extends Model {


   public function check_uid($uid) {
      if($uid == "") { return 0; }

      if(!is_numeric($uid)) { return 0; }

      if($uid < 1) { return 0; }

      return 1;
   }


   public function get_uid_by_name($username = '') {
      if($username == ""){ return -1; }

      $query = $this->db->query("SELECT uid FROM " . TABLE_USER . " WHERE username=?", array($username));

      if(isset($query->row['uid'])){
         return $query->row['uid'];
      }

      return -1;
   }


   public function get_uid_by_email($email = '') {
      $query = $this->db->query("SELECT uid FROM " . TABLE_EMAIL . " WHERE email=?", array($email));

      if(isset($query->row['uid'])){ return $query->row['uid']; }

      return -1;
   }


   public function get_username_by_email($email = '') {
      $query = $this->db->query("SELECT username FROM " . TABLE_USER . ", " . TABLE_EMAIL . " WHERE " . TABLE_USER . ".uid=" . TABLE_EMAIL . ".uid AND email=?", array($email));

      if(isset($query->row['username'])){ return $query->row['username']; }

      return "";
   }


   public function get_users_all_email_addresses($uid = 0) {
      $data = array();
      $uids = $uid;

      if($uid >= 0) {
         $query = $this->db->query("SELECT gid FROM " . TABLE_EMAIL_LIST . " WHERE uid=?", array((int)$uid));

         if(isset($query->rows)) {
            foreach ($query->rows as $q) {
               if(is_numeric($q['gid']) && $q['gid'] > 0) {
                  $uids .= "," . $q['gid'];
               }
            }
         }

         $query = $this->db->query("SELECT email FROM " . TABLE_EMAIL . " WHERE uid IN ($uids)");

         foreach ($query->rows as $q) {
            array_push($data, $q['email']);
         }

      }

      return $data;
   }

   public function get_email_addresses_from_groups($emails = array()) {
      $data = array();

      $q = get_q_string($emails);

      $query = $this->db->query("SELECT g.email FROM `" . TABLE_GROUP_EMAIL . "` g WHERE g.id IN (SELECT u.id FROM `" . TABLE_GROUP_USER . "` u WHERE u.email IN ($q))", $emails);

      if(isset($query->rows)) {
         foreach ($query->rows as $q) {
            if(validemail($q['email']) && !in_array($q['email'], $data)) { array_push($data, $q['email']); }
         }
      }

      return $data;
   }


   public function get_users_all_domains($uid = 0) {
      $data = array();

      $session = Registry::get('session');

      array_push($data, $session->get('domain'));

      if($uid > 0) {
         $query = $this->db->query("SELECT domain FROM " . TABLE_DOMAIN_USER . " WHERE uid=?", array((int)$uid));

         foreach ($query->rows as $q) {
            array_push($data, $q['domain']);
         }
      }

      if($data) { array_shift($data); }

      return $data;
   }


   public function get_additional_uids($uid = 0) {
      $data = array();

      if($uid > 0) {
         $query = $this->db->query("SELECT gid FROM " . TABLE_EMAIL_LIST . " WHERE uid=?", array((int)$uid));

         if(isset($query->rows)) {
            foreach ($query->rows as $q) {
               array_push($data, $q['gid']);
            }
         }
      }

      return $data;
   }


   public function get_emails($username = '') {
      $emails = "";

      $query = $this->db->query("SELECT " . TABLE_EMAIL . ".email AS email FROM " . TABLE_EMAIL . "," . TABLE_USER . " WHERE " . TABLE_EMAIL . ".uid=" . TABLE_USER . ".uid AND " . TABLE_USER . ".username=?", array($username));

      foreach ($query->rows as $q) {
         $emails .= $q['email'] . "\n";
      }

      return preg_replace("/\n$/", "", $emails);
   }


   public function get_emails_by_uid($uid = 0) {
      $emails = "";

      $query = $this->db->query("SELECT email FROM " . TABLE_EMAIL . " WHERE uid=?", array((int)$uid));
      foreach ($query->rows as $q) {
         $emails .= $q['email'] . "\n";
      }

      return preg_replace("/\n$/", "", $emails);
   }


   public function get_domains_by_uid($uid = 0) {
      $domains = "";

      $query = $this->db->query("SELECT domain FROM " . TABLE_DOMAIN_USER . " WHERE uid=?", array((int)$uid));

      foreach ($query->rows as $q) {
         $domains .= $q['domain'] . "\n";
      }

      return preg_replace("/\n$/", "", $domains);
   }


   public function get_primary_email_by_domain($uid = 0, $domain = '') {
      $email = "";

      $query = $this->db->query("SELECT email FROM " . TABLE_EMAIL . " WHERE uid=?", array((int)$uid));

      if(isset($query->row)) { $email = $query->row['email']; }

      foreach ($query->rows as $q) {
         if(preg_match("/\@$domain$/", $q['email'])) { return $q['email']; }
      }

      return $email;
   }


   public function get_user_by_dn($dn = '') {
      if($dn == '') { return array(); }

      $query = $this->db->query("SELECT * FROM " . TABLE_USER . " WHERE dn=?", array($dn));

      if($query->num_rows == 1) {
         return $query->row;
      }

      return array();
   }


   public function get_user_by_uid($uid = 0) {
      if(!is_numeric($uid) || (int)$uid < 0){
         return array();
      }

      $query = $this->db->query("SELECT * FROM " . TABLE_USER . " WHERE uid=?", array((int)$uid));

      return $query->row;
   }


   public function get_user_by_email($email = '') {
      if($email == '') {
         return array();
      }

      $query = $this->db->query("SELECT * FROM " . TABLE_USER . "," . TABLE_EMAIL . " WHERE " . TABLE_USER . ".uid=" . TABLE_EMAIL . ".uid AND email=?", array($email));

      return $query->row;
   }


   public function get_users($search = '', $page = 0, $page_len = 0, $sort = 'username', $order = 0) {
      $where_cond = " WHERE " . TABLE_USER . ".uid=" . TABLE_EMAIL . ".uid ";
      $_order = "";
      $users = array();
      $my_domain = array();
      $limit = "";
      $q = array();

      $from = (int)$page * (int)$page_len;

      $search = preg_replace("/\s{1,}/", "", $search);

      if($search){
         $where_cond .= " AND email like ? ";
         array_push($q, '%' . $search . '%');
      }

      /* sort order */

      if($order == 0) { $order = "ASC"; }
      else { $order = "DESC"; }

      $_order = "ORDER BY $sort $order";

      if($page_len > 0) { $limit = " LIMIT " . (int)$from . ", " . (int)$page_len; }

      $query = $this->db->query("SELECT " . TABLE_USER . ".uid, isadmin, username, realname, domain, MIN(email) as email FROM " . TABLE_USER . "," . TABLE_EMAIL . " $where_cond GROUP BY " . TABLE_USER . ".uid $_order $limit", $q);

      foreach ($query->rows as $q) {

         if(Registry::get('admin_user') == 1 || (isset($q['domain']) && $q['domain'] == $my_domain[0]) ) {
            $users[] = array(
                          'uid'          => $q['uid'],
                          'username'     => $q['username'],
                          'realname'     => $q['realname'],
                          'domain'       => isset($q['domain']) ? $q['domain'] : "",
                          'email'        => $q['email'],
                          'isadmin'      => $q['isadmin']
                         );
         }

      }

      return $users;
   }


   public function count_users($search = '') {
      $where_cond = "";
      $q = array();

      if($search){
         $where_cond .= " WHERE email like ? ";
         array_push($q, '%' . $search . '%');
      }

      $query = $this->db->query("SELECT COUNT(*) AS num, uid FROM " . TABLE_EMAIL . " $where_cond group by uid", $q);

      return $query->num_rows;
   }


   public function get_domains() {
      $data = array();

      $query = $this->db->query("SELECT DISTINCT mapped AS domain FROM " . TABLE_DOMAIN);

      foreach ($query->rows as $q) {
         array_push($data, $q['domain']);
      }

      return $data;
   }


   public function get_email_domains() {
      $data = array();

      $query = $this->db->query("SELECT domain FROM " . TABLE_DOMAIN);

      foreach ($query->rows as $q) {
         array_push($data, $q['domain']);
      }

      return $data;
   }


   public function get_next_uid() {

      $query = $this->db->query("SELECT MAX(uid) AS last_id FROM " . TABLE_EMAIL);

      if(isset($query->row['last_id']) && $query->row['last_id'] > 0) {
         return (int)$query->row['last_id'] + 1;
      }

      return 1;
   }


   public function add_user($user) {
      LOGGER("add user: " . $user['username'] . ", uid=" . (int)$user['uid']);

      if(!isset($user['domain']) || $user['domain'] == "") { return -1; }
      if(!isset($user['username']) || $user['username'] == "" || $this->get_uid_by_name($user['username']) > 0) { return -1; }

      $emails = explode("\n", $user['email']);
      foreach ($emails as $email) {
         $email = rtrim($email);

         if(validemail($email) == 0) { continue; }

         $query = $this->db->query("SELECT COUNT(*) AS count FROM " . TABLE_EMAIL . " WHERE email=?", array($email));

         /* remove from memcached */

         if(MEMCACHED_ENABLED) {
            $memcache = Registry::get('memcache');
            $memcache->delete(MEMCACHED_PREFIX . $email);
         }

         if($query->row['count'] > 0) {
            return $email;
         }
      }


      $query = $this->db->query("SELECT COUNT(*) AS count FROM " . TABLE_USER . " WHERE username=?", array($user['username']));
      if($query->row['count'] > 0) {
         return $user['username'];
      }

      $samaccountname = '';
      if(isset($user['samaccountname'])) { $samaccountname = $user['samaccountname']; }

      $query = $this->db->query("INSERT INTO " . TABLE_USER . " (uid, username, realname, password, domain, dn, isadmin, samaccountname) VALUES(?,?,?,?,?,?,?,?)", array((int)$user['uid'], $user['username'], $user['realname'], encrypt_password($user['password']), $user['domain'], @$user['dn'], (int)$user['isadmin'], $samaccountname));

      if($query->error == 1 || $this->db->countAffected() == 0){ return $user['username']; }

      foreach ($emails as $email) {
         $email = rtrim($email);

         if(validemail($email) == 0) { continue; }

         $ret = $this->add_email((int)$user['uid'], $email);
         if($ret == 0) { return -2; }
      }

      $this->update_domains_settings($user);

      return 1;
   }


   public function add_email($uid = 0, $email = '') {
      if($uid < 1 || $email == ""){ return 0; }

      $query = $this->db->query("INSERT INTO " . TABLE_EMAIL . " (uid, email) VALUES(?,?)", array((int)$uid, $email));

      $rc = $this->db->countAffected();

      LOGGER("add email: $email, uid=$uid (rc=$rc)");

      return $rc;
   }


   public function remove_email($uid = 0, $email = '') {
      if((int)$uid < 1 || $email == ""){ return 0; }

      $query = $this->db->query("DELETE FROM " . TABLE_EMAIL . " WHERE uid=? AND email=?", array((int)$uid, $email));

      $rc = $this->db->countAffected();

      LOGGER("remove email: $email, uid=$uid (rc=$rc)");

      return $rc;
   }


   public function update_user($user) {
      LOGGER("update user: " . $user['username'] . ", uid=" . (int)$user['uid']);

      $emails = explode("\n", $user['email']);
      foreach ($emails as $email) {
         $email = rtrim($email);

         if(validemail($email) == 0) { continue; }

         $query = $this->db->query("SELECT COUNT(*) AS count FROM " . TABLE_EMAIL . " WHERE uid!=? AND email=?", array((int)$user['uid'], $email));

         if($query->row['count'] > 0) {
            return $email;
         }
      }


      /* update password field if we have to */

      if(strlen($user['password']) >= MIN_PASSWORD_LENGTH) {
         $query = $this->db->query("UPDATE " . TABLE_USER . " SET password=? WHERE uid=?", array(encrypt_password($user['password']), (int)$user['uid']));
         if($this->db->countAffected() != 1) { return 0; }
      }

      $query = $this->db->query("UPDATE " . TABLE_USER . " SET username=?, realname=?, domain=?, dn=?, isadmin=? WHERE uid=?", array($user['username'], $user['realname'], $user['domain'], @$user['dn'], $user['isadmin'], (int)$user['uid']));


      /* first, remove all his email addresses */

      $query = $this->db->query("DELETE FROM " . TABLE_EMAIL . " WHERE uid=?", array((int)$user['uid']));

      /* then add all the emails we have from the CGI post input */

      foreach ($emails as $email) {
         $email = rtrim($email);

         if(validemail($email) == 0) { continue; }

         $query = $this->db->query("INSERT INTO " . TABLE_EMAIL . " (uid, email) VALUES(?,?)", array((int)$user['uid'], $email));

         /* remove from memcached */

         if(MEMCACHED_ENABLED) {
            $memcache = Registry::get('memcache');
            $memcache->delete(MEMCACHED_PREFIX . $email);
         }

      }

      $this->update_domains_settings($user);

      return 1;
   }


   private function update_domains_settings($user = array()) {
      $__d = array();

      if((int)$user['uid'] <= 0) { return 0; }

      $query = $this->db->query("DELETE FROM `" . TABLE_DOMAIN_USER . "` WHERE uid=?", array($user['uid']));

      $query = $this->db->query("SELECT domain FROM " . TABLE_DOMAIN . " WHERE mapped=?", array($user['domain']));

      foreach ($query->rows as $q) {
         $query = $this->db->query("INSERT INTO `" . TABLE_DOMAIN_USER . "` (domain, uid) VALUES(?,?)", array($q['domain'], $user['uid']));
      }

      return 1;
   }


   private function update_group_settings($email = '', $group = '') {
      $__g = array();

      $email = rtrim($email);

      if($email == '') { return 0; }

      $query = $this->db->query("DELETE FROM `" . TABLE_GROUP_USER . "` WHERE email=?", array($email));

      $query = $this->db->query("SELECT id, groupname FROM `" . TABLE_GROUP . "`");

      $groups = array();

      foreach ($query->rows as $q) {
         $groups[$q['groupname']] = $q['id'];
      }

      $group = explode("\n", $group);

      foreach($group as $g) {
         $g = rtrim($g);

         if($g && !isset($__g[$groups[$g]])) {
            $query = $this->db->query("INSERT INTO `" . TABLE_GROUP_USER . "` (id, email) VALUES(?,?)", array($groups[$g], $email));
            $__g[$groups[$g]] = 1;
         }
      }

      return 1;
   }


   private function update_folder_settings($uid = -1, $folder = '') {
      $__g = array();

      if($uid <= 0) { return 0; }

      $query = $this->db->query("DELETE FROM `" . TABLE_FOLDER_USER . "` WHERE uid=?", array($uid));

      $query = $this->db->query("SELECT id, name FROM `" . TABLE_FOLDER . "`");

      $folders = array();

      foreach ($query->rows as $q) {
         $folders[$q['name']] = $q['id'];
      }

      $folder = explode("\n", $folder);

      foreach($folder as $g) {
         $g = rtrim($g);

         if($g && !isset($__g[$folders[$g]])) {
            $query = $this->db->query("INSERT INTO `" . TABLE_FOLDER_USER . "` (id, uid) VALUES(?,?)", array($folders[$g], (int)$uid));
            $__g[$folders[$g]] = 1;
         }
      }

      return 1;
   }


   public function update_dn_by_uid($uid = 0, $new_dn = '') {
      if(!$this->check_uid($uid) || $new_dn == ''){ return 0; }

      $user = $this->get_user_by_uid($uid);

      if(isset($user['uid']) && $user['dn'] != '' && $user['dn'] != '*') {
         $query = $this->db->query("UPDATE " . TABLE_USER . " SET dn=? WHERE uid=?", array($new_dn, $uid));
         return 1;
      }

      return 0;
   }


   public function delete_user($uid) {
      if(!$this->check_uid($uid)){ return 0; }

      $query = $this->db->query("DELETE FROM " . TABLE_EMAIL . " WHERE uid=?", array((int)$uid));
      $query = $this->db->query("DELETE FROM " . TABLE_USER_SETTINGS . " WHERE username IN (SELECT username FROM " . TABLE_USER . " WHERE uid=?)", array((int)$uid));
      $query = $this->db->query("DELETE FROM " . TABLE_USER . " WHERE uid=?", array((int)$uid));
      $query = $this->db->query("DELETE FROM " . TABLE_DOMAIN_USER . " WHERE uid=?", array((int)$uid));
      $query = $this->db->query("DELETE FROM " . TABLE_FOLDER_USER . " WHERE uid=?", array((int)$uid));

      LOGGER("remove user: uid=$uid");

      return 1;
   }



}
