<?php


class ControllerSearchHelper extends Controller {
   private $error = array();
   private $a = array(
                    'o_from'          => '',
                    'f_from'          => '',
                    'o_to'            => '',
                    'f_to'            => '',
                    'from'            => '',
                    'to'              => '',
                    'from_domain'     => '',
                    'to_domain'       => '',
                    'subject'         => '',
                    'body'            => '',
                    'date1'           => '',
                    'date2'           => '',
                    'direction'       => '',
                    'size'            => '',
                    'attachment_type' => '',
                    'tag'             => '',
                    'ref'             => '' 
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

      if($this->request->post['searchtype'] == 'advanced') {

         $this->preprocess_post_advanced_request($this->request->post);
         $this->fixup_post_request();

         list ($this->data['n'], $this->data['messages']) = $this->model_search_search->search_messages($this->a, ADVANCED_SEARCH, $this->data['page']);
      }

      else if($this->request->post['searchtype'] == 'expert'){

         $this->preprocess_post_expert_request($this->request->post);
         $this->fixup_post_request();

         list ($this->data['n'], $this->data['messages']) = $this->model_search_search->search_messages($this->a, ADVANCED_SEARCH, $this->data['page']);
      }

      else {
         $this->fixup_post_simple_request();
         list ($this->data['n'], $this->data['messages']) = $this->model_search_search->search_messages($this->request->post, SIMPLE_SEARCH, $this->data['page']);
      }


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
      $this->a['o_from'] = substr($this->a['o_from'], 1, strlen($this->a['o_from']));
      $this->a['f_from'] = substr($this->a['f_from'], 1, strlen($this->a['f_from']));
      $this->a['o_to'] = substr($this->a['o_to'], 1, strlen($this->a['o_to']));
      $this->a['f_to'] = substr($this->a['f_to'], 1, strlen($this->a['f_to']));
      $this->a['from'] = substr($this->a['from'], 1, strlen($this->a['from']));
      $this->a['to'] = substr($this->a['to'], 1, strlen($this->a['to']));
      $this->a['from_domain'] = substr($this->a['from_domain'], 1, strlen($this->a['from_domain']));
      $this->a['to_domain'] = substr($this->a['to_domain'], 1, strlen($this->a['to_domain']));

      if(isset($this->request->post['ref'])) { $this->a['ref'] = $this->request->post['ref']; }

      $this->a['sort'] = $this->request->post['sort'];
      $this->a['order'] = $this->request->post['order'];
   }


   private function preprocess_post_expert_request($data = array()) {
      $token = '';

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
         else if($v == 'ref:') { $token = 'ref'; continue; }

         if($token == 'from') {
            $v = fix_email_address($v);

            if(substr($v, 0, 1) == '@') { $this->a['from_domain'] .= "|$v"; }
            else if(strstr($v, '@')) {
               if(in_array($v, $_SESSION['emails'])) { $this->a['o_from'] .= "|$v"; } else { $this->a['f_from'] .= "|$v"; }
            }
            else {
               $this->a['from'] .= " $v";
            }
         }
         else if($token == 'to') {
            $v = fix_email_address($v);

            if(substr($v, 0, 1) == '@') { $this->a['to_domain'] .= "|$v"; }
            else if(strstr($v, '@')) {
               if(in_array($v, $_SESSION['emails'])) { $this->a['o_to'] .= "|$v"; } else { $this->a['f_to'] .= "|$v"; }
            }
            else {
               $this->a['to'] .= " $v";
            }
         }
         else if($token == 'subject') { $this->a['subject'] .= ' ' . $v; }
         else if($token == 'body') { $this->a['body'] .= ' ' . $v; }
         else if($token == 'date1') { $this->a['date1'] = ' ' . $v; }
         else if($token == 'date2') { $this->a['date2'] = ' ' . $v; }
         else if($token == 'attachment_type') { $this->a['attachment_type'] .= '|' . $v; }
         else if($token == 'tag') { $this->a['tag'] .= ' ' . $v; }
         else if($token == 'ref') { $this->a['ref'] = ' ' . $v; }

         else if($token == 'direction') {
            if($v == 'inbound') { $this->a['direction'] = 0; }
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

      $this->a['attachment_type'] = substr($this->a['attachment_type'], 1, strlen($this->a['attachment_type']));
   }


   private function preprocess_post_advanced_request($data = array()) {

      if(isset($data['f'])) {
         foreach($data['f'] as $f) {
            $v = array_shift($data['v']);

            if($v == '') { continue; }

            if($f == 'from') {
               $v = fix_email_address($v);

               if(substr($v, 0, 1) == '@') { $this->a['from_domain'] .= "|$v"; }
               else if(strstr($v, '@')) {
                  if(in_array($v, $_SESSION['emails'])) { $this->a['o_from'] .= "|$v"; } else { $this->a['f_from'] .= "|$v"; }
               }
               else {
                  $this->a['from'] .= " $v";
               }
            }

            if($f == 'to') {
               $v = fix_email_address($v);

               if(substr($v, 0, 1) == '@') { $this->a['to_domain'] .= "|$v"; }
               else if(strstr($v, '@')) {
                  if(in_array($v, $_SESSION['emails'])) { $this->a['o_to'] .= "|$v"; } else { $this->a['f_to'] .= "|$v"; }
               }
               else {
                  $this->a['to'] .= " $v";
               }
            }


            if($f == 'subject') { $this->a['subject'] .= ' ' . $v; }
            if($f == 'body') { $this->a['body'] .= ' ' . $v; }
         }
      }

      if(isset($data['attachment_type'])) { $this->a['attachment_type'] = $data['attachment_type']; }
      if(isset($data['direction'])) { $this->a['direction'] = $data['direction']; }
      if(isset($data['tag'])) { $this->a['tag'] = $data['tag']; }
      if(isset($data['date1'])) { $this->a['date1'] = $data['date1']; }
      if(isset($data['date2'])) { $this->a['date2'] = $data['date2']; }
   }


}

?>
