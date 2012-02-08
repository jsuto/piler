<?php


class ControllerCommonNotfound extends Controller {

   public function index(){

      $this->id = "content";
      $this->template = "common/not_found.tpl";
      $this->layout = "common/layout";

      $this->document->title = $this->data['title_not_found'];


      $this->render();

   }


}


?>
