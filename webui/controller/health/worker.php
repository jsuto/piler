<?php


class ControllerHealthWorker extends Controller {
   private $error = [];

   public function index(){

      $this->id = "content";
      $this->template = "health/worker.tpl";
      $this->layout = "common/layout-empty";

      $db_history = Registry::get('db_history');

      $this->load->model('health/health');
      $this->load->model('stat/counter');
      if(ENABLE_SAAS == 1) {
         $this->load->model('saas/customer');
      }

      $request = Registry::get('request');
      $lang = Registry::get('language');

      $db = Registry::get('db');
      $db->select_db($db);


      if(Registry::get('admin_user') != 1 && Registry::get('readonly_admin') != 1) {
         die("go away");
      }

      $this->model_health_health->collect_data();
      $this->data['health'] = $this->model_health_health->data;

      $this->render();
   }


}
