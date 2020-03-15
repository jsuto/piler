<?php

class ControllerCommonLayoutSearch extends Controller {

      protected function index() {

         $this->data['title'] = $this->document->title;

         if(ENABLE_MOBILE_PREVIEW && MOBILE_DEVICE) {
            $this->template = "common/layout-search-mobile.tpl";
         } else {
            $this->template = "common/layout-search.tpl";
         }

         $this->data['search_args'] = '';

         $this->data['open_saved_search_box'] = 0;

         $this->children = array(
                      "common/menu",
                      "search/folder",
                      "search/popup",
                      "common/footer"
         );

         $this->render();

      }


}
