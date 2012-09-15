<?php  


class ControllerSearchFolder extends Controller {

   protected function index() {

      $this->id = "folder";
      $this->template = "search/folder.tpl";

      $request = Registry::get('request');
      $db = Registry::get('db');

      $this->load->model('folder/folder');

      $this->data['folders'] = $this->model_folder_folder->get_folders_for_user();
      $this->data['extra_folders'] = $this->model_folder_folder->get_extra_folders_for_user();

      $this->data['folders_by_hier'] = $this->model_folder_folder->get_all_folder_ids_hier($_SESSION['uid']);

      $this->render();
   }


}

?>
