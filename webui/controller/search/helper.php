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
                    'tag'             => ''
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
      } else if($this->request->post['searchtype'] == 'expert'){
         $this->preprocess_post_expert_request($this->request->post);
         $this->fixup_post_request();
         list ($this->data['n'], $this->data['messages']) = $this->model_search_search->search_messages($this->a, ADVANCED_SEARCH, $this->data['page']);
      } else {
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
      while(list($k, $v) = each($this->a)) {
         if($this->a[$k]) { $this->a[$k] = substr($this->a[$k], 1, strlen($this->a[$k])); }
      }

      $this->a['sort'] = $this->request->post['sort'];
      $this->a['order'] = $this->request->post['order'];
   }


   private function preprocess_post_expert_request($data = array()) {
      $token = '';

//print_r($data);

      $s = preg_replace("/:/", ": ", $data['search']);
      $s = preg_replace("/,/", " ", $s);
      $s = preg_replace("/\s{1,}/", " ", $s);
      $b = explode(" ", $s);

      while(list($k, $v) = each($b)) {
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

         if($token == 'from') {
            $v = fix_email_address($v);

            if(!strstr($v, '@')) { $this->a['from_domain'] .= "|$v"; }
            else {
               $this->a['from'] .= "|$v";
               if(in_array($v, $_SESSION['emails'])) { $this->a['o_from'] .= "|$v"; } else { $this->a['f_from'] .= "|$v"; }
            }
         }
         else if($token == 'to') {
            $v = fix_email_address($v);

            if(!strstr($v, '@')) { $this->a['to_domain'] .= "|$v"; }
            else {
               $this->a['to'] .= "|$v";
               if(in_array($v, $_SESSION['emails'])) { $this->a['o_to'] .= "|$v"; } else { $this->a['f_to'] .= "|$v"; }
            }
         }
         else if($token == 'subject') { $this->a['subject'] .= ' ' . $v; }
         else if($token == 'body') { $this->a['body'] .= ' ' . $v; }
         else if($token == 'date1') { $this->a['date1'] = $v; }
         else if($token == 'date2') { $this->a['date2'] = $v; }
         else if($token == 'direction') { $this->a['direction'] = $v; }
         else if($token == 'size') { $this->a['size'] .= ' ' . $v; }
         else if($token == 'attachment_type') { $this->a['attachment_type'] .= ' ' . $v; }
         else if($token == 'tag') { $this->a['tag'] .= ' ' . $v; }
      }

   }


   private function preprocess_post_advanced_request($data = array()) {

      if(isset($data['f'])) {
         foreach($data['f'] as $f) {
            $v = array_shift($data['v']);

            if($v == '') { continue; }

            if($f == 'from') {
               $v = fix_email_address($v);

               if(!strstr($v, '@')) { $this->a['from_domain'] .= "|$v"; }
               else {
                  $this->a['from'] .= "|$v";
                  if(in_array($v, $_SESSION['emails'])) { $this->a['o_from'] .= "|$v"; } else { $this->a['f_from'] .= "|$v"; }
               }
            }

            if($f == 'to') {
               $v = fix_email_address($v);

               if(!strstr($v, '@')) { $this->a['to_domain'] .= "|$v"; }
               else {
                  $this->a['to'] .= "|$v";
                  if(in_array($v, $_SESSION['emails'])) { $this->a['o_to'] .= "|$v"; } else { $this->a['f_to'] .= "|$v"; }
               }
            }


            if($f == 'subject') {
               $this->a['subject'] .= "|$v";
            }


            if($f == 'body') {
               $this->a['body'] .= "|$v";
            }

            if($f == 'tag') {
               $this->a['tag'] .= "|$v";
            }


         }
      }

   }


}

?>
