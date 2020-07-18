<?php


class ControllerSearchHelper extends Controller {
   private $error = array();
   private $a = array(
                    'date1'           => '',
                    'date2'           => '',
                    'direction'       => '',
                    'size'            => '',
                    'aname'           => '',
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
      if(ENABLE_MOBILE_PREVIEW && MOBILE_DEVICE) {
         $this->template = "search/helper-mobile.tpl";
      } else {
         $this->template = "search/helper.tpl";
      }

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

         if(isset($this->request->post['search']) && preg_match("/(from|to|subject|body|direction|d|size|date1|date2|attachment|a|aname|tag|note|id)\:/", $this->request->post['search'])) {
            $this->a = $this->model_search_search->preprocess_post_expert_request($this->request->post);
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

      if($this->data['total_found'] > MAX_SEARCH_HITS) {
         $this->data['total_pages'] = ceil(MAX_SEARCH_HITS / $this->data['page_len'])-1;
         $this->data['hits'] = MAX_SEARCH_HITS;
      }
      else {
         $this->data['total_pages'] = $this->data['total_pages'] = ceil($this->data['total_found'] / $this->data['page_len'])-1;
         $this->data['hits'] = $this->data['total_found'];
      }

      $this->data['hits_from'] = $this->data['page'] * $this->data['page_len'] + 1;
      $this->data['hits_to'] = $this->data['page'] * $this->data['page_len'] + $this->data['n'];

      $this->data['sort'] = $this->request->post['sort'];
      $this->data['order'] = $this->request->post['order'];

      $this->render();
   }


   private function fixup_post_simple_request() {
      $match = '';

      if(isset($this->request->post['from']) && $this->request->post['from']) { $match .= sprintf('%s %s ', FROM_TOKEN, $this->request->post['from']); }
      if(isset($this->request->post['to']) && $this->request->post['to']) { $match .= sprintf('%s %s ', TO_TOKEN, $this->request->post['to']); }
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

      $this->a['sort'] = "date";
      $this->a['order'] = 0;
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
            if($from == '') { $from = FROM_TOKEN; }
            $from .= " $v";
         }
         else {
            $match .= ' ' . $v;
         }
      }


      if($from) { $match = $from . ' ' . $match; }

      $this->a['match'] = preg_split("/ /", $match);

      if($this->a['date1'] && $this->a['date2'] == '') { $this->a['date2'] = $this->a['date1']; }

   }


}

?>
