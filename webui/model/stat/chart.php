<?php

class ModelStatChart extends Model {

   public function lineChartHamSpam($timespan, $title, $size_x, $size_y, $output){
      $ydata = array();
      $ydata2 = array();
      $dates = array();

      $chart = new LineChart($size_x, $size_y);

      $chart->getPlot()->getPalette()->setLineColor(array(
         //new Color(26, 192, 144),
         new Color(208, 48, 128),
      ));

      $line1 = new XYDataSet();

      $limit = $this->getDataPoints($timespan);

      $range = $this->getRangeInSeconds($timespan);

      if($timespan == "daily"){ $grouping = "GROUP BY FROM_UNIXTIME(ts, '%Y.%m.%d. %H')"; }
      else { $grouping = "GROUP BY FROM_UNIXTIME(ts, '%Y.%m.%d.')"; }


      if($timespan == "daily"){
         $delta = 3600;
         $date_format = "H:i";
      } else {
         $delta = 86400;
         $date_format = "m.d.";
      }

      if(Registry::get('admin_user') == 0) {

         $q = '';
         foreach($_SESSION['auditdomains'] as $a) {
            if($q) { $q .= ",?"; } else { $q = "?"; }
         }
         reset($_SESSION['auditdomains']);
         $query = $this->db->query("select arrived-(arrived%$delta) as ts, count(*) as num from " . VIEW_MESSAGES . " where arrived > $range AND todomain IN ($q) $domains $grouping ORDER BY ts DESC limit $limit", $_SESSION['auditdomains']);
      } else {
         $query = $this->db->query("select arrived-(arrived%$delta) as ts, count(*) as num from " . TABLE_META . " where arrived > $range $grouping ORDER BY ts DESC limit $limit");
      }


      foreach ($query->rows as $q) {
         array_push($ydata, $q['num']);
         array_push($dates, date($date_format, $q['ts']));
      }

      if($query->num_rows >= 15) {
         $i = 0;
         while(list($k, $v) = each($dates)) {
            $i++;
            if($i % 3) { $dates[$k] = ""; }
         }
         reset($dates);
      }



      $ydata = array_reverse($ydata);
      $dates = array_reverse($dates);

      for($i=0; $i<count($ydata); $i++){
         $ts = $dates[$i];
         $line1->addPoint(new Point("$ts", $ydata[$i]));
      }


      $dataSet = new XYSeriesDataSet();
      $dataSet->addSerie("RCVD", $line1);

      $chart->setDataSet($dataSet);

      $chart->setTitle($title);
      $chart->getPlot()->setGraphCaptionRatio(0.80);

      $this->sendOutput($chart, $output);
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

      $this->sendOutput($chart, $output);
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
