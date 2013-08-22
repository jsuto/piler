<?php


class ControllerGroupList extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "group/list.tpl";
      $this->layout = "common/layout";


      $request = Registry::get('request');
      $db = Registry::get('db');
      $language = Registry::get('language');

      $this->load->model('group/group');
      $this->load->model('user/auth');

      $this->document->title = $language->get('text_group_management');


      $this->data['page'] = 0;
      $this->data['page_len'] = get_page_length();

      $this->data['total_users'] = 0;

      $users = array();


      /* get search term if there's any */

      if($this->request->server['REQUEST_METHOD'] == 'POST'){
         $this->data['search'] = @$this->request->post['search'];
      }
      else {
         $this->data['search'] = @$this->request->get['search'];
      }

      /* get page */

      if(isset($this->request->get['page']) && is_numeric($this->request->get['page']) && $this->request->get['page'] > 0) {
         $this->data['page'] = $this->request->get['page'];
      }


      $this->data['sort'] = 'groupname';

      $this->data['order'] = (int)@$this->request->get['order'];

      if(@$this->request->get['sort'] == "uid") { $this->data['sort'] = "uid"; }
      if(@$this->request->get['sort'] == "realname") { $this->data['sort'] = "realname"; }
      if(@$this->request->get['sort'] == "email") { $this->data['sort'] = "email"; }
      if(@$this->request->get['sort'] == "domain") { $this->data['sort'] = "domain"; }
      if(@$this->request->get['sort'] == "policy") { $this->data['sort'] = "policy_group"; }


      /* check if we are admin */

      if(Registry::get('admin_user') == 1) {

         $this->data['groups'] = $this->model_group_group->get_groups($this->data['search'], $this->data['page'], $this->data['page_len'], 
                    $this->data['sort'], $this->data['order']);

         $this->data['total_groups'] = $this->model_group_group->count_groups($this->data['search']);
      }
      else {
         $this->template = "common/error.tpl";
         $this->data['errorstring'] = $this->data['text_you_are_not_admin'];
      }


      $this->data['prev_page'] = $this->data['page'] - 1;
      $this->data['next_page'] = $this->data['page'] + 1;

      $this->data['total_pages'] = floor($this->data['total_users'] / $this->data['page_len']);


      $this->render();
   }


}

?>
