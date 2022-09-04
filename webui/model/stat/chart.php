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


   public function pieChartHamSpam($emails = '', $timespan, $title, $output) {
      $ham = $spam = 0;

      $range = $this->getRangeInSeconds($timespan);

      $chart = new PieChart(SIZE_X, SIZE_Y);

      $query = $this->db->query("SELECT COUNT(*) AS SPAM FROM " . TABLE_META . " WHERE $emails AND arrived > $range");
      if($query->num_rows > 0) { $spam = $query->row['SPAM']; }

      $query = $this->db->query("SELECT COUNT(*) AS HAM FROM " . TABLE_META . " WHERE $emails AND arrived > $range");
      if($query->num_rows > 0) { $ham = $query->row['HAM']; }

      if($ham > $spam) {
         $chart->getPlot()->getPalette()->setPieColor(array(new Color(26, 192, 144), new Color(208, 48, 128) ));
      } else {
         $chart->getPlot()->getPalette()->setPieColor(array(new Color(208, 48, 128), new Color(26, 192, 144) ));
      }


      $dataSet = new XYDataSet();

      $dataSet->addPoint(new Point("HAM ($ham)", $ham));
      $dataSet->addPoint(new Point("SPAM ($spam)", $spam));

      $chart->setDataSet($dataSet);
      $chart->setTitle($title);

      @$this->sendOutput($chart, $output);
   }


   private function getRangeInSeconds($timespan) {
      $range = 0;

      if($timespan == "daily") { return time() - 86400; }
      if($timespan == "weekly") { return time() - 604800; }

      return time() - 2592000;
   }


   private function getDataPoints($timespan) {

      if($timespan == "daily") { return 24; }
      if($timespan == "weekly") { return 7; }

      return 30;
   }


   private function sendOutput($chart, $output = '') {
      if($output == "") {
         header("Content-type: image/png");
         header("Expires: now");
      }

      if($output) {
         $chart->render($output);
      }
      else {
         $chart->render();
      }
   }

}

?>
