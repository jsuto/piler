<?php


class ControllerDomainDomains extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "domain/domains.tpl";
      $this->layout = "common/layout-email";


      $request = Registry::get('request');
      $db = Registry::get('db');
      $language = Registry::get('language');

      $this->load->model('domain/domain');

      //$this->document->title = $language->get('text_group_management');

      $this->data['page'] = 0;
      $this->data['page_len'] = get_page_length();

      $this->data['total'] = 0;

      $this->data['sort'] = 'domain';

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
            $domains = $this->model_domain_domain->get_domains_by_string($this->request->get['term']);

            $i = 0;
            $s = '[ ';

            foreach($domains as $domain) {
               $i++;
               $s .= '{ "id": "' . $i . '", "value": "' . $domain['domain'] . '" },';
            }

            $s = preg_replace("/,$/", "", $s) . " ]";

            print $s;
         }

         // for domain list

         if(strlen($this->request->get['term']) == 1) {
            $this->data['domains'] = $this->model_domain_domain->get_domains_by_string($this->request->get['term'], $this->data['page'], $this->data['page_len']);

            $this->data['total'] = $this->model_domain_domain->count_domains($this->request->get['term']);

            $this->data['prev_page'] = $this->data['page'] - 1;
            $this->data['next_page'] = $this->data['page'] + 1;

            $this->data['total_pages'] = floor($this->data['total'] / $this->data['page_len']);

            $this->render();
         }

      }
   }


}

?>
