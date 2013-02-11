<?php  

class ControllerCommonLayoutSearch extends Controller {

      protected function index() {


         $this->data['title'] = $this->document->title;

         $this->template = "common/layout-search.tpl";

         $this->data['search_args'] = '';

         $this->data['open_saved_search_box'] = 0;


         /*if(isset($_SERVER['REQUEST_URI'])) {
            $this->data['search_args'] = preg_replace("/\/([\w]+)\.php\?{0,1}/", "", $_SERVER['REQUEST_URI']);

            if(preg_match("/\&a\=1/", $this->data['search_args'])) { $this->data['open_saved_search_box'] = 1; }
         }*/


         $this->children = array(
                      "common/menu",
                      "search/folder",
                      "search/popup",
                      "common/footer"
         );

         $this->render();

      }


}


?>
