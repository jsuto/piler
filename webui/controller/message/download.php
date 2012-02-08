<?php


class ControllerMessageDownload extends Controller {

   public function index(){

      $this->id = "content";
      $this->template = "message/headers.tpl";
      $this->layout = "common/layout-empty";

      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('search/search');
      $this->load->model('search/message');

      $this->document->title = $this->data['text_message'];

      $this->data['id'] = @$this->request->get['id'];

      if(!verify_piler_id($this->data['id'])) {
         AUDIT(ACTION_UNKNOWN, '', '', $this->data['id'], 'unknown piler id: ' . $this->data['id']);
         die("invalid id: " . $this->data['id']);
      }

      if(!$this->model_search_search->check_your_permission_by_piler_id($this->data['id'])) {
         AUDIT(ACTION_UNAUTHORIZED_VIEW_MESSAGE, '', '', $this->data['id'], '');
         die("no permission for " . $this->data['id']);
      }


      AUDIT(ACTION_DOWNLOAD_MESSAGE, '', '', $this->data['id'], '');

      header("Cache-Control: public, must-revalidate");
      header("Pragma: no-cache");
      header("Content-Type: application/octet-stream");
      header("Content-Disposition: attachment; filename=" . $this->data['id'] . ".eml");
      header("Content-Transfer-Encoding: binary\n");

      print $this->model_search_message->get_raw_message($this->data['id']);
      exit;

      $this->render();
   }


}

?>
