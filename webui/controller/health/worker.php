<?php


class ControllerHealthWorker extends Controller {
   private $error = array();

   public function index(){

      $archivesizeraw = $sqlsizeraw = $sphinxsizeraw = 0;
      $averagemessagesweekraw = $averagemessagesmonthraw = $averagemessagesizeraw = $averagesizedayraw = $averagesqlsizeraw = $averagesphinxsizeraw = 0;
   
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

      ini_set("default_socket_timeout", 5);


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


      list($archivesizeraw, $this->data['counters']) = $this->model_stat_counter->get_counters();
	  
      $this->data['archive_size'] = nice_size($archivesizeraw, ' ');

      $this->data['prefix'] = '';
      if(isset($this->data['counters'][MEMCACHED_PREFIX . 'rcvd'])) { $this->data['prefix'] = MEMCACHED_PREFIX; }

      $this->data['sysinfo'] = $this->model_health_health->sysinfo();

      $this->data['options'] = $this->model_health_health->get_options();
	  
      $sqlsizeraw = $this->model_health_health->get_database_size();
	  
      $sphinxsizeraw = $this->model_health_health->get_sphinx_size();
	  
      /* these next counters are for projecting space */
      $averagemessagesweekraw = ($this->data['processed_emails'][1]) / 7;
      $averagemessagesmonthraw = ($this->data['processed_emails'][2]) / 30;
      $averagemessagesizeraw = $archivesizeraw / $this->data['counters']['rcvd'];
      $averagesqlsizeraw = $sqlsizeraw / $this->data['counters']['rcvd'];
      $averagesphinxsizeraw = $sphinxsizeraw / $this->data['counters']['rcvd'];
      $averagesizedayraw = ($averagemessagesizeraw+$averagesqlsizeraw+$averagesphinxsizeraw) * $averagemessagesweekraw;

      $datapart = 0;
      foreach($this->data['shortdiskinfo'] as $part) {
         if( $part['partition'] == DATA_PARTITION ) { $datapart = $part['freespace']*1024; }
      }
	  
      $this->data['averagemessages'] = round($averagemessagesweekraw);							// average of messages over the past week
      $this->data['averagemessagesize'] = nice_size($averagemessagesizeraw,' ');				// average message size on disk
      $this->data['averagesqlsize'] = nice_size($averagesqlsizeraw,' ');						// average metadata size in sql
      $this->data['averagesphinxsize'] = nice_size($averagesphinxsizeraw,' ');					// average sphinx index
      $this->data['averagesizeday'] = nice_size($averagesizedayraw,' ');						// average size per day
      $this->data['daysleftatcurrentrate'] = convert_days_ymd($datapart / $averagesizedayraw);	// number of days of free space left
      if ( $averagemessagesweekraw > $averagemessagesmonthraw ) {
         $this->data['usagetrend'] = 1;
      } elseif( $averagemessagesweekraw < $averagemessagesmonthraw ) {
         $this->data['usagetrend'] = -1;
      } else {
         $this->data['usagetrend'] = 0;
      }
	  
      $this->render();
   }


}

?>
