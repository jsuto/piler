<?php

use PHPUnit\Framework\TestCase;

include_once("webui/system/model.php");
include_once("webui/model/health/health.php");

final class FormatTest extends TestCase
{

   public function providerTestTimeFormatValues(){
      return [
         ['0', '0.00 ms'],
         ['15', '15.00 sec'],
         ['0.87', '870.00 ms']
      ];
   }

   /**
    * @dataProvider providerTestTimeFormatValues
    */

   public function test_format_time_1($timeval, $expected_result) {
      $result = ModelHealthHealth::format_time($timeval);
      $this->assertEquals($result, $expected_result);
   }

}
