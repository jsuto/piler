<?php

class ModelHealthHealth extends Model {
   public $data = [];

   public function collect_data() {
      ini_set("default_socket_timeout", 5);

      list ($this->data['uptime'], $this->data['cpuload']) = $this->uptime();

      exec(PILER_BINARY . " -v", $a);
      $this->data['piler_version'] = $a[0];

      $x = exec(CPU_USAGE_COMMAND);
      $this->data['cpuinfo'] = 100 - (int)$x;

      list($this->data['totalmem'], $this->data['meminfo'], $this->data['totalswap'], $this->data['swapinfo']) = $this->meminfo();
      $this->data['shortdiskinfo'] = $this->diskinfo();

      list($archivesizeraw, $archivestoredsizeraw, $this->data['counters']) = $this->model_stat_counter->get_counters();

      $this->data['archive_size'] = nice_size($archivesizeraw, ' ');
      $this->data['archive_stored_size'] = nice_size($archivestoredsizeraw, ' ');

      $this->data['prefix'] = '';
      if(isset($this->data['counters'][MEMCACHED_PREFIX . 'rcvd'])) { $this->data['prefix'] = MEMCACHED_PREFIX; }

      $this->data['sysinfo'] = $this->model_health_health->sysinfo();
      $this->data['options'] = $this->model_health_health->get_options();

      $this->data['indexer_stat'] = $this->indexer_stat();
      $this->data['purge_stat'] = $this->purge_stat();

      $this->data['sphinx_current_main_size'] = $this->get_current_sphinx_main_index_size();

      $this->get_average_count_values();
      $this->get_average_size_values($archivesizeraw);

      if(ENABLE_SAAS == 1) {
         $this->data['num_of_online_users'] = $this->model_saas_customer->count_online();
      }

      $this->data['health'] = [];

      $lang = Registry::get('language');

      foreach (Registry::get('health_smtp_servers') as $smtp) {
         if($smtp[0]) {
            $this->data['health'][] = $this->checksmtp($smtp, $lang->data['text_error']);
         }
      }

   }


   public function get_average_count_values() {
      $this->data['processed_emails'] = $this->count_processed_emails();

      // average messages per day, computed over the past week
      $this->data['averagemessagesweekraw'] = ($this->data['processed_emails']['last_7_days_count']) / 7;

      // average messages per day, computed over the past month
      $this->data['averagemessagesmonthraw'] = ($this->data['processed_emails']['last_30_days_count']) / 30;

      //average messages per day, computed over the time period since the first email was archived
      $total_number_days = round( (time() - $this->get_first_email_arrival_ts()) / 86400 );
      if($total_number_days == 0) {
         $total_number_days = 1;
      }

      $this->data['averagemessagestotalraw'] = $this->data['counters']['rcvd'] / $total_number_days;
   }


   public function get_average_size_values($archivesizeraw = 0) {
      $averagemessagesizeraw = $averagesqlsizeraw = $averagesphinxsizeraw = 0;

      if($this->data['counters']['rcvd'] > 0) {
         // average message size, computed for total messages in database
         $averagemessagesizeraw = $archivesizeraw / $this->data['counters']['rcvd'];

         //average message metadata size, computed for total messages in database
         $averagesqlsizeraw = $this->get_database_size() / $this->data['counters']['rcvd'];

         //average message sphinx index size, computed for total messages in database
         $averagesphinxsizeraw = $this->get_sphinx_size() / $this->data['counters']['rcvd'];
      }

      // average total message size per day, computed over the time period since the first email was archived
      $averagesizedayraw = ($averagemessagesizeraw + $averagesqlsizeraw + $averagesphinxsizeraw) * $this->data['averagemessagestotalraw'];

      $datapart = 0;
      foreach($this->data['shortdiskinfo'] as $part) {
         if($part['partition'] == DATA_PARTITION) {
            $datapart = $part['freespace']*1024;
         }
      }

      $this->data['oldestmessagets'] = $this->get_oldest_record_ts();                // date of the oldest record in the db
      $this->data['averagemessages'] = round($this->data['averagemessagesweekraw']);               // rounded average of messages over the past week
      $this->data['averagemessagesize'] = nice_size($averagemessagesizeraw, ' ');    // formatted average message size on disk
      $this->data['averagesqlsize'] = nice_size($averagesqlsizeraw, ' ');            // formatted average metadata size in sql
      $this->data['averagesphinxsize'] = nice_size($averagesphinxsizeraw, ' ');      // formatted average sphinx index
      $this->data['averagesizeday'] = nice_size($averagesizedayraw, ' ');            // formatted average size per day

      // estimated number of days of free space left
      $averagesizedayraw > 0 ? $this->data['daysleftatcurrentrate'] = convert_days_ymd($datapart / $averagesizedayraw) : $this->data['daysleftatcurrentrate'] = 0;


      /*
       * determine if the trend of the last week compared to the last month is
       * increasing, decreasing, or neutral (only applies to message count, not size)
       */

      if ($this->data['averagemessagesweekraw'] > $this->data['averagemessagesmonthraw']) {
         $this->data['usagetrend'] = 1;
      } elseif($this->data['averagemessagesweekraw'] < $this->data['averagemessagesmonthraw']) {
         $this->data['usagetrend'] = -1;
      } else {
         $this->data['usagetrend'] = 0;
      }

   }


