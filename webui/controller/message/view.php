<?php


class ControllerMessageView extends Controller {

   public function index(){

      $this->id = "content";
      $this->template = "message/view.tpl";
      $this->layout = "common/layout-empty";

      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('search/search');
      $this->load->model('search/message');
      $this->load->model('audit/audit');

      $this->load->model('user/user');

      $this->document->title = $this->data['text_message'];

      $this->data['id'] = '';

      if(isset($_SERVER['REQUEST_URI'])) { $this->data['id'] = preg_replace("/\/message.php\//", "", $_SERVER['REQUEST_URI']); }

      if($this->request->server['REQUEST_METHOD'] == 'POST') {
         $this->data['id'] = $this->request->post['id'];
      }


      if(!verify_piler_id($this->data['id'])) {
         AUDIT(ACTION_UNKNOWN, '', '', $this->data['id'], 'unknown piler id: ' . $this->data['id']);
         die("invalid id: " . $this->data['id']);
      }

      if(!$this->model_search_search->check_your_permission_by_piler_id($this->data['id'])) {
         AUDIT(ACTION_UNAUTHORIZED_VIEW_MESSAGE, '', '', $this->data['id'], '');
         die("no permission for " . $this->data['id']);
      }


      AUDIT(ACTION_VIEW_MESSAGE, '', '', $this->data['id'], '');

      $this->data['username'] = Registry::get('username');

      /* fix username if we are admin */

      if(isset($this->request->get['user']) && strlen($this->request->get['user']) > 1 && (Registry::get('admin_user') == 1 || $this->model_user_user->isUserInMyDomain($this->request->get['user']) == 1) ) {
         $this->data['username'] = $this->request->get['user'];
      }


      if($this->request->server['REQUEST_METHOD'] == 'POST') {
         $this->model_search_message->add_message_tag($this->data['id'], $_SESSION['uid'], $this->request->post['tag']);
         header("Location: " . $_SERVER['HTTP_REFERER']);
         exit;
      }


      $this->data['message'] = $this->model_search_message->extract_message($this->data['id']);
      $this->data['message']['tag'] = $this->model_search_message->get_message_tag($this->data['id'], $_SESSION['uid']);

      $this->render();
   }


}

?>
