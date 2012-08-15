<?php


class ControllerMessageBulkrestore extends Controller {

   public function index(){

      $this->id = "content";
      $this->template = "message/bulkrestore.tpl";
      $this->layout = "common/layout-empty";

      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('search/search');
      $this->load->model('search/message');
      $this->load->model('message/restore');

      $this->load->model('user/user');
      $this->load->model('mail/mail');

      $this->document->title = $this->data['text_message'];

      if(!isset($this->request->post['idlist']) || $this->request->post['idlist'] == '') { die("no idlist parameter given"); }

      list($a, $idlist) = $this->model_search_search->check_your_permission_by_id_list(explode(",", $this->request->post['idlist']));

      $download = $this->request->post['download'];


      if($download == 1) {
         $this->model_message_restore->download_files_as_zip($idlist);
         exit;
      }


      $this->data['username'] = Registry::get('username');



      $this->data['restored'] = 0;

      foreach($idlist as $id) {

         AUDIT(ACTION_RESTORE_MESSAGE, '', '', $id, '');

         $rcpt = array();

         /* send the email to all the recipients of the original email if we are admin or auditor users */

         if(Registry::get('auditor_user') == 1) {
            $rcpt = $this->model_search_search->get_message_recipients($id);
         }
         else {
            array_push($rcpt, $_SESSION['email']);
         }

         $x = $this->model_mail_mail->send_smtp_email(SMARTHOST, SMARTHOST_PORT, SMTP_DOMAIN, SMTP_FROMADDR, $rcpt, 
               "Received: by piler" . EOL . PILER_HEADER_FIELD . $id . EOL . $this->model_search_message->get_raw_message($id) );

         if($x == 1) { $this->data['restored']++; }
      }


      $this->render();
   }


}

?>
