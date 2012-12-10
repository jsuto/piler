<?php

class ModelUserGoogle extends Model {

   public function check_for_account($google_account = array()) {

      $query = $this->db->query("SELECT " . TABLE_USER . ".username, " . TABLE_USER . ".uid, " . TABLE_USER . ".realname, " . TABLE_USER . ".dn, " . TABLE_USER . ".password, " . TABLE_USER . ".isadmin, " . TABLE_USER . ".domain FROM " . TABLE_USER . ", " . TABLE_EMAIL . " WHERE " . TABLE_EMAIL . ".email=? AND " . TABLE_EMAIL . ".uid=" . TABLE_USER . ".uid", array($google_account['email']));

      if($query->num_rows == 1) {
         $user = $query->row;
      }
      else {
/*
    [id] => 11731982531819289345
    [email] => bela@bacsi.hu
    [verified_email] => 1
    [name] => Bela Bacsi
    [given_name] => Bela
    [family_name] => Bacsi
*/      

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

      }

      $_SESSION['username'] = $user['username'];
      $_SESSION['uid'] = $user['uid'];
      $_SESSION['admin_user'] = 0;
      $_SESSION['email'] = $user['username'];
      $_SESSION['domain'] = $query->row['domain'];
      $_SESSION['realname'] = $query->row['realname'];

      $_SESSION['emails'] = $this->model_user_user->get_users_all_email_addresses($user['uid']);
      $_SESSION['folders'] = $this->model_folder_folder->get_all_folder_ids($user['uid']);
      $_SESSION['extra_folders'] = $this->model_folder_folder->get_all_extra_folder_ids($user['uid']);

      AUDIT(ACTION_LOGIN, $user['username'], '', '', 'successful auth against Google');

   }


   public function update_tokens($email = '', $id = 0, $token = array()) {
      if($email == '') { return 0; }

      $query = $this->db->query("SELECT email FROM " . TABLE_GOOGLE . " WHERE email=?", array($email));

      if($query->num_rows > 0) {
         $query = $this->db->query("UPDATE " . TABLE_GOOGLE . " SET id=?, access_token=?, refresh_token=?, created=? WHERE email=?", array($id, $token->{'access_token'}, $token->{'refresh_token'}, $token->{'created'}, $email));
      }
      else {
         $query = $this->db->query("INSERT INTO " . TABLE_GOOGLE . " (id, email, access_token, refresh_token, created) VALUES(?,?,?,?,?)", array($id, $email, $token->{'access_token'}, $token->{'refresh_token'}, $token->{'created'}));
      }

      return $this->db->countAffected();

/*
    [access_token] => ya29.AHES6ZSavh4CnWXyfAYRNwqqZ3FmZ-bHPZkWIEPlutG6K_E
    [token_type] => Bearer
    [expires_in] => 3600
    [refresh_token] => 1/J7bwdfCfQkjCu3Q51ypdJeGOzOtw6F1uyg1vaAwOZ2Q
    [created] => 1348415267
*/


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
