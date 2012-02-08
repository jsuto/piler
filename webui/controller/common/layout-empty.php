<?php  

class ControllerCommonLayoutempty extends Controller {

      protected function index() {

         $this->template = "common/layout-empty.tpl";

         $this->render();
      }


}


?>
