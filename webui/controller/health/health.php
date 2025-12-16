<?php


class ControllerHealthHealth extends Controller {

   public function index(){

      $this->id = "content";
      $this->template = "health/health.tpl";
      if(EXTERNAL_DASHBOARD_URL) {
         $this->layout = "common/layout-empty";
      }
      else {
         $this->layout = "common/layout";
      }

      $this->load->model('health/health');

      $request = Registry::get('request');
      $language = Registry::get('language');

      $this->document->title = $language->get('text_health_monitor');


      /* check if we are admin */

      if(Registry::get('admin_user') != 1 && Registry::get('readonly_admin') != 1) {
         $this->template = "common/error.tpl";
         $this->data['errorstring'] = $this->data['text_you_are_not_admin'];
      }
      else {
         if(isset($_GET['toggle_enable_purge'])) {
            $this->model_health_health->toggle_option('enable_purge');
            header('Location: ' . SITE_URL . 'index.php?route=health/health');
            exit;
         }

      }


      $this->render();
   }

}

?>
