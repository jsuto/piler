<?php


class ControllerMessageAttachment extends Controller {

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
         AUDIT(ACTION_UNKNOWN, '', '', $this->data['id'], 'unknown id: ' . $this->data['id']);
         die("invalid id: " . $this->data['id']);
      }

      $this->data['attachment'] = $this->model_search_message->get_attachment_by_id($this->data['id']);

      if(!isset($this->data['attachment']['filename'])) {
         die("invalid filename");
      }


      AUDIT(ACTION_DOWNLOAD_ATTACHMENT, '', '', $this->data['id'], '');

      header("Cache-Control: public, must-revalidate");
      header("Pragma: no-cache");
      header("Content-Type: application/octet-stream");
      header("Content-Disposition: attachment; filename=\"" . $this->data['attachment']['filename'] . "\"");
      header("Content-Transfer-Encoding: binary\n");

      print $this->data['attachment']['attachment'];


      exit;
   }


}

?>
