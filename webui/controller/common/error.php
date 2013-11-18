<?php


class ControllerCommonError extends Controller {

   public function index(){

      $this->id = "content";
      $this->template = "common/error.tpl";
      $this->layout = "common/layout";

      $session = Registry::get('session');

      $this->document->title = $this->data['text_error'];

      $this->data['errortitle'] = $this->data['text_error'];

      if($session->get("error")) {
         $this->data['errorstring'] = $session->get("error");
         $session->set("error", "");
      }
      else {
         $this->data['errorstring'] = "this is the errorstring";
      }


      $this->render();

   }


}


?>
