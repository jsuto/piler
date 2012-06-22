<?php


class ControllerHealthWorker extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "health/worker.tpl";
      $this->layout = "common/layout-empty";

      $db_history = Registry::get('db_history');

      $this->load->model('health/health');
      $this->load->model('stat/counter');

      $request = Registry::get('request');

      $lang = Registry::get('language');


      $this->data['health'] = array();

      if(Registry::get('admin_user') != 1 && Registry::get('readonly_admin') != 1) {
         die("go away");
      }

      foreach (Registry::get('health_smtp_servers') as $smtp) {
         $this->data['health'][] = $this->model_health_health->checksmtp($smtp, $lang->data['text_error']);
      }


      $this->data['processed_emails'] = $this->model_health_health->count_processed_emails();

      list ($this->data['uptime'], $this->data['cpuload']) = $this->model_health_health->uptime();

      $this->data['cpuinfo'] = 100 - (int)file_get_contents(CPUSTAT);

      $this->data['quarantinereportinfo'] = @file_get_contents(DAILY_QUARANTINE_REPORT_STAT);

      list($this->data['totalmem'], $this->data['meminfo'], $this->data['totalswap'], $this->data['swapinfo']) = $this->model_health_health->meminfo();
      $this->data['shortdiskinfo'] = $this->model_health_health->diskinfo();


      if(ENABLE_LDAP_IMPORT_FEATURE == 1) {
         $this->data['adsyncinfo'] = @file_get_contents(AD_SYNC_STAT);

         $this->data['total_emails_in_database'] = 0;

         $a = preg_split("/ /", $this->data['adsyncinfo']);
         list ($this->data['totalusers'], $this->data['totalnewusers'], $this->data['totaldeletedusers'], $this->data['total_emails_in_database']) = preg_split("/\//", $a[count($a)-1]);
         $this->data['adsyncinfo'] = $a[0] . " " . $a[1] . " " . $this->data['total_emails_in_database'];
      }


      /* counter related stuff */

      $db = Registry::get('db');
      $db->select_db($db->database);

      if($this->request->server['REQUEST_METHOD'] == 'POST' && isset($this->request->post['resetcounters']) && $this->request->post['resetcounters'] == 1) {
         if(isset($this->request->post['confirmed']) && $this->request->post['confirmed'] == 1 && Registry::get('admin_user') == 1) {
            $this->model_stat_counter->reset_counters();
            header("Location: index.php?route=health/health");
            exit;
         }
         else {
            $this->template = "health/counter-reset-confirm.tpl";
         }
      }


      list($this->data['archive_size'], $this->data['counters']) = $this->model_stat_counter->get_counters();

      $this->data['prefix'] = '';
      if(isset($this->data['counters'][MEMCACHED_PREFIX . 'rcvd'])) { $this->data['prefix'] = MEMCACHED_PREFIX; }

      $this->data['sysinfo'] = $this->model_health_health->sysinfo();

      $this->data['options'] = $this->model_health_health->get_options();

      $this->render();
   }


}

?>
