<?php

class ModelSaasCustomer extends Model
{

   public function get_customer_settings_by_email() {
       $data = array(
                      'branding_text' => BRANDING_TEXT,
                      'branding_url' => BRANDING_URL,
                      'branding_logo' => BRANDING_LOGO,
                      'support_link' => SUPPORT_LINK,
                      'colour' => ''
                    );


      if(!isset($_SESSION['email']) || !strchr($_SESSION['email'], '@') ) { return $data; }

      list ($user, $domain) = explode("@", $_SESSION['email']);

      $query = $this->db->query("SELECT * FROM " . TABLE_CUSTOMER_SETTINGS . " WHERE domain=(SELECT mapped FROM " . TABLE_DOMAIN . " WHERE domain=?)", array($domain));

      if($query->num_rows > 0) {
         if($query->row['branding_text']) { $data['branding_text'] = $query->row['branding_text']; }
         if($query->row['branding_url']) { $data['branding_url'] = $query->row['branding_url']; }
         if($query->row['branding_logo']) { $data['branding_logo'] = $query->row['branding_logo']; }
         if($query->row['support_link']) { $data['support_link'] = $query->row['support_link']; }
         if($query->row['colour']) { $data['colour'] = $query->row['colour']; }
      }

      return $data;
   }

}

?>
