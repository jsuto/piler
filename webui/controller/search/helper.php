<?php


class ControllerSearchHelper extends Controller {
   private $error = array();
   private $a = array(
                    'from'            => '',
                    'to'              => '',
                    'subject'         => '',
                    'body'            => '',
                    'date1'           => '',
                    'date2'           => '',
                    'direction'       => '',
                    'size'            => '',
                    'attachment_type' => '',
                    'tag'             => '',
                    'note'            => '',
                    'ref'             => '',
                    'folders'         => '',
                    'extra_folders'   => '',
                    'any'             => ''
                     );


   public function index(){

      $this->id = "content";
      $this->template = "search/helper.tpl";
      $this->layout = "common/layout-empty";


      $request = Registry::get('request');
      $db = Registry::get('db');

      $sphx = Registry::get('sphx');

      $this->load->model('search/search');
      $this->load->model('search/message');
      $this->load->model('user/user');


      $this->data['page'] = 0;
      if(isset($this->request->post['page'])) { $this->data['page'] = $this->request->post['page']; }

      $this->data['page_len'] = get_page_length();

      $this->data['n'] = -1;

      if($this->request->post['searchtype'] == 'expert'){

         if(isset($this->request->post['search']) && preg_match("/(from|to|subject|body|direction|size|date1|date2|attachment|tagnote)\:/", $this->request->post['search'])) {
            $this->preprocess_post_expert_request($this->request->post);
         }
         else {
            $this->naive_preprocess_post_expert_request($this->request->post);
         }

         $this->fixup_post_request();

         list ($this->data['n'], $this->data['all_ids'], $this->data['messages']) = $this->model_search_search->search_messages($this->a, $this->data['page']);
      }

      else {
         $this->fixup_post_simple_request();
         list ($this->data['n'], $this->data['all_ids'], $this->data['messages']) = $this->model_search_search->search_messages($this->request->post, $this->data['page']);
      }


      if($this->a['ref']) { $this->data['_ref'] = $this->a['ref']; }
      if(isset($this->request->post['ref']) && $this->request->post['ref']) { $this->data['_ref'] = $this->request->post['ref']; }

      /* paging info */

      $this->data['prev_page'] = $this->data['page'] - 1;
      $this->data['next_page'] = $this->data['page'] + 1;

      $this->data['total_pages'] = ceil($this->data['n'] / $this->data['page_len'])-1;

      $this->data['hits_from'] = $this->data['page'] * $this->data['page_len'] + 1;
      $this->data['hits_to'] = ($this->data['page']+1) * $this->data['page_len'];

      if($this->data['hits_to'] > $this->data['n']) { $this->data['hits_to'] = $this->data['n']; }


      $this->render();
   }


   private function fixup_post_simple_request() {
      if(!isset($this->request->post['date1'])) { $this->request->post['date1'] = ''; }
      if(!isset($this->request->post['date2'])) { $this->request->post['date2'] = ''; }
   }


   private function fixup_post_request() {
      if(isset($this->request->post['ref'])) { $this->a['ref'] = $this->request->post['ref']; }
      if(isset($this->request->post['folders'])) { $this->a['folders'] = $this->request->post['folders']; }
      if(isset($this->request->post['extra_folders'])) { $this->a['extra_folders'] = $this->request->post['extra_folders']; }

      $this->a['sort'] = $this->request->post['sort'];
      $this->a['order'] = $this->request->post['order'];
   }


   private function naive_preprocess_post_expert_request($data = array()) {
      $ndate = 0;

      if(!isset($data['search'])) { return; }

      $b = preg_split("/\s/", $data['search']);

      while(list($k, $v) = each($b)) {
         if($v == '') { continue; }

         if(preg_match("/\d{4}\-\d{1,2}\-\d{1,2}/", $v)) {
            $ndate++;
            $this->a["date$ndate"] = $v;
         }
         else if(strchr($v, '@')) {
            $this->a['from'] .= " $v";
         }
         else { $this->a['any'] .= ' ' . $v; }
      }

      if($this->a['date1'] && $this->a['date2'] == '') { $this->a['date2'] = $this->a['date1']; }

      if($this->a['any'] == ' ' . $this->data['text_enter_search_terms']) { $this->a['any'] = ''; }
   }


