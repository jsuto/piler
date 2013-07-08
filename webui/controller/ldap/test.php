<?php


class ControllerLdapTest extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "ldap/list.tpl";
      $this->layout = "common/layout";


      $request = Registry::get('request');
      $db = Registry::get('db');
      $lang = Registry::get('language');

      $ldap = new LDAP($this->request->post['ldap_host'], $this->request->post['ldap_bind_dn'], $this->request->post['ldap_bind_pw']);

      if($ldap->is_bind_ok()) {
         print "<span class=\"text-success\">" . $lang->data['text_connection_ok'] . "</span> ";

         $query = $ldap->query($this->request->post['ldap_base_dn'], "(mail=*)", array());
         if($query->num_rows < 1) {
            print "<span class=\"text-error\">" . $lang->data['text_not_found_any_email_address'] . "</span> ";
         }
      }
      else {
         print "<span class=\"text-error\">" . $lang->data['text_connection_failed'] . "</span> ";
      }

   }


}

?>
