<?php  


class ControllerSearchFolder extends Controller {

   protected function index() {

      $this->id = "folder";
      $this->template = "search/folder.tpl";

      $session = Registry::get('session');
      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('folder/folder');

      $this->data['extra_folders'] = $this->model_folder_folder->get_folders_for_user();
      array_unshift($this->data['extra_folders'], array('id' => 0, 'name' => '---'));

      $this->data['folders_by_hier'] = $this->model_folder_folder->get_all_folder_ids_hier($session->get("uid"));

      $this->render();
   }


}

?>
