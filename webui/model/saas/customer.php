<?php

class ModelSaasCustomer extends Model
{

   public function get($id = -1) {

      if($id > 0) {
         $query = $this->db->query("SELECT * FROM " . TABLE_CUSTOMER_SETTINGS . " WHERE id=?", array($id));
         if($query->num_rows > 0) { return $query->row; }
      }

      $query = $this->db->query("SELECT * FROM " . TABLE_CUSTOMER_SETTINGS . " ORDER BY domain ASC");

      if($query->num_rows > 0) { return $query->rows; }

      return array();
   }


   public function delete($id = 0, $description = '') {
      if($id == 0) { return 0; }

      $query = $this->db->query("DELETE FROM " . TABLE_CUSTOMER_SETTINGS . " WHERE id=?", array($id));

      $rc = $this->db->countAffected();

      LOGGER("remove ldap entry: #$id, $description  (rc=$rc)");

      return $rc;
   }


   public function add($arr = array()) {
      $branding_logo = '';

      if(!isset($arr['domain']) || !isset($arr['branding_text'])) { return 0; }

      if(isset($_FILES['branding_logo']['name'])) {
         $branding_logo = $_FILES['branding_logo']['name'];
         move_uploaded_file($_FILES['branding_logo']['tmp_name'], DIR_BASE . "/images/" . $_FILES['branding_logo']['name']);
      }

      $query = $this->db->query("INSERT INTO " . TABLE_CUSTOMER_SETTINGS . " (domain, branding_text, branding_url, branding_logo, support_link, colour) VALUES (?,?,?,?,?,?)", array($arr['domain'], $arr['branding_text'], $arr['branding_url'], $branding_logo, $arr['support_link'], $arr['colour']));

      $rc = $this->db->countAffected();

      LOGGER("add ldap entry: " . $arr['domain'] . " / " . $arr['branding_text'] . " / " . $arr['branding_url'] . " / " . $arr['support_link'] . " (rc=$rc)");

      if($rc == 1){ return 1; }

      return 0;
   }


   public function update($arr = array()) {
      $branding_logo = '';

      if(!isset($arr['id']) || !isset($arr['domain']) || !isset($arr['branding_text'])) { return 0; }

      if(isset($_FILES['branding_logo']['name'])) {
         $branding_logo = $_FILES['branding_logo']['name'];
         move_uploaded_file($_FILES['branding_logo']['tmp_name'], DIR_BASE . "/images/" . $_FILES['branding_logo']['name']);

         $query = $this->db->query("UPDATE " . TABLE_CUSTOMER_SETTINGS . " SET domain=?, branding_text=?, branding_url=?, branding_logo=?, support_link=?, colour=? WHERE id=?", array($arr['domain'], $arr['branding_text'], $arr['branding_url'], $branding_logo, $arr['support_link'], $arr['colour'], $arr['id']));
      }
      else {
         $query = $this->db->query("UPDATE " . TABLE_CUSTOMER_SETTINGS . " SET domain=?, branding_text=?, branding_url=?, support_link=?, colour=? WHERE id=?", array($arr['domain'], $arr['branding_text'], $arr['branding_url'], $arr['support_link'], $arr['colour'], $arr['id']));
      }


      return $this->db->countAffected();
   }


   public function get_customer_settings_by_email() {
       $data = array(
                      'branding_text' => BRANDING_TEXT,
                      'branding_url' => BRANDING_URL,
                      'branding_logo' => BRANDING_LOGO,
                      'support_link' => SUPPORT_LINK,
                      'colour' => BRANDING_COLOUR
                    );


      if(!isset($_SESSION['email']) || !strchr($_SESSION['email'], '@') ) { return $data; }

      list ($user, $domain) = explode("@", $_SESSION['email']);

      if(MEMCACHED_ENABLED) {
         $cache_key = sha1("customer_settings:" . $domain);
         $memcache = Registry::get('memcache');
         $m = $memcache->get($cache_key);
         if(isset($m['data'])) { return unserialize($m['data']); }
      }

      $query = $this->db->query("SELECT * FROM " . TABLE_CUSTOMER_SETTINGS . " WHERE domain=(SELECT mapped FROM " . TABLE_DOMAIN . " WHERE domain=?)", array($domain));

      if($query->num_rows > 0) {
         if($query->row['branding_text']) { $data['branding_text'] = $query->row['branding_text']; }
         if($query->row['branding_url']) { $data['branding_url'] = $query->row['branding_url']; }
         if($query->row['branding_logo']) { $data['branding_logo'] = $query->row['branding_logo']; }
         if($query->row['support_link']) { $data['support_link'] = $query->row['support_link']; }
         if($query->row['colour']) { $data['colour'] = $query->row['colour']; }
      }

      if(MEMCACHED_ENABLED && $cache_key) {
         $memcache->add($cache_key, array('data' => serialize($data)), 0, MEMCACHED_TTL);
      }

      return $data;
   }


   public function online($username = '') {
      if($username == '') { return 0; }

      $query = $this->db->query("INSERT INTO " . TABLE_ONLINE . " (username, ts, last_activity, ipaddr) VALUES(?,?,?,?)", array($username, NOW, NOW, $_SERVER['REMOTE_ADDR']));

      if($this->db->countAffected() == 0) {
         $query = $this->db->query("UPDATE " . TABLE_ONLINE . " SET ts=?, last_activity=? WHERE username=? AND ipaddr=?", array(NOW, $username, $_SERVER['REMOTE_ADDR']));
      }

      return 1;
   }


   public function offline($username = '') {
      if($username == '') { return 0; }

      $query = $this->db->query("DELETE FROM " . TABLE_ONLINE . " WHERE username=? AND ipaddr=?", array($username, $_SERVER['REMOTE_ADDR']));

      return 1;
   }


   public function count_online() {
      $query = $this->db->query("SELECT COUNT(*) AS num FROM " . TABLE_ONLINE);

      return $query->row['num'];
   }


   public function get_online_users() {
      $query = $this->db->query("SELECT * FROM " . TABLE_ONLINE . " ORDER BY username ASC");

      return $query->rows;
   }


}

?>
