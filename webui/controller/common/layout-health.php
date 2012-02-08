<?php  

class ControllerCommonLayoutHealth extends Controller {

      protected function index() {


         $this->data['title'] = $this->document->title;

         $this->template = "common/layout-health.tpl";


         $this->children = array(
                      "common/menu",
                      "common/footer"
         );

         $this->render();

      }


}


?>
