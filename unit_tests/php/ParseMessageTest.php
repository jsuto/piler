<?php

use PHPUnit\Framework\TestCase;

define('DIR_BASE', $_ENV['DIR_BASE']);
define('TEST_FILES_DIR', $_ENV['TEST_FILES_DIR']);

require_once DIR_BASE . 'system/helper/mime.php';


final class MailParserTest extends TestCase {

   public function providerTestParseMessage() {
      return [
         ["1.eml", 1, ["Liebe Gueste,\n\ndie Einarbeitung der Rechen- und Summenfunktionen ins RK-Formular"]],
         ["2.eml", 1, ["Hallo!\nDie seltsamen Zeilenumbr=C3=BCche treten tats=C3=A4chlich auf."]],
         ["3.eml", 1, ["\n\nCan we discuss? Send Reply For more information, THANKS."]],
         ["4.eml", 2, ["=0D=0A=0D=0A=0D=0A=0D=0A", "<HTML><HEAD>=0D=0A<META http-equiv=3D\"Content-Type\" content=3D\"te="]],
         ["5.eml", 2, ["\nHi ,\n\nIf so, stop by and test out our FREE phishing simulator! Find out how", "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1=2E0 Transitional//EN\" \"http://ww=\nw=2Ew3=2Eorg/TR/xhtml1/DTD/xhtml1-transitional=2Edtd\"><html xmlns=3D\"http:/="]],
         ["6.eml", 2, ["RGVhciBTaXJzLA0KDQpHbGFkIHRvIGhlYXIgdGhhdCB5b3UncmUgb24gdGhlIGZpbHRyYXRpb24g", "<html><head><meta http-equiv=3D\"content-type\" content=3D\"text/html; charse="]],
         ["7.eml", 2, ["Mai ajánlat: \n    \n      Exkluzív!", "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">"]],
         ["8.eml", 2, ["Hello,\n\nYou have received a newsletter from Chemol Travel.", "<html xmlns=3D\"http://www.w3.org/1999/xhtml\" xmlns:v=3D\"urn:schemas-micro=\nsoft-com:vml\" xmlns:o=3D\"urn:schemas-microsoft-com:office:office\">"]],
      ];

   }   


   /**
    * @dataProvider providerTestParseMessage
    */
   public function test_parse_message($input, $expected_part_count, $expected_body) {
      $message = file_get_contents(TEST_FILES_DIR . $input);
      Piler_Mime_Decode::ParseMessage($message, $parts);

      $this->assertEquals(count($parts), $expected_part_count);

      for($i=0; $i<count($parts); $i++) {
         $this->assertEquals($expected_body[$i], substr($parts[$i]['body'], 0, strlen($expected_body[$i])));
      }
   }

}
