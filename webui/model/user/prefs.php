<?php

class ModelUserPrefs extends Model {

   public function get_user_preferences($username = '') {
      if($username == "") { return 0; }

      $session = Registry::get('session');

      $query = $this->db->query("SELECT * FROM " . TABLE_USER_SETTINGS . " WHERE username=?", array($username));

      if(isset($query->row['pagelen'])) { $session->set("pagelen", $query->row['pagelen']); } else { $session->set("pagelen", PAGE_LEN); }
      if(isset($query->row['theme'])) { $session->set("theme", $query->row['theme']); } else { $session->set("theme", THEME); }
      if(isset($query->row['lang'])) { $session->set("lang", $query->row['lang']); } else { $session->set("lang", DEFAULT_LANG); }

      return 1;
   }


   public function set_user_preferences($username = '', $prefs = array() ) {

      if(!isset($prefs['pagelen']) || !is_numeric($prefs['pagelen']) || $prefs['pagelen'] < 10 || $prefs['pagelen'] > 1000) {
         return 1;
      }

      $session = Registry::get('session');

      $query = $this->db->query("SELECT COUNT(*) AS num FROM " . TABLE_USER_SETTINGS . " WHERE username=?", array($username));

      if((int)@$query->row['num'] == 1) {
         $query = $this->db->query("UPDATE " . TABLE_USER_SETTINGS . " SET pagelen=?, lang=? WHERE username=?", array((int)@$prefs['pagelen'], $prefs['lang'], $username));
      }
      else {
         $query = $this->db->query("INSERT INTO " . TABLE_USER_SETTINGS . " (username, pagelen, theme, lang) VALUES(?,?,?,?)", array($username, (int)@$prefs['pagelen'], 'default', $prefs['lang']));
      }


      $session->set("pagelen", $prefs['pagelen']);
      $session->set("lang", $prefs['lang']);

      LOGGER("set user preference", $username);

      return 1;
   }



   public function get_ga_settings($username = '') {
      $data = array('ga_enabled' => 0, 'ga_secret' => '');

      if($username == ""){ return $data; }

      $GA = new PHPGangsta_GoogleAuthenticator();

      $query = $this->db->query("SELECT ga_enabled, ga_secret FROM " . TABLE_USER_SETTINGS . " WHERE username=?", array($username));

      if(isset($query->row['ga_enabled'])) {
         $data['ga_enabled'] = $query->row['ga_enabled'];
         $data['ga_secret'] = $query->row['ga_secret'];

         if($data['ga_secret'] == '') {
            $data['ga_secret'] = $GA->createSecret();
            $this->update_ga_secret($username, $data['ga_secret']);
         }
      }
      else {
         $query = $this->db->query("INSERT INTO " . TABLE_USER_SETTINGS . " (username, ga_enabled, ga_secret) VALUES(?,0,?)", array($username, $GA->createSecret()));
      }


      return $data;
   }


   public function update_ga_secret($username = '', $ga_secret = '') {
      if($username == "" || $ga_secret == "") { return 0; }

      $query = $this->db->query("UPDATE " . TABLE_USER_SETTINGS . " SET ga_secret=? WHERE username=?", array($ga_secret, $username));

      return 1;
   }


   public function toggle_ga($username = '', $ga_enabled = '') {
      if($username == "" || $ga_enabled < 0 || $ga_enabled > 1) { return 0; }

      // In demo mode you can't enable Google AUthenticator since other users
      // would NOT be able to login without the GA secret
      if(DEMO_MODE && in_array($username, ['admin@local', 'auditor@local'])) { return 1; }

      $query = $this->db->query("UPDATE " . TABLE_USER_SETTINGS . " SET ga_enabled=? WHERE username=?", array($ga_enabled, $username));

      return 1;
   }


}

?>
