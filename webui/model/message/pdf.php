<?php

class ModelMessagePDF extends Model {


   public function download_files_as_zip($idlist = array()) {

      $zip = new ZipArchive();
      $pid = array();

      $randomid = generate_random_string(16);

      $filename = DIR_BASE . "tmp/" . $randomid;

      if($zip->open($filename, ZIPARCHIVE::CREATE) != true) { exit("cannot open <$filename>\n"); }

      $imgs = array();

      foreach($idlist as $id) {
         $piler_id = $this->model_search_message->get_piler_id_by_id($id);
         array_push($pid, $piler_id);


         $attachments = $this->model_search_message->get_attachment_list($piler_id);

         $images = array();

         foreach($attachments as $a) {
            if(preg_match("/image/", $a['type'])) {
               $attachment = $this->model_search_message->get_attachment_by_id($a['id']);
               $fp = fopen(DIR_BASE . 'tmp/' . $a['id'], "w+");
               if($fp) {
                  fwrite($fp, $attachment['attachment']);
                  fclose($fp);

                  $images[] = array('id' => $a['id'], 'name' => $attachment['filename']);

                  $imgs[] = array('name' => $a['id']);
               }
            }
         }



         $message = $this->model_search_message->extract_message($piler_id);

         $page  = "From: " . $message['from'] . "<br />\n";
         $page .= "To: " . $message['to'] . "<br />\n";
         $page .= "Subject: " . $message['subject'] . "<br />\n";
         $page .= "Date: " . $message['date'] . "<br />\n";
         $page .= "<hr />\n" . $message['message'];

         $this->create_pdf_from_eml($piler_id, $page, $images);

         foreach($imgs as $img) {
            unlink(DIR_BASE . 'tmp/' . $img['name']);
         }

         $zip->addFile(DIR_BASE . 'tmp/' . $piler_id . '.pdf', $piler_id . '.pdf');

         AUDIT(ACTION_DOWNLOAD_MESSAGE, '', '', $id, '');
      }


      $zip->close();


      foreach($pid as $piler_id) {
         unlink(DIR_BASE . 'tmp/' . $piler_id . '.pdf');
      }

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



   public function create_pdf_from_eml($piler_id = '', $text = '', $images = array()) {

      $pdf = new TCPDF(PDF_PAGE_ORIENTATION, PDF_UNIT, PDF_PAGE_FORMAT, true, 'UTF-8', false);

      $pdf->SetAuthor('Yourcompany Ltd');
      $pdf->SetTitle('EML email title');
      $pdf->SetSubject('EML email subject');
      //$pdf->SetKeywords('TCPDF, PDF, example, test, guide');

      // set default header data
      //$pdf->SetHeaderData(PDF_HEADER_LOGO, PDF_HEADER_LOGO_WIDTH, PDF_HEADER_TITLE.' 001', PDF_HEADER_STRING, array(0,190,255), array(0,64,128));
      //$pdf->setFooterData($tc=array(0,64,0), $lc=array(0,64,128));

      $pdf->setHeaderFont(Array(PDF_FONT_NAME_MAIN, '', PDF_FONT_SIZE_MAIN));
      $pdf->setFooterFont(Array(PDF_FONT_NAME_DATA, '', PDF_FONT_SIZE_DATA));

      $pdf->SetDefaultMonospacedFont(PDF_FONT_MONOSPACED);

      //$pdf->SetMargins(PDF_MARGIN_LEFT, PDF_MARGIN_TOP, PDF_MARGIN_RIGHT);
      //$pdf->SetMargins(10, 25, 10);
      $pdf->SetMargins(10, 5, 10);

      //$pdf->SetHeaderMargin(120);
      //$pdf->SetFooterMargin(PDF_MARGIN_FOOTER);

      $pdf->setPrintHeader(false);
      $pdf->setPrintFooter(false);


      //set auto page breaks
      //$pdf->SetAutoPageBreak(TRUE, PDF_MARGIN_BOTTOM);
      $pdf->SetAutoPageBreak(TRUE, 20);

      //set image scale factor
      //$pdf->setImageScale(PDF_IMAGE_SCALE_RATIO);

      //set some language-dependent strings
      //$pdf->setLanguageArray($l);


      // set default font subsetting mode
      $pdf->setFontSubsetting(true);

      // Set font
      // dejavusans is a UTF-8 Unicode font, if you only need to
      // print standard ASCII chars, you can use core fonts like
      // helvetica or times to reduce file size.
      $pdf->SetFont('dejavusans', '', 8, '', true);

      $pdf->AddPage();

      $pdf->writeHTMLCell($w=0, $h=0, $x='', $y='', $text, $border=0, $ln=1, $fill=0, $reseth=true, 'L', $autopadding=true);

      $i = 0;

      foreach($images as $img) {
         if($i > 0) { $pdf->AddPage(); }

         $pdf->Image(DIR_BASE . 'tmp/' . $img['id']);

         $i++;
      }

      $pdf->Output(DIR_BASE . 'tmp/' . $piler_id . '.pdf', 'F');
   }


}


?>
