<?php

use PHPUnit\Framework\TestCase;

require_once dirname(dirname(__FILE__)) . '/system/helper/mime.php';


final class SplitMessageTest extends TestCase {

   public function providerTestSplitMessage() {
      return [
         ["From: aaa\r\nTo:bbb\r\nSubject: test\r\n\r\nThis is a test",
          array('from' => 'aaa', 'to' => 'bbb', 'cc' => '', 'date' => '', 'subject' => 'test', 'content-type' => array('type' => 'text/plain')),
          "This is a test"],

         ["From: aaa\r\nSender: alala@aaa\r\nTo:bbb\r\nCC ccc\r\nSubject: test\r\n\r\nThis is a test",
          array('sender' => 'alala@aaa', 'from' => 'aaa', 'to' => 'bbb', 'cc' => '', 'date' => '', 'subject' => 'test', 'content-type' => array('type' => 'text/plain')),
          "This is a test"],

         ["From: aaa\nTo:bbb\nSubject: test\n\nThis is a test",
          array('from' => 'aaa', 'to' => 'bbb', 'cc' => '', 'date' => '', 'subject' => 'test', 'content-type' => array('type' => 'text/plain')),
          "This is a test"],

         ["From: aaa\r\nTo:bbb\r\nSubject: test\r\n\r\n\r\n\r\nThis is a test\nAaa\n",
          array('from' => 'aaa', 'to' => 'bbb', 'cc' => '', 'date' => '', 'subject' => 'test', 'content-type' => array('type' => 'text/plain')),
          "\r\n\r\nThis is a test\r\nAaa\r\n"],

         ["From: aaa\r\nTo:bbb\r\nSubject: test\r\nContent-type: text/html\r\n\r\n\r\nThis is a test\nAaa\n",
          array('from' => 'aaa', 'to' => 'bbb', 'cc' => '', 'date' => '', 'subject' => 'test', 'content-type' => array('type' => 'text/html')),
          "\r\nThis is a test\r\nAaa\r\n"],

         ["From: aaa\nTo:bbb\nSubject: test\nContent-Type: text/plain\n\nThis is a test",
          array('from' => 'aaa', 'to' => 'bbb', 'cc' => '', 'date' => '', 'subject' => 'test', 'content-type' => array('type' => 'text/plain')),
          "This is a test"],

         ["From: aaa\nTo:bbb\nSubject: test\nDate: Sun, 17 Apr 2016 22:40:03 +0800\nDKIM-Signature: v=1; a=rsa-sha1; c=relaxed/relaxed; d=chemoltravel.hu; s=ml;\n\tt=1471888357; bh=A/l/HLQe3HM4Xc4jFxAmhaWVCMU=;\n\th=Date:To:From:Subject:Sender:From:To:Subject:Date;\n\tb=JlEqXiAKBOoT/YyXKTMsXnEphh2J6sXxgNmbKbGybjo3cU1rgQEL0m1h26gl5AaBP\nContent-Type: text/plain\n\nThis is a test",
          array('from' => 'aaa', 'to' => 'bbb', 'cc' => '', 'subject' => 'test', 'date' => 'Sun, 17 Apr 2016 22:40:03 +0800', 'dkim-signature' => 'v=1; a=rsa-sha1; c=relaxed/relaxed; d=chemoltravel.hu; s=ml;  t=1471888357; bh=A/l/HLQe3HM4Xc4jFxAmhaWVCMU=;  h=Date:To:From:Subject:Sender:From:To:Subject:Date;  b=JlEqXiAKBOoT/YyXKTMsXnEphh2J6sXxgNmbKbGybjo3cU1rgQEL0m1h26gl5AaBP', 'content-type' => array('type' => 'text/plain')),
          "This is a test"],

         ["From: aaa\nTo:bbb\nSubject: test\nContent-Type: text/PLAIN\n\nThis is a test",
          array('from' => 'aaa', 'to' => 'bbb', 'cc' => '', 'date' => '', 'subject' => 'test', 'content-type' => array('type' => 'text/plain')),
          "This is a test"],

         ["From: aaa\nTo:bbb\nSubject: test\nContent-Type: text/plain; charset=\"ISO-8859-1\"\n\nThis is a test",
          array('from' => 'aaa', 'to' => 'bbb', 'cc' => '', 'date' => '', 'subject' => 'test', 'content-type' => array('type' => 'text/plain', 'charset' => 'ISO-8859-1')),
          "This is a test"],

         ["From: aaa\nTo:bbb\nSubject: test\nMIME-Version: 1.0\nContent-Type: multipart/alternative; boundary=\"_=_SWIFT_v4_1460476188_145aa333fc0127705a7e904aab6d1957_=_\"\n\nThis is a test",
          array('from' => 'aaa', 'to' => 'bbb', 'cc' => '', 'date' => '', 'subject' => 'test', 'mime-version' => '1.0', 'content-type' => array('type' => 'multipart/alternative', 'boundary' => '_=_SWIFT_v4_1460476188_145aa333fc0127705a7e904aab6d1957_=_')),
          "This is a test"],

         ["From: aaa\nTo:bbb\nSubject: test\nMIME-Version: 1.0\nContent-Type: multipart/alternative;\n boundary=\"_=_SWIFT_v4_1460476188_145aa333fc0127705a7e904aab6d1957_=_\"\n\nThis is a test",
          array('from' => 'aaa', 'to' => 'bbb', 'cc' => '', 'date' => '', 'subject' => 'test', 'mime-version' => '1.0', 'content-type' => array('type' => 'multipart/alternative', 'boundary' => '_=_SWIFT_v4_1460476188_145aa333fc0127705a7e904aab6d1957_=_')),
          "This is a test"],

         ["From: aaa\nTo:bbb\nSubject: test\nMIME-Version: 1.0\nContent-Type: multipart/related;\n\ttype=\"multipart/alternative\";\n\tboundary=\"----=_NextPart_000_0006_01D195BC.69E26510\"\n\nThis is a test",
          array('from' => 'aaa', 'to' => 'bbb', 'cc' => '', 'date' => '', 'subject' => 'test', 'mime-version' => '1.0', 'content-type' => array('type' => 'multipart/alternative', 'boundary' => '----=_NextPart_000_0006_01D195BC.69E26510')),
          "This is a test"],


      ];
   }


   /**
    * @dataProvider providerTestSplitMessage
    */
   public function test_split_message($input, $expected_headers, $expected_body) {
      Piler_Mime_Decode::splitMessage($input, $headers, $body);

      $this->assertEquals($headers, $expected_headers);
      $this->assertEquals($body, $expected_body);
   }


}
