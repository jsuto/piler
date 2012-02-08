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


      /*$this->data['queues'][] = format_qshape($lang->data['text_active_incoming_queue'], QSHAPE_ACTIVE_INCOMING);
      $this->data['queues'][] = format_qshape($lang->data['text_deferred_queue'], QSHAPE_DEFERRED);*/

      /*if(file_exists(QSHAPE_ACTIVE_INCOMING_OUT)) {
         $this->data['queues_out'][] = format_qshape($lang->data['text_active_incoming_queue'], QSHAPE_ACTIVE_INCOMING_OUT);
         $this->data['queues_out'][] = format_qshape($lang->data['text_deferred_queue'], QSHAPE_DEFERRED_OUT);
      }*/

      $this->data['processed_emails'] = $this->model_health_health->count_processed_emails();

      list ($this->data['uptime'], $this->data['cpuload']) = $this->model_health_health->uptime();

      $this->data['cpuinfo'] = 100 - (int)file_get_contents(CPUSTAT);
      $this->data['archive_size'] = (int)file_get_contents(ARCHIVE_SIZE);

      $this->data['quarantinereportinfo'] = @file_get_contents(DAILY_QUARANTINE_REPORT_STAT);

      list($this->data['totalmem'], $this->data['meminfo'], $this->data['totalswap'], $this->data['swapinfo']) = $this->model_health_health->meminfo();
      $this->data['shortdiskinfo'] = $this->model_health_health->diskinfo();

      /*if(file_exists(MAILLOG_PID_FILE)) {
         $this->data['maillog_status'] = $lang->data['text_running'];
      } else {
         $this->data['maillog_status'] = $lang->data['text_not_running'];
      }*/


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
            $this->model_stat_counter->resetCounters();
            header("Location: index.php?route=health/health");
            exit;
         }
         else {
            $this->template = "health/counter-reset-confirm.tpl";
         }
      }


      $this->data['counters'] = $this->model_stat_counter->getCounters();
      $this->data['prefix'] = '';
      if(isset($this->data['counters']['_c:rcvd'])) { $this->data['prefix'] = '_c:'; }

      $this->data['sysinfo'] = $this->model_health_health->sysinfo();

      $this->render();
   }


}

?>
