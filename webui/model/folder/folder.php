<?php

class ModelFolderFolder extends Model {


   public function get_folders($search = '', $page = 0, $page_len = 0, $sort = 'name', $order = 0) {
      $where_cond = '';
      $_order = "";
      $groups = array();
      $Q = array();
      $limit = "";

      $from = (int)$page * (int)$page_len;

      $search = preg_replace("/\s{1,}/", "", $search) . '%';

      if($search){
         $where_cond .= " WHERE `name` like ?";
         array_push($Q, $search);
      }

      /* sort order */

      if($order == 0) { $order = "ASC"; }
      else { $order = "DESC"; }

      $_order = "ORDER BY `$sort` $order";

      if($page_len > 0) { $limit = " LIMIT " . (int)$from . ", " . (int)$page_len; }

      $query = $this->db->query("SELECT `id`, `name` FROM `" . TABLE_FOLDER . "` $where_cond $_order $limit", $Q);

      foreach ($query->rows as $q) {

         $groups[] = array(
                          'id'          => $q['id'],
                          'foldername'   => $q['name']
                         );
      }

      return $groups;
   }


   public function get_folders_for_user() {
      $q = str_repeat("?,", count($_SESSION['folders']));
      $q = preg_replace("/\,$/", "", $q);

      $query = $this->db->query("SELECT `id`, `name` FROM `" . TABLE_FOLDER . "` WHERE id IN ($q)", $_SESSION['folders']);

      if(isset($query->rows)) { return $query->rows; }

      return array();
   }


   public function get_extra_folders_for_user() {
      $query = $this->db->query("SELECT `id`, `name` FROM `" . TABLE_FOLDER_EXTRA . "` WHERE uid=? ORDER BY name", array($_SESSION['uid']));

      if(isset($query->rows)) { return $query->rows; }

      return array();
   }


   private function is_your_extra_folder($folder_id = 0) {
      $query = $this->db->query("SELECT `id` FROM `" . TABLE_FOLDER_EXTRA . "` WHERE uid=? AND id=?", array($_SESSION['uid'], $folder_id));
      if(isset($query->row['id'])) { return 1; }

      return 0;
   }


   public function copy_message_to_folder_by_id($folder_id = 0, $meta_id = 0) {
      if(!$this->is_your_extra_folder($folder_id)) { return -1; }

      $query = $this->db->query("INSERT INTO " . TABLE_FOLDER_MESSAGE . " (folder_id, id) VALUES(?,?)", array($folder_id, $meta_id));
      return $this->db->countAffected();
   }


   public function get_all_folder_ids($uid = 0) {
      $arr = array();

      $query = $this->db->query("SELECT id FROM `" . TABLE_FOLDER_USER . "` WHERE uid=?", array($uid));

      if(isset($query->rows)) {
         foreach ($query->rows as $q) {
            array_push($arr, $q['id']);
            $this->get_sub_folders($q['id'], $arr);
         }
      }

      return $arr;
   }


   public function get_all_folder_ids_hier($uid = 0) {
      $arr = array();
      $a = array();

      $query = $this->db->query("SELECT " . TABLE_FOLDER_USER . ".id AS id, " . TABLE_FOLDER . ".name AS name FROM " . TABLE_FOLDER_USER . ", " . TABLE_FOLDER . " WHERE uid=? AND " . TABLE_FOLDER_USER . ".id=" . TABLE_FOLDER . ".id", array($uid));

      if(isset($query->rows)) {
         foreach ($query->rows as $q) {
            $a = array('id' => $q['id'], 'name' => $q['name'], 'children' => array());
            $a['children'] = $this->get_sub_folders_hier($q['id']);
            array_push($arr, $a);
         }
      }

      return $arr;
   }


   public function get_all_extra_folder_ids($uid = 0) {
      $arr = array();

      $query = $this->db->query("SELECT id FROM `" . TABLE_FOLDER_EXTRA . "` WHERE uid=?", array($uid));

      if(isset($query->rows)) {
         foreach ($query->rows as $q) {
            array_push($arr, $q['id']);
         }
      }

      return $arr;
   }


   private function get_sub_folders($id = 0, &$arr = array()) {
      $query = $this->db->query("SELECT id FROM `" . TABLE_FOLDER . "` WHERE parent_id=?", array($id));

      if(isset($query->rows)) {
         foreach ($query->rows as $q) {
            array_push($arr, $q['id']);
            $this->get_sub_folders($q['id'], $arr);
         }
      }

   }


   private function get_sub_folders_hier($id = 0) {
      $arr = array();

      $query = $this->db->query("SELECT id, name FROM `" . TABLE_FOLDER . "` WHERE parent_id=?", array($id));

      if($query->num_rows > 0) {

         foreach ($query->rows as $q) {
            $a = array('id'=> $q['id'], 'name'=>$q['name'], 'children'=>array());
            $a['children'] = $this->get_sub_folders_hier($q['id']);
            array_push($arr, $a);
         }

      }

      return $arr;
   }


   public function get_folders_by_string($s = '') {
      if(strlen($s) < 2) { return array(); }

      $query = $this->db->query("SELECT name FROM `" . TABLE_FOLDER . "` WHERE parent_id=0 AND name LIKE ? ORDER BY name ASC", array($s . "%") );

      if(isset($query->rows)) { return $query->rows; }

      return array();
   }


   public function get_folders_by_uid($uid = 0) {
      $folders = '';

      $query = $this->db->query("SELECT `" . TABLE_FOLDER_USER . "`.id, name FROM `" . TABLE_FOLDER_USER . "`, `" . TABLE_FOLDER . "` WHERE `" . TABLE_FOLDER_USER . "`.id=`" . TABLE_FOLDER . "`.id AND uid=?", array($uid) );

      if(isset($query->rows)) {
         foreach ($query->rows as $q) { $folders .= "\n" . $q['name']; }
      }

      return preg_replace("/^\n/", "", $folders);
   }


   public function add_extra_folder($name = '') {
      if($name == '') { return -1; }

      $query = $this->db->query("INSERT INTO " . TABLE_FOLDER_EXTRA . " (uid, name) VALUES(?,?)", array($_SESSION['uid'], $name));
      return $this->db->countAffected();
   }


   public function remove_extra_folder($id = 0) {
      if($id == 0) { return -1; }

      $query = $this->db->query("DELETE FROM " . TABLE_FOLDER_EXTRA . " WHERE id=? AND uid=?", array($id, $_SESSION['uid']));
      if($this->db->countAffected() == 1) {
         $query = $this->db->query("DELETE FROM " . TABLE_FOLDER_MESSAGE . " WHERE folder_id=?", array($id));
         return $this->db->countAffected();
      }

      return 0;
   }


}

?>
