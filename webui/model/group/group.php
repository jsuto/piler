<?php

class ModelGroupGroup extends Model {


   public function get_groups($search = '', $page = 0, $page_len = 0, $sort = 'groupname', $order = 0) {
      $where_cond = '';
      $_order = "";
      $groups = array();
      $Q = array();
      $limit = "";

      $from = (int)$page * (int)$page_len;

      $search = preg_replace("/\s{1,}/", "", $search) . '%';

      if($search){
         $where_cond .= " WHERE `groupname` like ?";
         array_push($Q, $search);
      }

      /* sort order */

      if($order == 0) { $order = "ASC"; }
      else { $order = "DESC"; }

      $_order = "ORDER BY `$sort` $order";

      if($page_len > 0) { $limit = " LIMIT " . (int)$from . ", " . (int)$page_len; }

      $query = $this->db->query("SELECT `id`, `groupname` FROM `" . TABLE_GROUP . "` $where_cond $_order $limit", $Q);

      foreach ($query->rows as $q) {

         $groups[] = array(
                          'id'          => $q['id'],
                          'groupname'   => $q['groupname']
                         );
      }

      return $groups;
   }


   public function get_emails_by_group_id($id = 0) {
      $emails = '';

      $query = $this->db->query("SELECT `email` FROM `" . TABLE_GROUP_USER . "` WHERE id=?", array($id));

      foreach ($query->rows as $q) {
         $emails .= $q['email'] . "\n";
      }

      return preg_replace("/\n$/", "", $emails);
   }


   public function get_assigned_emails_by_group_id($id = 0) {
      $emails = '';

      $query = $this->db->query("SELECT `email` FROM `" . TABLE_GROUP_EMAIL . "` WHERE id=?", array($id));

      foreach ($query->rows as $q) {
         $emails .= $q['email'] . "\n";
      }

      return preg_replace("/\n$/", "", $emails);
   }


   public function count_groups($search = '') {
      $where_cond = "";
      $Q = array();

      $search = preg_replace("/\s{1,}/", "", $search) . '%';

      if($search){
         $where_cond .= " WHERE `groupname` like '?'";
         array_push($Q, $search);
      }

      $query = $this->db->query("SELECT COUNT(*) AS num FROM `" . TABLE_GROUP . "` $where_cond", $Q);

      return $query->num_rows;
   }


   public function add_group($group = array()) {

      if(!isset($group['groupname']) || $group['groupname'] == "") { return -1; }

      $query = $this->db->query("INSERT INTO `" . TABLE_GROUP . "` (groupname) VALUES(?)", array($group['groupname']) );

      if($query->error == 1 || $this->db->countAffected() == 0){ return $group['groupname']; }

      $gid = $this->db->getLastId();

      $emails = explode("\n", $group['email']);
      foreach ($emails as $email) {
         $email = rtrim($email);
         if(validemail($email)) {
            $query = $this->db->query("INSERT INTO `" . TABLE_GROUP_USER . "` (id, email) VALUES(?,?)", array($gid, $email));
         }
      }


      $emails = explode("\n", $group['assigned_email']);
      foreach ($emails as $email) {
         $email = rtrim($email);
         if(validemail($email)) {
            $query = $this->db->query("INSERT INTO `" . TABLE_GROUP_EMAIL . "` (id, email) VALUES(?,?)", array($gid, $email));
         }
      }

      LOGGER("add group: " . $group['groupname'] . ", id=" . (int)$gid);

      return 1;
   }


   public function update_group($group = array()) {
      LOGGER("update user: " . $group['groupname'] . ", id=" . (int)$group['id']);

      $query = $this->db->query("UPDATE `" . TABLE_GROUP . "` SET `groupname`=? WHERE id=?", array($group['groupname'], (int)$group['id']));

      $query = $this->db->query("DELETE FROM `" . TABLE_GROUP_USER . "` WHERE id=?", array($group['id']));

      $emails = explode("\n", $group['email']);
      foreach ($emails as $email) {
         $email = rtrim($email);

         if(validemail($email)) {
            $query = $this->db->query("INSERT INTO `" . TABLE_GROUP_USER . "` (id, email) VALUES(?,?)", array($group['id'], $email));
         }
      }


      $query = $this->db->query("DELETE FROM `" . TABLE_GROUP_EMAIL . "` WHERE id=?", array($group['id']));

      $emails = explode("\n", $group['assigned_email']);
      foreach ($emails as $email) {
         $email = rtrim($email);

         if(validemail($email)) {
            $query = $this->db->query("INSERT INTO `" . TABLE_GROUP_EMAIL . "` (id, email) VALUES(?,?)", array($group['id'], $email));
         }
      }


      return $this->db->countAffected();
   }


