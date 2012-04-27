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

      if($smtp[0] && $smtp[1] && is_numeric($smtp[1]) && $smtp[1] > 0 && $smtp[1] < 65536) {
         $time_start = microtime(true);

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
      $now = time();

      $ts = $now - 86400;
      $query = $this->db->query("select count(*) as count from " . TABLE_META . " where arrived > $ts");
      if(isset($query->row['count'])) { $today = $query->row['count']; }

      $ts = $now - 604800;
      $query = $this->db->query("select count(*) as count from " . TABLE_META . " where arrived > $ts");
      if(isset($query->row['count'])) { $last_7_days = $query->row['count']; }

      $ts = $now - 2592000;
      $query = $this->db->query("select count(*) as count from " . TABLE_META . " where arrived > $ts");
      if(isset($query->row['count'])) { $last_30_days = $query->row['count']; }

      return array($today, $last_7_days, $last_30_days);
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

      $mem_percentage = sprintf("%.2f", 100*($_m['MemTotal:'] - $_m['MemFree:'] - $_m['Cached:']) / $_m['MemTotal:']);
      $swap_percentage = sprintf("%.2f", 100*($_m['SwapTotal:'] - $_m['SwapFree:']) / $_m['SwapTotal:']);

      return array(sprintf("%.0f", $_m['MemTotal:'] / 1000), $mem_percentage, sprintf("%.0f", $_m['SwapTotal:'] / 1000), $swap_percentage);
   }


   public function diskinfo() {
      $shortinfo = array();

      $s = exec("df -h", $output);

      $partitions = Registry::get('partitions_to_monitor');

      while(list($k, $v) = each($output)) {
         if($k > 0) {
            $p = preg_split("/\ {1,}/", $v);
            if(isset($p[5]) && in_array($p[5], $partitions)) {
               $shortinfo[] = array(
                                    'partition' => $p[5],
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


}


?>
