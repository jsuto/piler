<?php

class ModelStatChart extends Model {

   public function lineChartArchivedMessages($timespan){
      $data = [];

      $session = Registry::get('session');

      $range = $this->getRangeInSeconds($timespan);

      if($timespan == "daily"){ $grouping = "GROUP BY FROM_UNIXTIME(ts, '%Y.%m.%d. %H')"; }
      else { $grouping = "GROUP BY FROM_UNIXTIME(ts, '%Y.%m.%d.')"; }


      if($timespan == "daily"){
         $delta = 3600;
         $data_points = 24;
      } else {
         $delta = 86400;
         $data_points = 30;
      }

      $now = time();
      $now -= $now % $delta + ($data_points-1)*$delta;

      for($i=0; $i<$data_points; $i++) {
         $data[$now] = 0;
         $now += $delta;
      }

      if(Registry::get('admin_user') == 0) {

         $q = '';
         $auditdomains = $session->get('auditdomains');

         foreach($auditdomains as $a) {
            if($q) { $q .= ",?"; } else { $q = "?"; }
         }
         reset($auditdomains);
         $query = $this->db->query("select (arrived-(arrived%$delta)) as ts, count(*) as num from " . VIEW_MESSAGES . " where arrived > $range AND todomain IN ($q) $domains $grouping ORDER BY ts DESC limit $data_points", $auditdomains);
      } else {
         $query = $this->db->query("select (arrived-(arrived%$delta)) as ts, count(*) as num from " . TABLE_META . " where arrived > $range $grouping ORDER BY ts DESC limit $data_points");
      }

      foreach ($query->rows as $q) {
         if(isset($data[$q['ts']])) {
            $data[$q['ts']] = $q['num'];
         }
      }

      return $data;
   }


   private function getRangeInSeconds($timespan) {
      $range = 0;

      if($timespan == "daily") { return time() - 86400; }
      if($timespan == "weekly") { return time() - 604800; }

      return time() - 2592000;
   }


}
