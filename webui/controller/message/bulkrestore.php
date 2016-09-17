<?php


class ControllerMessageBulkrestore extends Controller {

   public function index(){

      $this->id = "content";
      $this->template = "message/bulkrestore.tpl";
      $this->layout = "common/layout-empty";

      $session = Registry::get('session');
      $request = Registry::get('request');
      $db = Registry::get('db');

      $imap_ok = 0;
      $email = '';

      $this->load->model('search/search');
      $this->load->model('search/message');
      $this->load->model('message/restore');

      $this->load->model('user/user');
      $this->load->model('mail/mail');


      $this->document->title = $this->data['text_message'];

      if(!isset($this->request->post['idlist']) || $this->request->post['idlist'] == '') { die("no idlist parameter given"); }

      $idlist = $this->model_search_search->check_your_permission_by_id_list(explode(",", $this->request->post['idlist']));

      $download = $this->request->post['download'];

      if(isset($this->request->post['email'])) { $email = $this->request->post['email']; }

      if($download == 1) {
         $this->model_message_restore->download_files_as_zip($idlist);
         exit;
      }


      $this->data['username'] = Registry::get('username');



      $this->data['restored'] = 0;

      if(RESTORE_OVER_IMAP == 1) {
         require_once 'Zend/Mail/Protocol/Imap.php';
         require_once 'Zend/Mail/Storage/Imap.php';

         $imap_ok = $this->model_mail_mail->connect_imap();

         if(!$imap_ok) {
            syslog(LOG_INFO, "imap connection failed for bulkrestore");
         }
      }


      foreach($idlist as $id) {

         AUDIT(ACTION_RESTORE_MESSAGE, '', '', $id, '');

         $rcpt = array();

         /* send the email to all the recipients of the original email if you are an auditor user */

         if(Registry::get('auditor_user') == 1) {
            if($email) {
               $rcpt[0] = $email;
            }
            else {
               $rcpt[0] = $session->get("email");
            }
         }
         else {
            array_push($rcpt, $session->get("email"));
         }

         if(count($rcpt) > 0) {
            $piler_id = $this->model_search_message->get_piler_id_by_id($id);

            $msg = $this->model_search_message->get_raw_message($piler_id);
            $this->model_search_message->remove_journal($msg);

            if(RESTORE_OVER_IMAP == 1) {
               if($imap_ok) {
                  $x = $this->imap->append(IMAP_RESTORE_FOLDER,  $msg);
                  syslog(LOG_INFO, "imap append $id/$piler_id, rc=$x");
               }
               else { $x = 0; }
            }
            else {

               if(RESTORE_EMAILS_AS_ATTACHMENT == 1) {
                  $msg = $this->model_mail_mail->message_as_rfc822_attachment($piler_id, $msg, $rcpt[0]);
                  $x = $this->model_mail_mail->send_smtp_email(SMARTHOST, SMARTHOST_PORT, SMTP_DOMAIN, SMTP_FROMADDR, $rcpt, $msg);
               }
               else {
                  $x = $this->model_mail_mail->send_smtp_email(SMARTHOST, SMARTHOST_PORT, SMTP_DOMAIN, SMTP_FROMADDR, $rcpt, 
                          "Received: by piler" . EOL . PILER_HEADER_FIELD . $id . EOL . $msg );
               }

            }

            if($x == 1) { $this->data['restored']++; }
         }
      }

      if(RESTORE_OVER_IMAP == 1) { $this->model_mail_mail->disconnect_imap(); }


      $this->render();
   }


}

?>
