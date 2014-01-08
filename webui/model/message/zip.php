<?php

class ModelMessageZIP extends Model {


   public function download_attachments_as_zip($piler_id = '') {

      $zip = new ZipArchive();
      $pid = array();

      $randomid = generate_random_string(16);

      $filename = DIR_BASE . "tmp/" . $randomid;

      if($zip->open($filename, ZIPARCHIVE::CREATE) != true) { exit("cannot open <$filename>\n"); }

      $attachments = $this->model_search_message->get_attachment_list($piler_id);

      foreach($attachments as $a) {
         $attachment = $this->model_search_message->get_attachment_by_id($a['id']);
         $fp = fopen(DIR_BASE . 'tmp/' . $a['id'], "w+");
         if($fp) {
            fwrite($fp, $attachment['attachment']);
            fclose($fp);

            $zip->addFile(DIR_BASE . 'tmp/' . $a['id'], $attachment['filename']);
         }
      }

      $zip->close();


      foreach($attachments as $a) {
         unlink(DIR_BASE . 'tmp/' . $a['id']);
      }



      header("Cache-Control: public, must-revalidate");
      header("Pragma: no-cache");
      header("Content-Type: application/zip");
      header("Expires: 0");
      header("Content-Length: " . filesize($filename));
      header("Content-Disposition: attachment; filename=" . $piler_id . ".zip");
      header("Content-Transfer-Encoding: binary\n");

      readfile($filename);

      unlink($filename);
   }


}

?>
