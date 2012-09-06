<?php


class ControllerSearchSearch extends Controller {

   public function index(){

      $this->id = "content";
      $this->template = "search/search.tpl";
      $this->layout = "common/layout-new";

      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('search/search');

      $this->document->title = $this->data['text_search'];

      $this->data['searchtype'] = 'expert';

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

      if(isset($a['date1'])) { $this->data['date1'] = $a['date1']; }
      if(isset($a['date2'])) { $this->data['date2'] = $a['date2']; }

      if(isset($a['direction'])) { $this->data['direction'] = $a['direction']; }
   }


}

?>
