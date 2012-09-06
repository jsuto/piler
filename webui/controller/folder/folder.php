<?php


class ControllerFolderFolder extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "user/list.tpl";
      $this->layout = "common/layout-empty";


      $request = Registry::get('request');
      $db = Registry::get('db');
      $language = Registry::get('language');

      $this->load->model('folder/folder');


      $this->data['term'] = '';

      if(!isset($this->request->get['term']) || strlen($this->request->get['term']) < 2) { die("no data"); }


      /* check if we are admin */

      if(Registry::get('admin_user') == 1) {
         $results = $this->model_folder_folder->get_folders_by_string($this->request->get['term']);

         $i = 0;
         $s = '[ ';

         foreach($results as $result) {
            $i++;
            $s .= '{ "id": "' . $i . '", "value": "' . $result['name'] . '" },';
         }

         $s = preg_replace("/,$/", "", $s) . " ]";

         print $s;
      }
   }


}

?>
