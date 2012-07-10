<?php


class ControllerGroupEmail extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "group/email.tpl";
      $this->layout = "common/layout-email";


      $request = Registry::get('request');
      $db = Registry::get('db');
      $language = Registry::get('language');

      $this->load->model('group/group');

      //$this->document->title = $language->get('text_group_management');

      $this->data['page'] = 0;
      $this->data['page_len'] = get_page_length();

      $this->data['total'] = 0;

      $this->data['sort'] = 'email';

      $this->data['term'] = '';

      if(!isset($this->request->get['term']) || strlen($this->request->get['term']) < 1) { die("no data"); }

      if(isset($this->request->get['page']) && is_numeric($this->request->get['page']) && $this->request->get['page'] > 0) {
         $this->data['page'] = $this->request->get['page'];
      }

      $this->data['search'] = $this->request->get['term'];

      /* check if we are admin */

      if(Registry::get('admin_user') == 1) {

         // for autocomplete

         if(strlen($this->request->get['term']) >= 2) {
            $emails = $this->model_group_group->get_emails_by_string($this->request->get['term']);

            $i = 0;
            $s = '[ ';

            foreach($emails as $email) {
               $i++;
               $s .= '{ "id": "' . $i . '", "value": "' . $email['email'] . '" },';
            }

            $s = preg_replace("/,$/", "", $s) . " ]";

            print $s;
         }

         // for email list

         if(strlen($this->request->get['term']) == 1) {
            $this->data['emails'] = $this->model_group_group->get_emails_by_string($this->request->get['term'], $this->data['page'], $this->data['page_len']);

            $this->data['total'] = $this->model_group_group->count_emails($this->request->get['term']);

            $this->data['prev_page'] = $this->data['page'] - 1;
            $this->data['next_page'] = $this->data['page'] + 1;

            $this->data['total_pages'] = floor($this->data['total'] / $this->data['page_len']);

            $this->render();
         }

      }
   }


}

?>
