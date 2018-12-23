<?php


class ControllerMessageJournal extends Controller {

   public function index(){

      $this->id = "content";
      $this->template = "message/journal.tpl";
      $this->layout = "common/layout-empty";

      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('search/search');
      $this->load->model('search/message');
      $this->load->model('audit/audit');
      $this->load->model('user/user');

      $this->document->title = $this->data['text_message'];

      $this->data['id'] = @$this->request->get['id'];

      if(!verify_piler_id($this->data['id'])) {
         AUDIT(ACTION_UNKNOWN, '', '', $this->data['id'], 'unknown id: ' . $this->data['id']);
         die("invalid id: " . $this->data['id']);
      }

      if(!$this->model_search_search->check_your_permission_by_id($this->data['id'])) {
         AUDIT(ACTION_UNAUTHORIZED_VIEW_MESSAGE, '', '', $this->data['id'], '');
         die("no permission for " . $this->data['id']);
      }

      AUDIT(ACTION_VIEW_JOURNAL, '', '', $this->data['id'], '');

      if(Registry::get('auditor_user') == 1) { $this->data['rcpt'] = $this->model_search_search->get_message_addresses_in_my_domain($this->data['id']); }

      $this->data['piler_id'] = $this->model_search_message->get_piler_id_by_id($this->data['id']);

      $this->data['data'] = $this->model_search_message->get_message_journal($this->data['piler_id']);

      $this->data['can_download'] = $this->model_audit_audit->can_download();
      $this->data['can_restore'] = $this->model_audit_audit->can_restore();

      $this->render();
   }


}

?>
