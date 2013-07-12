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

}

?>