   private function preprocess_post_expert_request($data = array()) {
      $token = '';
      $ndate = 0;

      if(!isset($data['search'])) { return; }

      $s = preg_replace("/:/", ": ", $data['search']);
      $s = preg_replace("/,/", " ", $s);
      $s = preg_replace("/\s{1,}/", " ", $s);
      $b = explode(" ", $s);

      while(list($k, $v) = each($b)) {
         if($v == '') { continue; }

         if($v == 'from:') { $token = 'from'; continue; }
         else if($v == 'to:') { $token = 'to'; continue; }
         else if($v == 'subject:') { $token = 'subject'; continue; }
         else if($v == 'body:') { $token = 'body'; continue; }
         else if($v == 'direction:' || $v == 'd:') { $token = 'direction'; continue; }
         else if($v == 'size:') { $token = 'size'; continue; }
         else if($v == 'date1:') { $token = 'date1'; continue; }
         else if($v == 'date2:') { $token = 'date2'; continue; }
         else if($v == 'attachment:' || $v == 'a:') { $token = 'attachment_type'; continue; }
         else if($v == 'size') { $token = 'size'; continue; }
         else if($v == 'tag:') { $token = 'tag'; continue; }
         else if($v == 'note:') { $token = 'note'; continue; }
         else if($v == 'ref:') { $token = 'ref'; continue; }
         else {
            if(preg_match("/\d{4}\-\d{1,2}\-\d{1,2}/", $v)) {
               $ndate++;
               $this->a["date$ndate"] = $v;
            }
         }


         if($token == 'from') {
            if($v == 'OR') { continue; }
            $this->a['from'] .= " $v";
         }

         else if($token == 'to') {
            if($v == 'OR') { continue; }
            $this->a['to'] .= " $v";
         }

         else if($token == 'subject') { $this->a['subject'] .= ' ' . $v; }
         else if($token == 'body') { $this->a['body'] .= ' ' . $v; }
         else if($token == 'date1') { $this->a['date1'] = ' ' . $v; }
         else if($token == 'date2') { $this->a['date2'] = ' ' . $v; }
         else if($token == 'attachment_type') { $this->a['attachment_type'] .= '|' . $v; }
         else if($token == 'tag') { $this->a['tag'] .= ' ' . $v; }
         else if($token == 'note') { $this->a['note'] .= ' ' . $v; }
         else if($token == 'ref') { $this->a['ref'] = ' ' . $v; }

         else if($token == 'direction') {
            if($v == 'inbound') { $this->a['direction'] = "0"; }
            else if($v == 'outbound') { $this->a['direction'] = 2; }
            else if($v == 'internal') { $this->a['direction'] = 1; }
         }

         else if($token == 'size') {
            $o = substr($v, 0, 1);
            if($o == '<' || $o == '>') {
               $v = substr($v, 1, strlen($v));
               $o1 = substr($v, 0, 1);
               if($o1 == '=') {
                  $v = substr($v, 1, strlen($v));
                  $o .= $o1;
               }
            }
            else { $o = ''; }

            $s = explode("k", $v);
            if($s[0] != $v) { $v = $s[0] * 1000; }

            $s = explode("M", $v);
            if($s[0] != $v) { $v = $s[0] * 1000000; }

            $this->a['size'] .= ' ' . $o . $v;
         }

      }

      if($this->a['any'] == ' ' . $this->data['text_enter_search_terms']) { $this->a['any'] = ''; }

      $this->a['attachment_type'] = substr($this->a['attachment_type'], 1, strlen($this->a['attachment_type']));
   }


}

?>
