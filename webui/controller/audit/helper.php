<?php


class ControllerAuditHelper extends Controller {
   private $error = array();
   private $a = array(
                    'user'            => '',
                    'ipaddr'          => '',
                    'action'          => '',
                    'ref'             => '',
                    'date1'           => '',
                    'date2'           => '',
                    'order'           => '',
                    'sort'            => '',
                    'page_len'        => 50
                    );


   public function index(){

      $this->id = "content";
      $this->template = "audit/helper.tpl";
      $this->layout = "common/layout-empty";


      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('audit/audit');

      if(Registry::get('admin_user') == 0) {
         die("go away");
      }


      $this->data['page'] = 0;
      if(isset($this->request->post['page'])) { $this->data['page'] = $this->request->post['page']; }

      $this->data['page_len'] = get_page_length();

      $this->data['n'] = -1;



      if(isset($this->request->post)) {
         $this->fixup_request($this->request->post);
         list($this->data['n'], $this->data['messages']) = $this->model_audit_audit->search_audit($this->a);
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
      $ndate = 0;
      global $actions;

      if(!isset($data['search'])) { return; }

      $s = preg_replace("/:/", ": ", $data['search']);
      $s = preg_replace("/,/", " ", $s);
      $s = preg_replace("/\s{1,}/", " ", $s);
      $b = explode(" ", $s);

      while(list($k, $v) = each($b)) {
         if($v == '') { continue; }

         if(preg_match("/(login|loginfailed|logout|view|download|search|restore)$/", $v) && isset($actions[$v])) { $this->a['action'] .= '*' . $actions[$v]; }
         if(preg_match("/\@/", $v)) { $this->a['user'] .= '*' . $v; }
         if(preg_match("/\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}/", $v)) { $this->a['ipaddr'] .= '*' . $v; }
         if(preg_match("/^\d{1,}$/", $v)) { $this->a['ref'] .= '*' . $v; }
         if(preg_match("/\d{4}\-\d{1,2}\-\d{1,2}/", $v) || preg_match("/\d{4}\.\d{1,2}\.\d{1,2}/", $v)) {
            $ndate++;
            $this->a["date$ndate"] = $v;
         }
      }


      $this->a['user'] = preg_replace("/^\*/", "", $this->a['user']);
      $this->a['ipaddr'] = preg_replace("/^\*/", "", $this->a['ipaddr']);
      $this->a['ref'] = preg_replace("/^\*/", "", $this->a['ref']);
      $this->a['action'] = preg_replace("/^\*/", "", $this->a['action']);

      //if(isset($data['action'])) { $arr['action'] = $data['action']; }

      if(isset($data['sort'])) { $this->a['sort'] = $data['sort']; }
      if(isset($data['order'])) { $this->a['order'] = $data['order']; }
      if(isset($data['page'])) { $this->a['page'] = $data['page']; }
      if(isset($data['page_len'])) { $this->a['page_len'] = $data['page_len']; }

   }


}

?>
