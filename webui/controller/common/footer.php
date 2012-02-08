<?php  


class ControllerCommonFooter extends Controller {

   protected function index() {

      $this->id = "footer";
      $this->template = "common/footer.tpl";


      $this->render();
   }


}

?>
