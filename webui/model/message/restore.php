<?php

class ModelMessageRestore extends Model {


   public function download_files_as_zip($idlist = array()) {

      $zip = new ZipArchive();

      $randomid = generate_random_string(16);

      $filename = DIR_TMP . "/" . $randomid;

      if($zip->open($filename, ZIPARCHIVE::CREATE) != true) { exit("cannot open <$filename>\n"); }

      foreach($idlist as $id) {
         $rawemail = $this->model_search_message->get_raw_message($id);
         $zip->addFromString($id . ".eml",  $rawemail);

         AUDIT(ACTION_DOWNLOAD_MESSAGE, '', '', $id, '');
      }


      $zip->close();


      header("Cache-Control: public, must-revalidate");
      header("Pragma: no-cache");
      header("Content-Type: application/zip");
      header("Expires: 0");
      header("Content-Length: " . filesize($filename));
      header("Content-Disposition: attachment; filename=archive-$randomid.zip");
      header("Content-Transfer-Encoding: binary\n");

      readfile($filename);

      unlink($filename);

   }


}


?>
