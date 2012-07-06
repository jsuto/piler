<?php  

class ControllerCommonLayoutemail extends Controller {

      protected function index() {

         $this->data['title'] = $this->document->title;

         $this->template = "common/layout-email.tpl";

         $this->render();
      }


}


?>
