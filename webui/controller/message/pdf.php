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


      if(!$this->model_search_search->check_your_permission_by_id($this->data['id'])) {
         AUDIT(ACTION_UNAUTHORIZED_VIEW_MESSAGE, '', '', $this->data['id'], '');
         die("no permission for " . $this->data['id']);
      }

      $this->data['piler_id'] = $this->model_search_message->get_piler_id_by_id($this->data['id']);
      $this->data['attachments'] = $this->model_search_message->get_attachment_list($this->data['piler_id']);
      $this->data['message'] = $this->model_search_message->extract_message($this->data['piler_id']);

      $images = $this->model_message_attachment->write_image_attachments_to_tmp($this->data['attachments'], $this->data['id']);


      $tmpname = $this->data['piler_id'] . "-tmp-" . microtime(true) . ".html";

      $fp = fopen(DIR_BASE . 'tmp/' . $tmpname, "w+");
      if($fp) {
         fwrite($fp, "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" /></head><body>");

         fwrite($fp, '<strong>' . $this->data['message']['subject'] . '</strong><br />');
         fwrite($fp, '<strong>' . $this->data['message']['from'] . '</strong><br />');
         fwrite($fp, '<strong>' . $this->data['message']['to'] . '</strong><br />');
         fwrite($fp, '<strong>' . $this->data['message']['date'] . '</strong><br />');

         if(strlen($this->data['message']['cc']) > 6) {
            fwrite($fp, '<strong>' . $this->data['message']['cc'] . '</strong><br />');
         }

         fwrite($fp, '<hr />');

         fwrite($fp, $this->data['message']['message']);

         foreach($images as $img) {
            fwrite($fp, "<p><img src=" . SITE_URL . "tmp/" . $img['name'] . " alt=\"\" /></p>\n");
         }

         fwrite($fp, "</body></html>");
         fclose($fp);
      }

      AUDIT(ACTION_DOWNLOAD_MESSAGE, '', '', $this->data['id'], '');

      header("Cache-Control: public, must-revalidate");
      header("Pragma: no-cache");
      header("Content-Type: application/pdf");
      header("Content-Disposition: attachment; filename=" . $this->data['piler_id'] . ".pdf");
      header("Content-Transfer-Encoding: binary\n");

      print(system(WKHTMLTOPDF_COMMAND . " " . SITE_URL . "tmp/$tmpname -"));

      unlink(DIR_BASE . 'tmp/' . $tmpname);
   }


}
