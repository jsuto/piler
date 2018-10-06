<?php


class ControllerMessagePDF extends Controller {

   public function index(){

      $this->id = "content";
      $this->template = "message/headers.tpl";
      $this->layout = "common/layout-empty";

      $request = Registry::get('request');
      $db = Registry::get('db');
      $session = Registry::get('session');

      $this->load->model('search/search');
      $this->load->model('search/message');
      $this->load->model('message/attachment');
      $this->load->model('audit/audit');

      $this->document->title = $this->data['text_message'];

      $this->data['id'] = @$this->request->get['id'];

      $this->data['search'] = "";

      // FIXME!!!
      $message = $this->model_search_message->get_message_array($this->data['id'], $this->data['search']);

      $images = $this->model_message_attachment->write_image_attachments_to_tmp($message['attachments'], $this->data['id']);


      $tmpname = $message['piler_id'] . "-tmp-" . microtime(true) . ".html";

      $fp = fopen(DIR_BASE . 'tmp/' . $tmpname, "w+");
      if($fp) {
         fwrite($fp, "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" /></head><body>");
         fwrite($fp, $message['message']['message']);

         foreach($images as $img) {
            fwrite($fp, "<p><img src=" . SITE_URL . "/tmp/" . $img['name'] . " alt=\"\" /></p>\n");
         }

         fwrite($fp, "</body></html>");
         fclose($fp);
      }

      AUDIT(ACTION_DOWNLOAD_MESSAGE, '', '', $this->data['id'], '');

      header("Cache-Control: public, must-revalidate");
      header("Pragma: no-cache");
      header("Content-Type: application/pdf");
      header("Content-Disposition: attachment; filename=" . $message['piler_id'] . ".pdf");
      header("Content-Transfer-Encoding: binary\n");

      print(system(WKHTMLTOPDF_COMMAND . " " . SITE_URL . "tmp/$tmpname -"));

      unlink(DIR_BASE . 'tmp/' . $tmpname);
   }


}
