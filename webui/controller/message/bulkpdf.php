<?php


class ControllerMessageBulkPDF extends Controller {

   public function index(){

      $this->id = "content";
      $this->template = "message/bulkrestore.tpl";
      $this->layout = "common/layout-empty";

      $request = Registry::get('request');
      $db = Registry::get('db');

      $imap_ok = 0;

      $this->load->model('search/search');
      $this->load->model('search/message');
      $this->load->model('message/pdf');

      $this->load->model('user/user');
      $this->load->model('mail/mail');

      $this->load->helper('tcpdf/config/lang/hun');
      $this->load->helper('tcpdf/tcpdf');

      $this->document->title = $this->data['text_message'];

      if(!isset($this->request->post['idlist']) || $this->request->post['idlist'] == '') { die("no idlist parameter given"); }

      $idlist = $this->model_search_search->check_your_permission_by_id_list(explode(",", $this->request->post['idlist']));

      $this->model_message_pdf->download_files_as_zip($idlist);
   }


}

?>
