<?php


class ControllerPolicyApply extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "policy/apply.tpl";
      $this->layout = "common/layout-empty";


      $request = Registry::get('request');

      $db = Registry::get('db');
      $lang = Registry::get('language');

      if(Registry::get('admin_user') == 0) {
         die("go away");
      }

      system('sudo -n /etc/init.d/rc.piler reload', $val);

      if($val === 0) {
         print $lang->data['text_applied'];
      } else {
         print $lang->data['text_install_sudo_apply'];
      }


   }

}

?>
