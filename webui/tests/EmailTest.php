<?php

use PHPUnit\Framework\TestCase;

require_once dirname(dirname(__FILE__)) . '/config.php';
require_once dirname(dirname(__FILE__)) . '/system/model.php';
require_once dirname(dirname(__FILE__)) . '/system/loader.php';
require_once dirname(dirname(__FILE__)) . '/system/language.php';
require_once dirname(dirname(__FILE__)) . '/system/misc.php';

final class SearchSearchTest extends TestCase {

   public function providerTestFixEmailAddressForSphinx() {
      return [
         ['aaa@aaa.fu', 'aaaXaaaXfu'],
         ['list-507327664@mail.aaa.fu', 'listX507327664XmailXaaaXfu'],
         ['aaa+bbb@aaa.fu', 'aaaXbbbXaaaXfu'],
         ['ahahah_aiai@aaa.fu', 'ahahahXaiaiXaaaXfu'],
         ['aaa|@bbb@ccc.fu', 'aaa|bbbXcccXfu']
      ];
   }


   /**
    * @dataProvider providerTestFixEmailAddressForSphinx
    */

   public function test_get_boundary($input, $expected_result) {
      $loader = new Loader();
      Registry::set('load', $loader);
      $language = new Language();
      Registry::set('language', $language);

      $loader->load->model('search/search');
      $m = new ModelSearchSearch();

      $result = $m->fix_email_address_for_sphinx($input);
      $this->assertEquals($result, $expected_result);
   }


}
