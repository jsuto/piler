<?php


class ControllerMessageRestore extends Controller {

   public function index(){

      $this->id = "content";
      $this->template = "message/restore.tpl";
      $this->layout = "common/layout-empty";

      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('search/search');
      $this->load->model('search/message');

      $this->load->model('user/user');
      $this->load->model('mail/mail');

      $this->document->title = $this->data['text_message'];

      $this->data['id'] = @$this->request->get['id'];

      $rcpt = array();

      if(ENABLE_IMAP_AUTH == 1) {
         require_once 'Zend/Mail/Protocol/Imap.php';
         require_once 'Zend/Mail/Storage/Imap.php';
      }


      if(Registry::get('auditor_user') == 1) {
         $this->data['id'] = @$this->request->post['id'];
         $rcpt = preg_split("/\s/", $this->request->post['rcpt']);
      }


      if(!verify_piler_id($this->data['id'])) {
         AUDIT(ACTION_UNKNOWN, '', '', $this->data['id'], 'unknown piler id: ' . $this->data['id']);
         die("invalid id: " . $this->data['id']);
      }

      if(!$this->model_search_search->check_your_permission_by_id($this->data['id'])) {
         AUDIT(ACTION_UNAUTHORIZED_VIEW_MESSAGE, '', '', $this->data['id'], '');
         die("no permission for " . $this->data['id']);
      }

      AUDIT(ACTION_RESTORE_MESSAGE, '', '', $this->data['id'], '');


      $this->data['username'] = Registry::get('username');


      /* send the email to all the recipients of the original email if we are admin or auditor users */

      if(Registry::get('auditor_user') == 0) {
         array_push($rcpt, $_SESSION['email']);
      }

      $this->data['data'] = $this->data['text_failed_to_restore'];

      if(count($rcpt) > 0) {

         $this->data['piler_id'] = $this->model_search_message->get_piler_id_by_id($this->data['id']);

         $msg = $this->model_search_message->get_raw_message($this->data['piler_id']);
         $this->model_search_message->remove_journal($msg);

         if(ENABLE_IMAP_AUTH == 1) {
            if($this->model_mail_mail->connect_imap()) {
               $x = $this->imap->append('INBOX',  $msg);
               $this->model_mail_mail->disconnect_imap();
            }
            else {
               $x = 0;
            }
         }
         else {

            $x = $this->model_mail_mail->send_smtp_email(SMARTHOST, SMARTHOST_PORT, SMTP_DOMAIN, SMTP_FROMADDR, $rcpt, 
               "Received: by piler" . EOL . PILER_HEADER_FIELD . $this->data['id'] . EOL . $msg );
         }

         if($x == 1) { $this->data['data'] = $this->data['text_restored']; }
      }

      $this->render();
   }


}

?>
