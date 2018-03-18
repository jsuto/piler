<?php

class ModelHealthHealth extends Model {

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
         $t2 = date(DATE_TEMPLATE . " H:i", $st['mtime']+30*60);

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