   public function get_domain_by_id($id = 0) {
      if(!is_numeric($id) || (int)$id < 0){
         return array();
      }

      $query = $this->db->query("SELECT * FROM `" . TABLE_GROUP . "` WHERE id=?", array((int)$id));

      return $query->row;
   }


   public function delete_group($id = 0) {

      $query = $this->db->query("DELETE FROM `" . TABLE_GROUP_EMAIL . "` WHERE id=?", array($id));

      $query = $this->db->query("DELETE FROM `" . TABLE_GROUP_USER . "` WHERE id=?", array($id));

      $query = $this->db->query("DELETE FROM `" . TABLE_GROUP . "` WHERE id=?", array((int)$id));

      LOGGER("remove group: id=$id");

      return 1;
   }


   public function get_emails_by_string($s = '', $page = 0, $page_len = PAGE_LEN) {
      $emails = array();

      $from = (int)$page * (int)$page_len;

      if(strlen($s) < 1) { return array(); }

      if(ENABLE_LDAP_AUTH == 1) {
         $ldap = new LDAP(LDAP_HOST, LDAP_HELPER_DN, LDAP_HELPER_PASSWORD);
         if($ldap->is_bind_ok()) {

            $query = $ldap->query(LDAP_BASE_DN, "(&(objectClass=" . LDAP_ACCOUNT_OBJECTCLASS . ")(" . LDAP_MAIL_ATTR . "=" . $s . "*))", array());

            if(isset($query->rows)) {
               $emails = $this->model_user_auth->get_email_array_from_ldap_attr($query->rows);
            }
         }
      }


      $query = $this->db->query("SELECT email FROM `" . TABLE_EMAIL . "` WHERE email LIKE ? ORDER BY email ASC  LIMIT " . (int)$from . ", " . (int)$page_len, array($s . "%") );

      if(isset($query->rows)) {
         foreach($query->rows as $q) {
            array_push($emails, $q['email']);
         }
      }

      sort($emails);

      if(strlen($s) == 1 && ENABLE_LDAP_AUTH == 1) {
         $emails = array_slice($emails, $page * $page_len, $page_len);
      }

      return $emails;
   }


   public function count_emails($s = '') {
      $count = 0;

      if(strlen($s) < 1) { return $count; }

      if(ENABLE_LDAP_AUTH == 1) {
         $ldap = new LDAP(LDAP_HOST, LDAP_HELPER_DN, LDAP_HELPER_PASSWORD);
         if($ldap->is_bind_ok()) {

            $query = $ldap->query(LDAP_BASE_DN, "(&(objectClass=" . LDAP_ACCOUNT_OBJECTCLASS . ")(" . LDAP_MAIL_ATTR . "=" . $s . "*))", array());

            if(isset($query->rows)) {
               $count = $query->num_rows;
            }
         }
      }

      $query = $this->db->query("SELECT COUNT(*) AS num FROM `" . TABLE_EMAIL . "` WHERE email LIKE ?", array($s . "%") );

      $count += $query->row['num'];

      return $count;
   }


   public function get_groups_by_string($s = '') {
      if(strlen($s) < 2) { return array(); }

      $query = $this->db->query("SELECT groupname FROM `" . TABLE_GROUP . "` WHERE groupname LIKE ? ORDER BY groupname ASC", array($s . "%") );

      if(isset($query->rows)) { return $query->rows; }

      return array();
   }


   public function get_groups_by_email($email = array()) {
      $groups = '';
      $q = '?';

      for($i=1; $i<count($email); $i++) {
         $q .= ',?';
      }

      $query = $this->db->query("SELECT `" . TABLE_GROUP_USER . "`.id, groupname FROM `" . TABLE_GROUP_USER . "`, `" . TABLE_GROUP . "` WHERE `" . TABLE_GROUP_USER . "`.id=`" . TABLE_GROUP . "`.id AND email IN ($q)", $email);

      if(isset($query->rows)) {
         foreach ($query->rows as $q) { $groups .= "\n" . $q['groupname']; }
      }

      return preg_replace("/^\n/", "", $groups);
   }


}

?>
