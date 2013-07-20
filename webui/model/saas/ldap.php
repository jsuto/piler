<?php

class ModelSaasLdap extends Model
{

   public function get() {

      $query = $this->db->query("SELECT id, description, ldap_type, ldap_host, ldap_base_dn, ldap_bind_dn FROM " . TABLE_LDAP . " ORDER BY description ASC");

      if($query->num_rows > 0) { return $query->rows; }

      return array();
   }


   public function delete($id = 0, $description = '') {
      if($id == 0) { return 0; }

      $query = $this->db->query("DELETE FROM " . TABLE_LDAP . " WHERE id=?", array($id));

      $rc = $this->db->countAffected();

      LOGGER("remove ldap entry: #$id, $description  (rc=$rc)");

      return $rc;
   }


   public function add($arr = array()) {
      if(!isset($arr['description']) || !isset($arr['ldap_host'])) { return 0; }

      $query = $this->db->query("INSERT INTO " . TABLE_LDAP . " (description, ldap_host, ldap_base_dn, ldap_bind_dn, ldap_bind_pw, ldap_type) VALUES (?,?,?,?,?,?)", array($arr['description'], $arr['ldap_host'], $arr['ldap_base_dn'], $arr['ldap_bind_dn'], $arr['ldap_bind_pw'], $arr['ldap_type']));

      $rc = $this->db->countAffected();

      LOGGER("add ldap entry: " . $arr['description'] . " / " . $arr['ldap_type'] . " / " . $arr['ldap_host'] . " / " . $arr['ldap_base_dn'] . " (rc=$rc)");

      if($rc == 1){ return 1; }

      return 0;
   }


   public function get_ldap_params_by_email($email = '') {
      $domain = '';

      if($email == '') { return array(); }

      list($l,$d) = explode("@", $email);

      $query = $this->db->query("SELECT ldap_type, ldap_host, ldap_base_dn, ldap_bind_dn, ldap_bind_pw from " . TABLE_DOMAIN . " as d, " . TABLE_LDAP . " as l where d.ldap_id=l.id and d.domain=?", array($d));

      if($query->num_rows > 0) { return array($query->row['ldap_type'], $query->row['ldap_host'], $query->row['ldap_base_dn'], $query->row['ldap_bind_dn'], $query->row['ldap_bind_pw']); }

      return array();
   }


   public function get_accounts_in_domain($domain = '') {
      $ldap_type = '';
      $ldap_host = LDAP_HOST;
      $ldap_base_dn = LDAP_BASE_DN;
      $ldap_helper_dn = LDAP_HELPER_DN;
      $ldap_helper_password = LDAP_HELPER_PASSWORD;

      if(ENABLE_SAAS == 1) {
         $a = $this->model_saas_ldap->get_ldap_params_by_email("aaa@" . $domain);

         if(count($a) >= 5) {
            $ldap_type = $a[0];
            $ldap_host = $a[1];
            $ldap_base_dn = $a[2];
            $ldap_helper_dn = $a[3];
            $ldap_helper_password = $a[4];
         }
      }

      list($ldap_mail_attr, $ldap_account_objectclass, $ldap_distributionlist_attr, $ldap_distributionlist_objectclass) = get_ldap_attribute_names($ldap_type);

      if($ldap_host == '' || $ldap_helper_password == '') { return array(); }

      $ldap = new LDAP($ldap_host, $ldap_helper_dn, $ldap_helper_password);

      if($ldap->is_bind_ok()) {

         $query = $ldap->query($ldap_base_dn, "(&(objectClass=$ldap_account_objectclass)($ldap_mail_attr=*@$domain))", array($ldap_mail_attr));

         if($query->num_rows > 0) { asort($query->rows); return $query->rows; }
      }

      return array();
   }

}

?>
