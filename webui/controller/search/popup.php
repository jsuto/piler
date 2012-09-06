<?php  


class ControllerSearchPopup extends Controller {

   protected function index() {

      $this->id = "popup";
      $this->template = "search/popup.tpl";

      $this->render();
   }


}

?>
