<?php


class ControllerStatGraph extends Controller {
   private $error = array();

   public function index(){

      $request = Registry::get('request');
      $db = Registry::get('db');
      $db_history = Registry::get('db_history');

      $this->load->model('user/user');


      $this->load->model('stat/chart');

      $this->load->helper('libchart/classes/libchart');

      $this->data['username'] = Registry::get('username');

      $timespan = @$this->request->get['timespan'];

      $db->select_db($db->database);

      $aa = new ModelStatChart();

      $aa->lineChartHamSpam($timespan, $this->data['text_archived_messages'], SIZE_X, SIZE_Y, "");

   }

}

?>
