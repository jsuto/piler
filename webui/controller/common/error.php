<?php


class ControllerCommonError extends Controller {

   public function index(){

      $this->id = "content";
      $this->template = "common/error.tpl";
      $this->layout = "common/layout";

      $this->document->title = $this->data['text_error'];

      $this->data['errortitle'] = $this->data['text_error'];

      if(isset($_SESSION['error'])){
         $this->data['errorstring'] = $_SESSION['error'];
         unset($_SESSION['error']);
      }
      else {
         $this->data['errorstring'] = "this is the errorstring";
      }


      $this->render();

   }


}


?>
