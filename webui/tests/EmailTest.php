<?php

use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\TestCase;

require_once dirname(dirname(__FILE__)) . '/config.php';
require_once dirname(dirname(__FILE__)) . '/system/model.php';
require_once dirname(dirname(__FILE__)) . '/system/loader.php';
require_once dirname(dirname(__FILE__)) . '/system/language.php';
require_once dirname(dirname(__FILE__)) . '/system/misc.php';

final class EmailTest extends TestCase {

   private function getSearchModel() {
      $loader = new Loader();
      Registry::set('load', $loader);
      $language = new Language();
      Registry::set('language', $language);

      $loader->load->model('search/search');
      return new ModelSearchSearch();
   }

   public static function providerTestFixEmailAddressForSphinx() {
      return [
         ['aaa@aaa.fu', 'aaaXaaaXfu'],
         ['list-507327664@mail.aaa.fu', 'listX507327664XmailXaaaXfu'],
         ['aaa+bbb@aaa.fu', 'aaaXbbbXaaaXfu'],
         ['ahahah_aiai@aaa.fu', 'ahahahXaiaiXaaaXfu'],
         ['aaa|@bbb@ccc.fu', 'aaa|bbbXcccXfu']
      ];
   }


   #[DataProvider('providerTestFixEmailAddressForSphinx')]
   public function test_get_boundary($input, $expected_result) {
      $m = $this->getSearchModel();

      $result = $m->fix_email_address_for_sphinx($input);
      $this->assertEquals($result, $expected_result);
   }


   public static function providerTestMessageIdSearchVariants() {
      return [
         ['', []],
         ['<abc@example.com>', ['<abc@example.com>', 'abc@example.com']],
         ['abc@example.com', ['abc@example.com', '<abc@example.com>']]
      ];
   }


   #[DataProvider('providerTestMessageIdSearchVariants')]
   public function test_message_id_search_variants($input, $expected_result) {
      $m = $this->getSearchModel();

      $result = $m->get_message_id_search_variants($input);
      $this->assertEquals($expected_result, $result);
   }


   public function test_preprocess_message_id_expert_request() {
      $m = $this->getSearchModel();

      $result = $m->preprocess_post_expert_request([
         'search' => 'message-id:<abc@example.com> from:sender@example.com',
         'sort' => 'date',
         'order' => 0
      ]);

      $this->assertEquals('<abc@example.com>', trim($result['message_id']));
      $this->assertContains(FROM_TOKEN, $result['match']);
      $this->assertContains('sender@example.com', $result['match']);
   }


}
