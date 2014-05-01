<?php


class ControllerHealthWorker extends Controller {
   private $error = array();

   public function index(){

      $archivesizeraw = $sqlsizeraw = $sphinxsizeraw = 0;
      $averagemessagesweekraw = $averagemessagesmonthraw = $averagemessagesizeraw = $averagesizedayraw = $averagesqlsizeraw = $averagesphinxsizeraw = 0;
      $averagemessagestotalraw = 0;
 
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


      $this->data['health'] = array();

      if(Registry::get('admin_user') != 1 && Registry::get('readonly_admin') != 1) {
         die("go away");
      }

      ini_set("default_socket_timeout", 5);


      foreach (Registry::get('health_smtp_servers') as $smtp) {
         if($smtp[0]) {
            $this->data['health'][] = $this->model_health_health->checksmtp($smtp, $lang->data['text_error']);
         }
      }

      if(ENABLE_SAAS == 1) {
         $this->data['num_of_online_users'] = $this->model_saas_customer->count_online();
      }

      $this->data['processed_emails'] = $this->model_health_health->count_processed_emails();

      list ($this->data['uptime'], $this->data['cpuload']) = $this->model_health_health->uptime();

      $this->data['cpuinfo'] = 100 - (int)file_get_contents(CPUSTAT);

      $this->data['quarantinereportinfo'] = @file_get_contents(DAILY_QUARANTINE_REPORT_STAT);

      list($this->data['totalmem'], $this->data['meminfo'], $this->data['totalswap'], $this->data['swapinfo']) = $this->model_health_health->meminfo();
      $this->data['shortdiskinfo'] = $this->model_health_health->diskinfo();


      /* counter related stuff */

      $db = Registry::get('db');
      $db->select_db($db->database);

      list($archivesizeraw, $this->data['counters']) = $this->model_stat_counter->get_counters();
 
      $oldest_record_timestamp = $this->model_health_health->get_oldest_record_ts();
      $total_number_days = round( (time() - $this->model_health_health->get_first_email_arrival_ts()) / 86400 );

      $this->data['archive_size'] = nice_size($archivesizeraw, ' ');

      $this->data['prefix'] = '';
      if(isset($this->data['counters'][MEMCACHED_PREFIX . 'rcvd'])) { $this->data['prefix'] = MEMCACHED_PREFIX; }

      $this->data['sysinfo'] = $this->model_health_health->sysinfo();

      $this->data['options'] = $this->model_health_health->get_options();
  
      $sqlsizeraw = $this->model_health_health->get_database_size();
  
      $sphinxsizeraw = $this->model_health_health->get_sphinx_size();


      /*
       * message count variables
       */

      //average messages per day, computed over the past week
      $averagemessagesweekraw = ($this->data['processed_emails'][2]) / 7;

      //average messages per day, computed over the past month
      $averagemessagesmonthraw = ($this->data['processed_emails'][3]) / 30;

      //average messages per day, computed over the time period since the first email was archived
      if($total_number_days > 0) { $averagemessagestotalraw = $this->data['counters']['rcvd'] / $total_number_days; }


      /*
       * message size variables
       */

      if($this->data['counters']['rcvd'] > 0) {

         //average message size, computed for total messages in database
         $averagemessagesizeraw = $archivesizeraw / $this->data['counters']['rcvd'];

         //average message metadata size, computed for total messages in database
         $averagesqlsizeraw = $sqlsizeraw / $this->data['counters']['rcvd'];

         //average message sphinx index size, computed for total messages in database
         $averagesphinxsizeraw = $sphinxsizeraw / $this->data['counters']['rcvd'];
      }

      //average total message size per day, computed over the time period since the first email was archived
      $averagesizedayraw = ($averagemessagesizeraw + $averagesqlsizeraw + $averagesphinxsizeraw) * $averagemessagestotalraw;



      $datapart = 0;
      foreach($this->data['shortdiskinfo'] as $part) {
         if( $part['partition'] == DATA_PARTITION ) { $datapart = $part['freespace']*1024; }    // if the partition is the selected storage partition, record freespace on that partition
      }
  
      $this->data['oldestmessagets'] = $oldest_record_timestamp;							    // date of the oldest record in the db
      $this->data['averagemessages'] = round($averagemessagesweekraw);							// rounded average of messages over the past week
      $this->data['averagemessagesize'] = nice_size($averagemessagesizeraw,' ');    			// formatted average message size on disk
      $this->data['averagesqlsize'] = nice_size($averagesqlsizeraw,' ');						// formatted average metadata size in sql
      $this->data['averagesphinxsize'] = nice_size($averagesphinxsizeraw,' ');					// formatted average sphinx index
      $this->data['averagesizeday'] = nice_size($averagesizedayraw,' ');						// formatted average size per day

      // estimated number of days of free space left
      $averagesizedayraw > 0 ? $this->data['daysleftatcurrentrate'] = convert_days_ymd($datapart / $averagesizedayraw) : $this->data['daysleftatcurrentrate'] = 0;


      /*
       * determine if the trend of the last week compared to the last month is
       * increasing, decreasing, or neutral
       * (only applies to message count, not size)
       */

      if ( $averagemessagesweekraw > $averagemessagesmonthraw ) {
         $this->data['usagetrend'] = 1;
      } elseif( $averagemessagesweekraw < $averagemessagesmonthraw ) {
         $this->data['usagetrend'] = -1;
      } else {
         $this->data['usagetrend'] = 0;
      }


      $this->data['indexer_stat'] = $this->model_health_health->indexer_stat();
      $this->data['purge_stat'] = $this->model_health_health->purge_stat();


      $this->render();
   }


}

?>
