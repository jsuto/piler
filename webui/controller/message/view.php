<?php


class ControllerMessageView extends Controller {

   public function index(){

      $this->id = "content";
      $this->template = "message/view.tpl";
      $this->layout = "common/layout-empty";

      $session = Registry::get('session');
      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('search/search');
      $this->load->model('search/message');
      $this->load->model('audit/audit');

      $this->load->model('user/user');

      if(ENABLE_FOLDER_RESTRICTIONS == 1) {
         $this->load->model('folder/folder');
      }

      $this->document->title = $this->data['text_message'];

      $this->data['id'] = '';
      $this->data['search'] = '';
      $this->data['rcpt'] = array();

      if(isset($_SERVER['REQUEST_URI'])) { $a = preg_split("/\//", $_SERVER['REQUEST_URI']); $this->data['id'] = $a[count($a)-1]; }

      if($this->request->server['REQUEST_METHOD'] == 'POST') {
         $this->data['id'] = $this->request->post['id'];
         $this->data['search'] = $this->request->post['search'];
      }

      if(substr($this->data['id'], 0, 1) == 'a') {
         $this->template = "message/auto.tpl";
         $this->data['id'] = substr($this->data['id'], 1, 200);
      }


      if(!verify_piler_id($this->data['id'])) {
         AUDIT(ACTION_UNKNOWN, '', '', $this->data['id'], 'unknown id: ' . $this->data['id']);
         die("invalid id: " . $this->data['id']);
      }

      if(!$this->model_search_search->check_your_permission_by_id($this->data['id'])) {
         AUDIT(ACTION_UNAUTHORIZED_VIEW_MESSAGE, '', '', $this->data['id'], '');
         die("no permission for " . $this->data['id']);
      }


      AUDIT(ACTION_VIEW_MESSAGE, '', '', $this->data['id'], '');

      $this->data['username'] = Registry::get('username');

      if(Registry::get('auditor_user') == 1) { $this->data['rcpt'] = $this->model_search_search->get_message_addresses_in_my_domain($this->data['id']); }

      /* fix username if we are admin */

      if(isset($this->request->get['user']) && strlen($this->request->get['user']) > 1 && (Registry::get('admin_user') == 1 || $this->model_user_user->isUserInMyDomain($this->request->get['user']) == 1) ) {
         $this->data['username'] = $this->request->get['user'];
      }


      if($this->request->server['REQUEST_METHOD'] == 'POST' && isset($this->request->post['tag'])) {
         $this->model_search_message->add_message_tag($this->data['id'], $session->get("uid"), $this->request->post['tag']);
         header("Location: " . $_SERVER['HTTP_REFERER']);
         exit;
      }

      $this->data['piler_id'] = $this->model_search_message->get_piler_id_by_id($this->data['id']);

      $this->data['attachments'] = $this->model_search_message->get_attachment_list($this->data['piler_id']);

      $this->data['message'] = $this->model_search_message->extract_message($this->data['piler_id'], $this->data['search']);
      $this->data['message']['tag'] = $this->model_search_message->get_message_tag($this->data['id'], $session->get("uid"));
      $this->data['message']['note'] = $this->model_search_message->get_message_note($this->data['id'], $session->get("uid"));

      $this->data['message']['private'] = $this->model_search_message->get_message_private($this->data['id']);

      $this->data['spam'] = $this->model_search_message->is_message_spam($this->data['id']);

      $this->data['images'] = array();

      $this->data['meta'] = $this->model_search_message->get_metadata_by_id($this->data['id']);

      $this->data['can_download'] = $this->model_audit_audit->can_download();
      $this->data['can_restore'] = $this->model_audit_audit->can_restore();

      if(ENABLE_FOLDER_RESTRICTIONS == 1) {
         $this->data['folders'] = $this->model_folder_folder->get_folders_for_user();
         $this->data['folder_id'] = $this->model_folder_folder->get_folder_id_by_id($this->data['id']);
      }

      if(TSA_PUBLIC_KEY_FILE && MEMCACHED_ENABLED) {
         $this->data['tsa'] = $this->model_search_message->get_tsa_award();
      }

      if(Registry::get('auditor_user') == 1 && SHOW_HEADERS_FOR_AUDITOR_MESSAGE_PREVIEW) {
         $this->data['headers'] = $this->model_search_message->get_message_headers($this->data['piler_id']);
      }

      foreach($this->data['attachments'] as $a) {
         if(preg_match('/image/', $a['type'])) {
            $attachment = $this->model_search_message->get_attachment_by_id($a['id']);

            $fname = 'i.' . $a['id'];
            if(strstr($a['type'], 'svg')) {
               $fname .= '.svg';
            }

            $fp = fopen(DIR_BASE . 'tmp/' . $fname, 'w+');
            if($fp) {
               fwrite($fp, $attachment['attachment']);
               fclose($fp);

               $this->data['images'][] = array(
                                           'name' => $fname
                                         );
            }
         }
      }


      $this->render();
   }


}
