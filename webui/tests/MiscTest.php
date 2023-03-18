<?php

use PHPUnit\Framework\TestCase;

require_once dirname(dirname(__FILE__)) . '/config.php';
require_once dirname(dirname(__FILE__)) . '/system/model.php';
require_once dirname(dirname(__FILE__)) . '/system/loader.php';
require_once dirname(dirname(__FILE__)) . '/system/language.php';
require_once dirname(dirname(__FILE__)) . '/system/misc.php';

final class MiscTest extends TestCase
{

   public function providerTestValiddomain() {
      return [
         ['', 0],
         ['local', 1],
         ['@local', 1],
         ['aaa.fu', 1],
         ['@aaa.fu', 0],
         ['AAa.fu', 1],
         ['.aaa.fu', 0],
         ['-aaa.fu', 0],
         ['_aaa.fu', 0],
         ['2aaa.fu', 1],
         ['aaafu', 0],
         ['a.com.', 0],
         ['a.co.uk', 1],
         ['a.co.u2k', 1],
         ['a.com', 1],
         ['ccc.com', 1],
         ['aaa.bbb.ccc.com', 1],
         ['aaa.photography', 1],
      ];
   }

   /**
    * @dataProvider providerTestValiddomain
    */

   public function test_validdomain($data, $expected_result) {
      $result = validdomain($data);
      $this->assertEquals($result, $expected_result);
   }
}
