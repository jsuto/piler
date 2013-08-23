<?php


class ControllerImportTest extends Controller {
   private $error = array();

   public function index(){

      $this->id = "content";
      $this->template = "import/list.tpl";
      $this->layout = "common/layout";

      require_once 'Zend/Mail/Protocol/Imap.php';
      require_once 'Zend/Mail/Protocol/Pop3.php';

      $request = Registry::get('request');
      $db = Registry::get('db');
      $lang = Registry::get('language');

      if($this->request->post['type'] == 'pop3') {

         try {

            $conn = new Zend_Mail_Protocol_Pop3($this->request->post['server'], '110', false);

         } catch (Zend_Mail_Protocol_Exception $e) {
             print "<span class=\"text-error\">" . $this->request->post['server'] . ": " . $lang->data['text_connection_failed'] . "</span> ";
         }

         if($conn) {
            $s = $conn->connect($this->request->post['server']);

            if($s) {

               try {
                  $conn->login($this->request->post['username'], $this->request->post['password']);
                  print "<span class=\"text-success\">" . $lang->data['text_connection_ok'] . "</span> ";
               }
               catch (Zend_Mail_Protocol_Exception $e) {
                  print "<span class=\"text-error\">" . $this->request->post['username'] . ": " . $lang->data['text_login_failed'] . "</span> ";

               }
            }

         }


      }

      else {
         print "<span class=\"text-error\">" . $lang->data['text_error'] . "</span> ";
      }

   }


}

?>
