<?php

class ModelUserUser extends Model {


   public function checkUID($uid) {
      if($uid == "") { return 0; }

      if(!is_numeric($uid)) { return 0; }

      if($uid < 1) { return 0; }

      return 1;
   }


   public function getUidByName($username = '') {
      if($username == ""){ return -1; }

      $query = $this->db->query("SELECT uid FROM " . TABLE_USER . " WHERE username=?", array($username));

      if(isset($query->row['uid'])){
         return $query->row['uid'];
      }

      return -1;
   }


   public function getUsernameByUid($uid = 0) {

      $query = $this->db->query("SELECT username FROM " . TABLE_USER . " WHERE uid=?", array((int)$uid));

      if(isset($query->row['username'])){
         return $query->row['username'];
      }

      return "";
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

      if($uid > 0) {
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


   public function getEmailAddress($username = '') {

      $query = $this->db->query("SELECT " . TABLE_EMAIL . ".email AS email FROM " . TABLE_EMAIL . "," . TABLE_USER . " WHERE " . TABLE_EMAIL . ".uid=" . TABLE_USER . ".uid AND " . TABLE_USER . ".username=? LIMIT 1", array($username));

      if(isset($query->row['email'])){
         return $query->row['email'];
      }

      return "";
   }


   public function getEmails($username = '') {
      $emails = "";

      $query = $this->db->query("SELECT " . TABLE_EMAIL . ".email AS email FROM " . TABLE_EMAIL . "," . TABLE_USER . " WHERE " . TABLE_EMAIL . ".uid=" . TABLE_USER . ".uid AND " . TABLE_USER . ".username=?", array($username));

      foreach ($query->rows as $q) {
         $emails .= $q['email'] . "\n";
      }

      return preg_replace("/\n$/", "", $emails);
   }


   public function getEmailsByUid($uid = 0) {
      $emails = "";

      $query = $this->db->query("SELECT email FROM " . TABLE_EMAIL . " WHERE uid=?", array((int)$uid));
      foreach ($query->rows as $q) {
         $emails .= $q['email'] . "\n";
      }

      return preg_replace("/\n$/", "", $emails);
   }


   public function getUserByDN($dn = '') {
      if($dn == '') { return array(); }

      $query = $this->db->query("SELECT * FROM " . TABLE_USER . " WHERE dn=?", array($dn));

      if($query->num_rows == 1) {
         return $query->row;
      }

      return array();
   }


   public function getUserByUid($uid = 0) {
      if(!is_numeric($uid) || (int)$uid < 0){
         return array();
      }

      $query = $this->db->query("SELECT * FROM " . TABLE_USER . " WHERE uid=?", array((int)$uid));

      return $query->row;
   }


   public function getUserByEmail($email = '') {
      if($email == '') {
         return array();
      }

      $query = $this->db->query("SELECT * FROM " . TABLE_USER . "," . TABLE_EMAIL . " WHERE " . TABLE_USER . ".uid=" . TABLE_EMAIL . ".uid AND email=?", array($email));

      return $query->row;
   }


   public function getUsernameByEmail($email = '') {
      $username = "";

      if($email == '') { return $username; }

      $query = $this->db->query("SELECT username FROM " . TABLE_USER . " WHERE uid IN (SELECT uid FROM " . TABLE_EMAIL . " WHERE email=?)", array($email));

      if(isset($query->row['username'])) { $username = $query->row['username']; }

      return $username;
   }


   public function getUsers($search = '', $page = 0, $page_len = 0, $sort = 'username', $order = 0) {
      $where_cond = " WHERE " . TABLE_USER . ".uid=" . TABLE_EMAIL . ".uid ";
      $_order = "";
      $users = array();
      $my_domain = array();
      $limit = "";

      $from = (int)$page * (int)$page_len;

      $search = preg_replace("/\s{1,}/", "", $search);

      if($search){
         $where_cond .= " AND email like '%" . $this->db->escape($search) . "%' ";
      }

      /* sort order */

      if($order == 0) { $order = "ASC"; }
      else { $order = "DESC"; }

      $_order = "ORDER BY $sort $order";

      if($page_len > 0) { $limit = " LIMIT " . (int)$from . ", " . (int)$page_len; }

      $query = $this->db->query("SELECT " . TABLE_USER . ".uid, isadmin, username, realname, domain, email FROM " . TABLE_USER . "," . TABLE_EMAIL . " $where_cond group by " . TABLE_USER . ".uid $_order $limit");

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


   public function howManyUsers($search = '') {
      $where_cond = "";

      if($search){
         $where_cond .= " WHERE email like '%" . $this->db->escape($search) . "%' ";
      }

      $query = $this->db->query("SELECT COUNT(*) AS num, uid FROM " . TABLE_EMAIL . " $where_cond group by uid");

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


   public function getNextUid() {

      $query = $this->db->query("SELECT MAX(uid) AS last_id FROM " . TABLE_USER);

      if(isset($query->row['last_id']) && $query->row['last_id'] > 0) {
         return (int)$query->row['last_id'] + 1;
      }

      return 1;
   }


   public function addUser($user) {
      LOGGER("add user: " . $user['username'] . ", uid=" . (int)$user['uid']);

      if(!isset($user['domain']) || $user['domain'] == "") { return -1; }
      if(!isset($user['username']) || $user['username'] == "" || $this->getUidByName($user['username']) > 0) { return -1; }

      $emails = explode("\n", $user['email']);
      foreach ($emails as $email) {
         $email = rtrim($email);

         $query = $this->db->query("SELECT COUNT(*) AS count FROM " . TABLE_EMAIL . " WHERE email=?", array($email));

         /* remove from memcached */

         if(MEMCACHED_ENABLED) {
            $memcache = Registry::get('memcache');
            $memcache->delete("_c:" . $email);
         }

         if($query->row['count'] > 0) {
            return $email;
         }
      }


      $query = $this->db->query("SELECT COUNT(*) AS count FROM " . TABLE_USER . " WHERE username=?", array($user['username']));
      if($query->row['count'] > 0) {
         return $user['username'];
      }

      $encrypted_password = crypt($user['password']);

      $query = $this->db->query("INSERT INTO " . TABLE_USER . " (uid, username, realname, password, domain, dn, isadmin) VALUES(?,?,?,?,?,?,?)", array((int)$user['uid'], $user['username'], $user['realname'], $encrypted_password, $user['domain'], @$user['dn'], (int)$user['isadmin']));

      if($query->error == 1 || $this->db->countAffected() == 0){ return $user['username']; }

      foreach ($emails as $email) {
         $email = rtrim($email);

         $ret = $this->addEmail((int)$user['uid'], $email);
         if($ret == 0) { return -2; }
      }


      return 1;
   }


   public function addEmail($uid = 0, $email = '') {
      if($uid < 1 || $email == ""){ return 0; }

      $query = $this->db->query("INSERT INTO " . TABLE_EMAIL . " (uid, email) VALUES(?,?)", array((int)$uid, $email));

      $rc = $this->db->countAffected();

      LOGGER("add email: $email, uid=$uid (rc=$rc)");

      return $rc;
   }


   public function removeEmail($uid = 0, $email = '') {
      if((int)$uid < 1 || $email == ""){ return 0; }

      $query = $this->db->query("DELETE FROM " . TABLE_EMAIL . " WHERE uid=? AND email=?", array((int)$uid, $email));

      $rc = $this->db->countAffected();

      LOGGER("remove email: $email, uid=$uid (rc=$rc)");

      return $rc;
   }


   public function updateUser($user) {
      LOGGER("update user: " . $user['username'] . ", uid=" . (int)$user['uid']);

      $emails = explode("\n", $user['email']);
      foreach ($emails as $email) {
         $email = rtrim($email);

         $query = $this->db->query("SELECT COUNT(*) AS count FROM " . TABLE_EMAIL . " WHERE uid!=? AND email=?", array((int)$user['uid'], $email));

         if($query->row['count'] > 0) {
            return $email;
         }
      }


      /* update password field if we have to */
 
      if(strlen($user['password']) >= MIN_PASSWORD_LENGTH) {
         $query = $this->db->query("UPDATE " . TABLE_USER . " SET password=? WHERE uid=?", array(crypt($user['password']), (int)$user['uid']));
         if($this->db->countAffected() != 1) { return 0; }
      }

      $query = $this->db->query("UPDATE " . TABLE_USER . " SET username=?, realname=?, domain=?, dn=?, isadmin=? WHERE uid=?", array($user['username'], $user['realname'], $user['domain'], @$user['dn'], $user['isadmin'], (int)$user['uid']));


      /* first, remove all his email addresses */

      $query = $this->db->query("DELETE FROM " . TABLE_EMAIL . " WHERE uid=?", array((int)$user['uid']));

      /* then add all the emails we have from the CGI post input */

      foreach ($emails as $email) {
         $email = rtrim($email);
         $query = $this->db->query("INSERT INTO " . TABLE_EMAIL . " (uid, email) VALUES(?,?)", array((int)$user['uid'], $email));

         /* remove from memcached */

         if(MEMCACHED_ENABLED) {
            $memcache = Registry::get('memcache');
            $memcache->delete("_c:" . $email);
         }

      }

      return 1;
   }


   public function deleteUser($uid) {
      if(!$this->checkUID($uid)){ return 0; }

      $query = $this->db->query("DELETE FROM " . TABLE_EMAIL . " WHERE uid=?", array((int)$uid));
      $query = $this->db->query("DELETE FROM " . TABLE_USER . " WHERE uid=?", array((int)$uid));

      LOGGER("remove user: uid=$uid");

      return 1;
   }



}

?>
