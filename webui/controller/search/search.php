<?php


class ControllerSearchSearch extends Controller {

   public function index(){

      $this->id = "content";
      $this->template = "search/search.tpl";
      $this->layout = "common/layout-search";

      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('search/search');

      $this->data['searchtype'] = 'simple';

      if(isset($this->request->get['type'])) {
         if($this->request->get['type'] == 'advanced') {
            $this->template = "search/advanced.tpl";
            $this->data['searchtype'] = 'advanced';
         }

         if($this->request->get['type'] == 'expert') {
            $this->template = "search/expert.tpl";
            $this->data['searchtype'] = 'expert';
         }

      }

      if(isset($this->request->post['searchterm'])) {
         $this->fixup_post_request();

         $a = preg_replace("/\&loaded=1$/", "", $this->request->post['searchterm']);
         $this->model_search_search->update_search_term($a);
      }

      $this->render();
   }


   private function fixup_post_request() {
      $i = 0;
      $a = array();

      $this->data['blocks'] = array();

      $this->data['searchterm'] = $this->request->post['searchterm'];

      parse_str($this->request->post['searchterm'], $a);

      if(isset($a['from'])) { $this->data['from'] = $a['from']; }
      if(isset($a['to'])) { $this->data['to'] = $a['to']; }
      if(isset($a['subject'])) { $this->data['subject'] = $a['subject']; }

      if(isset($a['search'])) { $this->data['_search'] = $a['search']; }

      if(isset($a['sort'])) { $this->data['sort'] = $a['sort']; }
      if(isset($a['order'])) { $this->data['order'] = $a['order']; }

      if(isset($a['f'])) {
       foreach($a['f'] as $f) {
         $val = array_shift($a['v']);

         if($val == '') { continue; }

         if($i == 0) {
            $this->data['key0'] = 0;

            if($f == 'from') { $this->data['key0'] = 0; }
            else if($f == 'to') { $this->data['key0'] = 1; }
            else if($f == 'subject') { $this->data['key0'] = 2; }
            else if($f == 'body') { $this->data['key0'] = 3; }

            $this->data['val0'] = $val;
         }
         else {
            array_push($this->data['blocks'], array('key' => $f, 'val' => $val));
         }

         $i++;
       }
      }

      if(isset($a['date1'])) { $this->data['date1'] = $a['date1']; }
      if(isset($a['date2'])) { $this->data['date2'] = $a['date2']; }

      if(isset($a['direction'])) { $this->data['direction'] = $a['direction']; }
   }


}

?>
