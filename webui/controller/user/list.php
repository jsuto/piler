<?php


class ControllerUserList extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "user/list.tpl";
      $this->layout = "common/layout";


      $request = Registry::get('request');
      $db = Registry::get('db');
      $language = Registry::get('language');

      $this->load->model('user/user');

      $this->document->title = $language->get('text_user_management');


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


      $this->data['sort'] = 'username';

      $this->data['order'] = (int)@$this->request->get['order'];

      if(@$this->request->get['sort'] == "uid") { $this->data['sort'] = "uid"; }
      if(@$this->request->get['sort'] == "realname") { $this->data['sort'] = "realname"; }
      if(@$this->request->get['sort'] == "email") { $this->data['sort'] = "email"; }
      if(@$this->request->get['sort'] == "domain") { $this->data['sort'] = "domain"; }
      if(@$this->request->get['sort'] == "policy") { $this->data['sort'] = "policy_group"; }


      /* check if we are admin */

      if(Registry::get('admin_user') == 1) {

         $users = $this->model_user_user->get_users($this->data['search'], $this->data['page'], $this->data['page_len'], 
                    $this->data['sort'], $this->data['order']);

         $this->data['total_users'] = $this->model_user_user->count_users($this->data['search']);

         foreach ($users as $user) {
            $policy_group = DEFAULT_POLICY;

            $this->data['users'][] = array(
                                          'uid'          => $user['uid'],
                                          'username'     => $user['username'],
                                          'realname'     => $user['realname'],
                                          'email'        => $user['email'],
                                          'shortemail'   => short_email($user['email']),
                                          'domain'       => $user['domain'],
                                          'policy_group' => $policy_group,
                                          'isadmin'       => $user['isadmin']
                                        );
         }

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
