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

      if(!verify_piler_id($this->data['id'])) {
         AUDIT(ACTION_UNKNOWN, '', '', $this->data['id'], 'unknown piler id: ' . $this->data['id']);
         die("invalid id: " . $this->data['id']);
      }

      if(!$this->model_search_search->check_your_permission_by_piler_id($this->data['id'])) {
         AUDIT(ACTION_UNAUTHORIZED_VIEW_MESSAGE, '', '', $this->data['id'], '');
         die("no permission for " . $this->data['id']);
      }

      AUDIT(ACTION_RESTORE_MESSAGE, '', '', $this->data['id'], '');


      $this->data['username'] = Registry::get('username');

      $rcpt = array();


      /* send the email to all the recipients of the original email if we are admin or auditor users */

      if(Registry::get('admin_user') == 1 || Registry::get('auditor_user') == 1) {
         $rcpt = $this->model_search_search->get_message_recipients($this->data['id']);
      }
      else {
         array_push($rcpt, $_SESSION['email']);
      }


      $x = $this->model_mail_mail->send_smtp_email(SMARTHOST, SMARTHOST_PORT, SMTP_DOMAIN, SMTP_FROMADDR, $rcpt, 
            "Received: by piler" . EOL . PILER_HEADER_FIELD . $this->data['id'] . EOL . $this->model_search_message->get_raw_message($this->data['id']) );

      if($x == 1) { $this->data['data'] = $this->data['text_restored']; }
      else { $this->data['data'] = $this->data['text_failed_to_restore']; }

      $this->render();
   }


}

?>
