<?php


class ControllerSearchHelper extends Controller {
   private $error = array();
   private $a = array(
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
                    'id'              => '',
                    'match'           => array()
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

         if(isset($this->request->post['search']) && preg_match("/(from|to|subject|body|direction|d|size|date1|date2|attachment|a|tag|note|id)\:/", $this->request->post['search'])) {
            $this->preprocess_post_expert_request($this->request->post);
         }
         else {
            $this->naive_preprocess_post_expert_request($this->request->post);
         }

         $this->fixup_post_request();

         list ($this->data['n'], $this->data['total_found'], $this->data['all_ids'], $this->data['messages']) = $this->model_search_search->search_messages($this->a, $this->data['page']);
      }

      else {
         $this->fixup_post_simple_request();
         list ($this->data['n'], $this->data['total_found'], $this->data['all_ids'], $this->data['messages']) = $this->model_search_search->search_messages($this->a, $this->data['page']);
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

      $this->data['sort'] = $this->request->post['sort'];
      $this->data['order'] = $this->request->post['order'];

      $this->render();
   }


   private function fixup_post_simple_request() {
      $match = '';

      if(isset($this->request->post['from']) && $this->request->post['from']) { $match .= "@from " . $this->request->post['from'] . ' '; }
      if(isset($this->request->post['to']) && $this->request->post['to']) { $match .= "@to " . $this->request->post['to'] . ' '; }
      if(isset($this->request->post['subject']) && $this->request->post['subject']) { $match .= "@subject " . $this->request->post['subject'] . ' '; }
      if(isset($this->request->post['body']) && $this->request->post['body']) { $match .= "@body " . $this->request->post['body'] . ' '; }

      if(isset($this->request->post['tag'])) { $this->a['tag'] = $this->request->post['tag']; }
      if(isset($this->request->post['note'])) { $this->a['note'] = $this->request->post['note']; }
      if(isset($this->request->post['attachment_type'])) { $this->a['attachment_type'] = $this->request->post['attachment_type']; }

      if(isset($this->request->post['date1'])) { $this->a['date1'] = $this->request->post['date1']; }
      if(isset($this->request->post['date2'])) { $this->a['date2'] = $this->request->post['date2']; }

      if($this->a['attachment_type'] && $this->a['attachment_type'] != "any") { $match .= " @attachment_types " . preg_replace("/,/", " OR ", $this->a['attachment_type']); }

      $match = preg_replace("/OR/", "|", $match);
      $this->a['match'] = preg_split("/ /", $match);
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
      $from = $match = '';
      $prev_token_is_email = 0;

      if(!isset($data['search'])) { return; }

      $s = preg_replace("/OR/", "|", $data['search']);
      $b = preg_split("/\s/", $s);

      while(list($k, $v) = each($b)) {
         if($v == '') { continue; }

         if(preg_match("/\d{4}\-\d{1,2}\-\d{1,2}/", $v) || preg_match("/\d{1,2}\/\d{1,2}\/\d{4}/", $v)) {
            $ndate++;
            $this->a["date$ndate"] = $v;
         }
         else if(strchr($v, '@')) {
            $prev_token_is_email = 1;
            if($from == '') { $from = "@from"; }
            $from .= " $v";
         }
         else {
            if($prev_token_is_email == 1) {
               $prev_token_is_email = 0;
               $from .= " $v";
            }
            else {
               $match .= ' ' . $v;
            }
         }
      }

      if($match && $match != ' ' . $this->data['text_enter_search_terms']) {
         $match = "@(subject,body) $match";
      }


      if($from) { $match = $from . ' ' . $match; }

      $this->a['match'] = preg_split("/ /", $match);

      if($this->a['date1'] && $this->a['date2'] == '') { $this->a['date2'] = $this->a['date1']; }

   }


   private function preprocess_post_expert_request($data = array()) {
      $token = 'match';
      $ndate = 0;
      $match = array();

      if(!isset($data['search'])) { return; }

      $s = preg_replace("/:/", ": ", $data['search']);
      $s = preg_replace("/,/", " ", $s);
      $s = preg_replace("/\(/", "( ", $s);
      $s = preg_replace("/\)/", ") ", $s);
      $s = preg_replace("/OR/", "|", $s);
      $s = preg_replace("/AND/", "", $s);
      $s = preg_replace("/\s{1,}/", " ", $s);
      $b = explode(" ", $s);

      while(list($k, $v) = each($b)) {
         if($v == '') { continue; }

         if($v == 'from:') { $token = 'match'; $this->a['match'][] = '@from'; continue; }
         else if($v == 'to:') { $token = 'match'; $this->a['match'][] = '@to'; continue; }
         else if($v == 'subject:') { $token = 'match'; $this->a['match'][] = '@subject'; continue; }
         else if($v == 'body:') { $token = 'match'; $this->a['match'][] = '@body'; continue; }
         else if($v == 'direction:' || $v == 'd:') { $token = 'direction'; continue; }
         else if($v == 'size:') { $token = 'size'; continue; }
         else if($v == 'date1:') { $token = 'date1'; continue; }
         else if($v == 'date2:') { $token = 'date2'; continue; }
         else if($v == 'attachment:' || $v == 'a:') { $token = 'match'; $this->a['match'][] = '@attachment_types'; continue; }
         else if($v == 'size') { $token = 'size'; continue; }
         else if($v == 'tag:') { $token = 'tag'; continue; }
         else if($v == 'note:') { $token = 'note'; continue; }
         else if($v == 'ref:') { $token = 'ref'; continue; }
         else if($v == 'id:') { $token = 'id'; continue; }
         else {
            if(preg_match("/\d{4}\-\d{1,2}\-\d{1,2}/", $v) || preg_match("/\d{1,2}\/\d{1,2}\/\d{4}/", $v)) {
               $ndate++;
               $this->a["date$ndate"] = $v;
            }
         }


         if($token == 'match') { $this->a['match'][] = $v; }
         else if($token == 'date1') { $this->a['date1'] = ' ' . $v; }
         else if($token == 'date2') { $this->a['date2'] = ' ' . $v; }
         else if($token == 'tag') { $this->a['tag'] .= ' ' . $v; }
         else if($token == 'note') { $this->a['note'] .= ' ' . $v; }
         else if($token == 'ref') { $this->a['ref'] = ' ' . $v; }
         else if($token == 'id') { $this->a['id'] .= ' ' . $v; }

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

   }


}

?>
