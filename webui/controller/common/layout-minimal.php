<?php  

class ControllerCommonLayoutMinimal extends Controller {

      protected function index() {


         $this->data['title'] = $this->document->title;

         $this->template = "common/layout-minimal.tpl";


         $this->children = array(
                      "common/menu",
                      "common/footer"
         );

         $this->render();

      }


}


?>
