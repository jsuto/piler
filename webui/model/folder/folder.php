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


   private function get_sub_folders($id = 0, &$arr = array()) {
      $query = $this->db->query("SELECT id FROM `" . TABLE_FOLDER . "` WHERE parent_id=?", array($id));

      if(isset($query->rows)) {
         foreach ($query->rows as $q) {
            array_push($arr, $q['id']);
            $this->get_sub_folders($q['id'], $arr);
         }
      }

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


}

?>