   public function format_time($time = 0) {
      if($time >= 1) {
         return sprintf("%.2f", $time) . " sec";
      }
      else {
         return sprintf("%.2f", 1000*$time) . " ms";
      }
   }


   public function checksmtp($smtp = array(), $error = '') {

      $ret = $error;
      $time = 0;
      $time_start = microtime(true);

      if($smtp[0] && $smtp[1] && is_numeric($smtp[1]) && $smtp[1] > 0 && $smtp[1] < 65536) {

         $s = @fsockopen($smtp[0], $smtp[1]);

         if($s) {
            $ret = trim(fgets($s, 4096));
            fputs($s, "QUIT\r\n");
            fclose($s);
         }
      }

      $time = microtime(true) - $time_start;
      return array($smtp[0] . ":" . $smtp[1], $ret, $this->format_time($time), $smtp[2]);
   }


   public function count_processed_emails() {
      $today = $last_7_days = $last_30_days = 0;
      $a = array();
      $now = time();

      $ts = $now - 3600;
      $query = $this->db->query("select count(*) as count from " . TABLE_META . " where arrived > $ts");
      if(isset($query->row['count'])) {
         $a['last_60_mins_count'] = $query->row['count'];
      }

      $ts = $now - 86400;
      $query = $this->db->query("select count(*) as count from " . TABLE_META . " where arrived > $ts");
      if(isset($query->row['count'])) {
         $a['today_count'] = $query->row['count'];
      }

      $ts = $now - 604800;
      $query = $this->db->query("select count(*) as count from " . TABLE_META . " where arrived > $ts");
      if(isset($query->row['count'])) {
         $a['last_7_days_count'] = $query->row['count'];
      }

      $ts = $now - 2592000;
      $query = $this->db->query("select count(*) as count from " . TABLE_META . " where arrived > $ts");
      if(isset($query->row['count'])) {
         $a['last_30_days_count'] = $query->row['count'];
      }

      return $a;
   }


   public function uptime() {
      $s = exec("uptime");
      list ($uptime, $loadavg) = preg_split("/ load average\: /", $s);

      return array(preg_replace("/\,\ {0,}$/", "", $uptime), $loadavg);
   }


   public function meminfo() {
      $m = explode("\n", file_get_contents("/proc/meminfo"));

      while(list($k, $v) = each($m)) {
         $a = preg_split("/\ {1,}/", $v);
         if(isset($a[0]) && $a[0]) { $_m[$a[0]] = $a[1]; }
      }

      $mem_percentage = isset($_m['MemTotal:']) && $_m['MemTotal:'] > 0 ? sprintf("%.2f", 100*($_m['MemTotal:'] - $_m['MemFree:'] - $_m['Cached:']) / $_m['MemTotal:']) : "0";
      $swap_percentage = isset($_m['SwapTotal:']) && $_m['SwapTotal:'] > 0 ? sprintf("%.2f", 100*($_m['SwapTotal:'] - $_m['SwapFree:']) / $_m['SwapTotal:']) : "0";

      return array(sprintf("%.0f", @$_m['MemTotal:'] / 1000), $mem_percentage, sprintf("%.0f", @$_m['SwapTotal:'] / 1000), $swap_percentage);
   }


