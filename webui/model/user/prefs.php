<?php

class ModelUserPrefs extends Model {

   public function get_user_preferences($username = '') {
      if($username == "") { return 0; }

      $query = $this->db->query("SELECT * FROM " . TABLE_USER_SETTINGS . " WHERE username=?", array($username));

      if(isset($query->row['pagelen'])) { $_SESSION['pagelen'] = $query->row['pagelen']; } else { $_SESSION['pagelen'] = PAGE_LEN; }
      if(isset($query->row['theme'])) { $_SESSION['theme'] = $query->row['theme']; } else { $_SESSION['theme'] = THEME; }

      return 1;
   }


   public function set_user_preferences($username = '', $prefs = array() ) {

      if(!isset($prefs['pagelen']) || !is_numeric($prefs['pagelen']) || $prefs['pagelen'] < 10 || $prefs['pagelen'] > 100
         || !isset($prefs['theme']) || !preg_match("/^([a-zA-Z0-9\-\_]+)$/", $prefs['theme']) || !file_exists(DIR_THEME . $prefs['theme']) ) { return 1; }

      $query = $this->db->query("SELECT COUNT(*) AS num FROM " . TABLE_USER_SETTINGS . " WHERE username=?", array($username));

      if((int)@$query->row['num'] == 1) {
         $query = $this->db->query("UPDATE " . TABLE_USER_SETTINGS . " SET pagelen=?, theme=? WHERE username=?", array((int)@$prefs['pagelen'], $prefs['theme'], $username));
      }
      else {
         $query = $this->db->query("INSERT INTO " . TABLE_USER_SETTINGS . " (username, pagelen, theme) VALUES(?,?,?)", array($username, (int)@$prefs['pagelen'], $prefs['theme']));
      }


      $_SESSION['pagelen'] = $prefs['pagelen'];
      $_SESSION['theme'] = $prefs['theme'];

      LOGGER("set user preference", $username);

      return 1;
   }

}

?>
