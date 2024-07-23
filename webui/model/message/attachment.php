<?php

class ModelMessageAttachment extends Model {

   public function get_attachment_by_id($id = 0) {
      if($id <= 0) { return []; }

      $query = $this->db->query("SELECT id, piler_id, attachment_id, name, type, ptr FROM " . TABLE_ATTACHMENT . " WHERE id=?", [$id]);

      if(isset($query->row)) {
         if($query->row['ptr'] > 0) {
            return $this->get_attachment_by_id($query->row['ptr']);
         }

         $metaid = $this->model_search_message->get_id_by_piler_id($query->row['piler_id']);

         if($metaid > 0 && $this->model_search_search->check_your_permission_by_id($metaid) == 1) {
            $attachment = $this->get_attachment_content($query->row['piler_id'], $query->row['attachment_id']);

            return ['filename' => fix_evolution_mime_name_crap($query->row['name']), 'piler_id' => $query->row['piler_id'], 'attachment' => $attachment];
         }
      }

      return [];
   }


   public function get_attachment_content($piler_id = '', $attachment_id = '') {
      $data = '';

      if($piler_id == '' || $attachment_id == '' || !preg_match("/^([0-9a-f]+)$/", $piler_id) || !preg_match("/^([0-9m]+)$/", $attachment_id)) { return $data; }

      $cmd = DECRYPT_ATTACHMENT_BINARY . " $piler_id $attachment_id";

      if(LOG_LEVEL >= DEBUG) { syslog(LOG_INFO, "attachment cmd: $cmd"); }

      $handle = popen($cmd, "r");

      while(($buf = fread($handle, DECRYPT_BUFFER_LENGTH))){
         $data .= $buf;
      }
      pclose($handle);

      /* check if it's a base64 encoded stuff */

      $s = substr($data, 0, 4096);
      $s = preg_replace("/(\r|\n)/", "", $s);

      if(!preg_match("/\s/", $s)) {
         return base64_decode(preg_replace("/\s/", "", $data));
      }

      return $data;
   }


   public function write_image_attachments_to_tmp($attachments, $id) {
      $images = [];

      foreach($attachments as $a) {

         if(preg_match('/image/', $a['type'])) {

            $attachment = $this->get_attachment_by_id($a['id']);

            $fname = 'i.' . $a['id'];

            if(strstr($a['type'], 'svg')) {
               $fname .= '.svg';
            }

            $fp = fopen(DIR_BASE . 'tmp/' . $fname, 'w+');

            if($fp) {
               fwrite($fp, $attachment['attachment']);
               fclose($fp);

               $images[] = ['name' => $fname];
            }
         }
      }

      return $images;
   }


   public function dump_attachment($basedir='', $in_or_out="in", $email='', $id=0, $attachment=[]) {
      if($basedir == '' || $email == '') {
         return;
      }

      $dir = sprintf("%s/%s/%s", $basedir, $email, $in_or_out);

      if(!is_dir($dir)) {
         if(!mkdir($dir, 0700, true)) {
            die("Failed to create folder $dir");
         }
      }

      $fname = sprintf("%s/%d-%s", $dir, $id, $attachment['filename']);
      $fp = fopen($fname, "w+");
      if($fp) {
         fwrite($fp, $attachment['attachment']);
         fclose($fp);
      } else {
         syslog(LOG_INFO, "ERROR: could not write $fname");
      }
   }


   public function get_last_attachment_id() {
      $query = $this->db->query("SELECT id FROM " . TABLE_ATTACHMENT . " ORDER BY id DESC LIMIT 1");

      if(isset($query->row['id'])) {
         return $query->row['id'];
      }

      return 0;
   }


   public function get_checkpoint() {
      $query = $this->db->query("SELECT value FROM `" . TABLE_OPTION . "` WHERE `key`=?", [ATTACHMENT_DUMP_CHECKPOINT]);
      if(isset($query->row['value'])) {
         return $query->row['value'];
      } else {
         $this->db->query("INSERT INTO `" . TABLE_OPTION . "` (`key`, value) VALUES(?,0)", [ATTACHMENT_DUMP_CHECKPOINT]);
         return 1;
      }
   }


   public function update_checkpoint($value=0) {
      $this->db->query("UPDATE `" . TABLE_OPTION . "` SET value=? WHERE `key`=?", [$value, ATTACHMENT_DUMP_CHECKPOINT]);
   }
}
