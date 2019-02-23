<?php

require_once DIR_SYSTEM . 'helper/mime.php';

class ModelMessageRestore extends Model {


   public function download_files_as_zip($idlist = array()) {
      $i = 0;

      $zip = new ZipArchive();

      $randomid = generate_random_string(16);

      $zipname = DIR_BASE . "tmp/" . $randomid;

      if($zip->open($zipname, ZIPARCHIVE::CREATE) != true) { exit("cannot open <$zipname>\n"); }

      foreach($idlist as $id) {
         $i++;

         $filename = $piler_id = $this->model_search_message->get_piler_id_by_id($id);

         if(EML_NAME_BASED_ON_SUBJECT == 1) {
            $filename = $this->model_search_message->get_subject_id_by_id($id);
            $filename = $this->model_search_message->fix_subject($filename) . "-" . $i;
         }

         $rawemail = $this->model_search_message->get_raw_message($piler_id);

         Piler_Mime_Decode::removeJournal($rawemail);

         $zip->addFromString($filename . ".eml",  $rawemail);

         AUDIT(ACTION_DOWNLOAD_MESSAGE, '', '', $id, '');
      }


      $zip->close();


      header("Cache-Control: public, must-revalidate");
      header("Pragma: no-cache");
      header("Content-Type: application/zip");
      header("Expires: 0");
      header("Content-Length: " . filesize($zipname));
      header("Content-Disposition: attachment; filename=archive-$randomid.zip");
      header("Content-Transfer-Encoding: binary\n");

      readfile($zipname);

      unlink($zipname);
   }

}


?>
