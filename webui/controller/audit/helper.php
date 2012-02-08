<?php


class ControllerAuditHelper extends Controller {
   private $error = array();
   private $search_args = 0;

   public function index(){

      $this->id = "content";
      $this->template = "audit/helper.tpl";
      $this->layout = "common/layout-empty";


      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('audit/audit');


      $this->data['page'] = 0;
      if(isset($this->request->post['page'])) { $this->data['page'] = $this->request->post['page']; }

      $this->data['page_len'] = get_page_length();

      $this->data['n'] = -1;



      if(isset($this->request->post)) {
         $a = $this->fixup_request($this->request->post);
         list($this->data['n'], $this->data['messages']) = $this->model_audit_audit->search_audit($a);
      }

      $this->data['actions'][ACTION_UNKNOWN] = '??';
      $this->data['actions'][ACTION_LOGIN] = $this->data['text_login2'];
      $this->data['actions'][ACTION_LOGIN_FAILED] = $this->data['text_login_failed'];
      $this->data['actions'][ACTION_LOGOUT] = $this->data['text_logout2'];
      $this->data['actions'][ACTION_VIEW_MESSAGE] = $this->data['text_view_message2'];
      $this->data['actions'][ACTION_VIEW_HEADER] = $this->data['text_view_header'];
      $this->data['actions'][ACTION_UNAUTHORIZED_VIEW_MESSAGE] = $this->data['text_unauthorized_view_message'];
      $this->data['actions'][ACTION_RESTORE_MESSAGE] = $this->data['text_restore_message'];
      $this->data['actions'][ACTION_DOWNLOAD_MESSAGE] = $this->data['text_download_message2'];
      $this->data['actions'][ACTION_SEARCH] = $this->data['text_search2'];
      $this->data['actions'][ACTION_SAVE_SEARCH] = $this->data['text_save_search'];
      $this->data['actions'][ACTION_CHANGE_USER_SETTINGS] = $this->data['text_change_user_settings'];
      $this->data['actions'][ACTION_REMOVE_MESSAGE] = $this->data['text_remove_message2'];
      $this->data['actions'][ACTION_UNAUTHORIZED_REMOVE_MESSAGE] = $this->data['text_unauthorized_remove_message'];



      
      /* paging info */

      $this->data['prev_page'] = $this->data['page'] - 1;
      $this->data['next_page'] = $this->data['page'] + 1;

      $this->data['total_pages'] = ceil($this->data['n'] / $this->data['page_len'])-1;

      $this->data['hits_from'] = $this->data['page'] * $this->data['page_len'] + 1;
      $this->data['hits_to'] = ($this->data['page']+1) * $this->data['page_len'];

      if($this->data['hits_to'] > $this->data['n']) { $this->data['hits_to'] = $this->data['n']; }


      $this->render();
   }


   private function fixup_request($data = array()) {
      $arr = array();

      if(isset($data['f'])) {
         foreach($data['f'] as $f) {
            $val = array_shift($data['v']);


            if($val == '') { continue; }

            if($f == 'user') { if(isset($arr['user'])) { $arr['user'] .= '*' . $val; } else { $arr['user'] = $val; } }
            if($f == 'ipaddr') { if(isset($arr['ipaddr'])) { $arr['ipaddr'] .= '*' . $val; } else { $arr['ipaddr'] = $val; } }
            if($f == 'ref') { if(isset($arr['ref'])) { $arr['ref'] .= '*' . $val; } else { $arr['ref'] = $val; } }
         }
      }

      if(isset($data['action'])) { $arr['action'] = $data['action']; }

      if(isset($data['date1'])) { $arr['date1'] = $data['date1']; }
      if(isset($data['date2'])) { $arr['date2'] = $data['date2']; }

      if(isset($data['sort'])) { $arr['sort'] = $data['sort']; }
      if(isset($data['order'])) { $arr['order'] = $data['order']; }

      $arr['page'] = $this->data['page'];
      $arr['page_len'] = $this->data['page_len'];

      return $arr;
   }


}

?>
