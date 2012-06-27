<?php  

class ControllerCommonLayoutemail extends Controller {

      protected function index() {

         $this->template = "common/layout-email.tpl";

         $this->render();
      }


}


?>
