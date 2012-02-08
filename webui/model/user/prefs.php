<?php

class ModelUserPrefs extends Model {

   public function get_user_preferences($username = '') {
      if($username == "") { return 0; }

      $query = $this->db->query("SELECT * FROM " . TABLE_USER_SETTINGS . " WHERE username=?", array($username));

      if(isset($query->row['pagelen'])) { $_SESSION['pagelen'] = $query->row['pagelen']; } else { $_SESSION['pagelen'] = PAGE_LEN; }
      if(isset($query->row['lang'])) { $_SESSION['lang'] = $query->row['lang']; } else { $_SESSION['lang'] = LANG; }
      if(isset($query->row['theme'])) { $_SESSION['theme'] = $query->row['theme']; } else { $_SESSION['theme'] = THEME; }

      return 1;
   }


   public function set_user_preferences($username = '', $prefs = array() ) {

      if(!isset($prefs['pagelen']) || !is_numeric($prefs['pagelen']) || $prefs['pagelen'] < 10 || $prefs['pagelen'] > 100
         || !isset($prefs['lang']) || strlen($prefs['lang']) != 2 || !file_exists(DIR_LANGUAGE . $prefs['lang']) 
         || !isset($prefs['theme']) || !preg_match("/^([a-zA-Z0-9\-\_]+)$/", $prefs['theme']) || !file_exists(DIR_THEME . $prefs['theme']) ) { return 1; }

      $query = $this->db->query("SELECT COUNT(*) AS num FROM " . TABLE_USER_SETTINGS . " WHERE username=?", array($username));

      if((int)@$query->row['num'] == 1) {
         $query = $this->db->query("UPDATE " . TABLE_USER_SETTINGS . " SET lang=?, pagelen=?, theme=? WHERE username=?", array($prefs['lang'], (int)@$prefs['pagelen'], $prefs['theme'], $username));
      }
      else {
         $query = $this->db->query("INSERT INTO " . TABLE_USER_SETTINGS . " (username, pagelen, lang, theme) VALUES(?,?,?,?)", array($username, (int)@$prefs['pagelen'], $prefs['lang'], $prefs['theme']));
      }


      $_SESSION['pagelen'] = $prefs['pagelen'];
      $_SESSION['lang'] = $prefs['lang'];
      $_SESSION['theme'] = $prefs['theme'];

      LOGGER("set user preference", $username);

      return 1;
   }

}

?>