   public function diskinfo() {
      $shortinfo = array();
      $a = array();

      $s = exec("df", $output);

      $partitions = Registry::get('partitions_to_monitor');

      while(list($k, $v) = each($output)) {
         if($k > 0) {
            $p = preg_split("/\ {1,}/", $v);
            if(isset($p[5]) && in_array($p[5], $partitions) && !isset($a[$p[5]])) {
               $a[$p[5]] = 1;

               $shortinfo[] = array(
                                    'partition' => $p[5],
                                    'freespace' => $p[3],
                                    'total' => $p[1],
                                    'used' => $p[2],
                                    'utilization' => preg_replace("/\%/", "", $p[4])
                              );
            }
         }
      }

      return $shortinfo;
   }


   public function sysinfo() {
      $hostname = exec("hostname -f");
      $s = exec("uname -a");

      return array($hostname, $s);
   }


   public function get_options() {
      $data = array();

      $query = $this->db->query("SELECT * FROM `" . TABLE_OPTION . "`");
      if(isset($query->rows)) {
         foreach ($query->rows as $q) {
            $data[$q['key']] = $q['value'];
         }
      }

      return $data;
   }


   public function toggle_option($option = '') {
      $value = 0;

      $query = $this->db->query("SELECT `value` FROM `" . TABLE_OPTION . "` WHERE `key`=?", array($option));

      if(isset($query->row['value'])) {
         if($query->row['value'] == 0) { $value = 1; }
         else { $value = 0; }

         $query = $this->db->query("UPDATE `" . TABLE_OPTION . "` SET `value`=? WHERE `key`=?", array($value, $option));

      }

   }


   public function get_database_size() {
      $data = array();

      $query = $this->db->query("SELECT table_schema AS `name`, 
								SUM( data_length + index_length ) AS `size` 
								FROM information_schema.TABLES
								WHERE table_schema = '".DB_DATABASE."'
								GROUP BY table_schema;");
      if(isset($query->rows)) {
         $data = array_pop($query->rows);
      } else {
         $data['size'] = 0;
      }

      return $data['size'];
   }


   public function get_oldest_record_ts() {
      $data = array();

      $query = $this->db->query("SELECT `sent` AS `oldest_record_ts` FROM " . TABLE_META . " WHERE `deleted`=0 and `sent` > 837381600 ORDER BY `sent` ASC LIMIT 1");

      if(isset($query->rows)) {
         $data = array_pop($query->rows);
      } else {
         $data['oldest_record_ts'] = 0;
      }

      return $data['oldest_record_ts'];
   }


   public function get_first_email_arrival_ts() {
      $query = $this->db->query("SELECT `arrived`  FROM " . TABLE_META . " ORDER BY id ASC LIMIT 1");

      if(isset($query->row['arrived'])) { return $query->row['arrived']; }

      return time();
   }


   public function get_sphinx_size($directory = DIR_SPHINX) {
      $dirSize=0;

      if(!$dh=opendir($directory)) {
         return false;
      }

      while($file = readdir($dh)) {
         if($file == "." || $file == "..") {
            continue;
         }

         if(is_file($directory."/".$file)) {
            $dirSize += filesize($directory."/".$file);
         }
         if(is_dir($directory."/".$file)) {
            $dirSize += $this->get_sphinx_size($directory."/".$file);
         }
      }

      closedir($dh);
      return $dirSize;
   }


   public function indexer_stat() {
      $data = array('', '');

      if(file_exists(INDEXER_BEACON)) {

         $st = stat(INDEXER_BEACON);
         $t1 = date(DATE_TEMPLATE . " H:i", $st['mtime']);
         $t2 = date(DATE_TEMPLATE . " H:i", $st['mtime']+DELTA_INDEXER_PERIOD);

         $data = array($t1, $t2);
      }

      return $data;
   }


   public function purge_stat() {
      $data = array('', '');

      if(file_exists(PURGE_BEACON)) {

         $st = stat(PURGE_BEACON);
         $t1 = date(DATE_TEMPLATE . " H:i", $st['mtime']);
         $t2 = date(DATE_TEMPLATE . " H:i", $st['mtime']+86400);

         $data = array($t1, $t2);
      }

      return $data;
   }


   public function get_current_sphinx_main_index_size() {
      $size = 0;

      if(file_exists(SPHINX_MAIN_INDEX_SIZE)) {
         $size = (int) file_get_contents(SPHINX_MAIN_INDEX_SIZE);
      }

      return $size;
   }

}
