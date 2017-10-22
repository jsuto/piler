<?php

use PHPUnit\Framework\TestCase;

define('DIR_SYSTEM', dirname(dirname(__FILE__)) . '/webui/system/');

include_once("webui/system/model.php");
include_once("webui/model/search/message.php");

final class SearchMessageTest extends TestCase {

   public function providerTestNiceSizeValues() {
      return [
         ['0', '1k'],
         ['1000', '1k'],
         ['2000', '2k'],
         ['4300', '4k'],
         ['4700', '5k'],
         ['3000000', '3.0M'],
         ['4240000', '4.2M'],
         ['6810000', '6.8M']
      ]; 
   }

   /**
    * @dataProvider providerTestNiceSizeValues
    */

   public function test_nice_size($input, $expected_result) {
      $result = ModelSearchMessage::NiceSize($input);
      $this->assertEquals($result, $expected_result);
   }


}
