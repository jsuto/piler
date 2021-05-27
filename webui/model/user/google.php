<?php

class ModelUserGoogle extends Model {

   public function check_for_account($google_account = array()) {

      $session = Registry::get('session');

      $query = $this->db->query("SELECT " . TABLE_USER . ".username, " . TABLE_USER . ".uid, " . TABLE_USER . ".realname, " . TABLE_USER . ".dn, " . TABLE_USER . ".password, " . TABLE_USER . ".isadmin, " . TABLE_USER . ".domain FROM " . TABLE_USER . ", " . TABLE_EMAIL . " WHERE " . TABLE_EMAIL . ".email=? AND " . TABLE_EMAIL . ".uid=" . TABLE_USER . ".uid", array($google_account['email']));

      if($query->num_rows == 1) {
         $user = $query->row;
      }
      else {

         $d = explode('@', $google_account['email']);

         $user['uid'] = $this->model_user_user->get_next_uid();

         $user['username'] = $google_account['email'];
         $user['realname'] = $google_account['name'];
         $user['email'] = $google_account['email'];
         $user['domain'] = $d[1];
         $user['dn'] = '*';
         $user['isadmin'] = 0;
         $user['password'] = generate_random_string(12);
         $user['group'] = '';
         $user['folder'] = '';


         $this->model_user_user->add_user($user);

         $this->model_domain_domain->addDomain($user['domain'], $user['domain']);
      }

      $session->set("username", $user['username']);
      $session->set("uid", $user['uid']);
      $session->set("admin_user", 0);
      $session->set("email", $user['username']);
      $session->set("domain", $user['domain']);
      $session->set("realname", $user['realname']);

      $session->set("emails", $this->model_user_user->get_users_all_email_addresses($user['uid']));
      $session->set("folders", $this->model_folder_folder->get_folder_id_array_for_user($user['uid']));

      AUDIT(ACTION_LOGIN, $user['username'], '', '', 'successful auth against Google');

   }


   public function update_tokens($email = '', $id = '', $token = array()) {
      if($email == '' || $id == '') { return 0; }

      $query = $this->db->query("SELECT email FROM " . TABLE_GOOGLE . " WHERE email=?", array($email));

      if($query->num_rows > 0) {
         $query = $this->db->query("UPDATE " . TABLE_GOOGLE . " SET id=?, access_token=?, refresh_token=?, created=? WHERE email=?", array($id, $token->{'access_token'}, $token->{'refresh_token'}, $token->{'created'}, $email));
      }
      else {
         $query = $this->db->query("INSERT INTO " . TABLE_GOOGLE . " (id, email, access_token, refresh_token, created) VALUES(?,?,?,?,?)", array($id, $email, $token->{'access_token'}, $token->{'refresh_token'}, $token->{'created'}));
      }

      return $this->db->countAffected();
   }


   public function refresh_access_token($email = '') {
      if($email == '') { return ''; }

      $query = $this->db->query("SELECT refresh_token FROM " . TABLE_GOOGLE . " WHERE email=?", array($email));

      if(!isset($query->row['refresh_token'])) { return ''; }

      $client = new apiClient();
      $client->setApplicationName(GOOGLE_APPLICATION_NAME);

      $client->setClientId(GOOGLE_CLIENT_ID);
      $client->setClientSecret(GOOGLE_CLIENT_SECRET);
      $client->setRedirectUri(GOOGLE_REDIRECT_URL);
      $client->setDeveloperKey(GOOGLE_DEVELOPER_KEY);

      $client->refreshToken($query->row['refresh_token']);
      $s = $client->getAccessToken();
      $a = json_decode($s);

      if(isset($a->{'access_token'})) { return $a->{'access_token'}; }

      return '';
   }


}

?>
