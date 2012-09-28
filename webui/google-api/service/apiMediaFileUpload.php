<?php
/**
 * Copyright 2012 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @author Chirag Shah <chirags@google.com>
 *
 */
class apiMediaFileUpload {
  public $mimeType;
  public $fileName;
  public $chunkSize;

  public static function process($metadata, $method, &$params) {
    $payload = array();

    $data = isset($params['data']) ? $params['data']['value'] : false;
    $mimeType = isset($params['mimeType']) ? $params['mimeType']['value'] : false;
    $file = isset($params['file']) ? $params['file']['value'] : false;
    $uploadPath = $method['mediaUpload']['protocols']['simple']['path'];

    unset($params['data']);
    unset($params['mimeType']);
    unset($params['file']);

    if ($file) {
      if (substr($file, 0, 1) != '@') {
        $file = '@' . $file;
      }
      $payload['file'] = $file;
      $payload['content-type'] = 'multipart/form-data';
      $payload['restBasePath'] = $uploadPath;

      // This is a standard file upload with curl.
      return $payload;
    }

    $parsedMeta = is_string($metadata) ? json_decode($metadata, true) : $metadata;
    if ($metadata && false == $data) {
      // Process as a normal API request.
      return false;
    }

    // Process as a media upload request.
    $params['uploadType'] = array(
        'type' => 'string',
        'location' => 'query',
        'value' => 'media',
    );

    // Determine which type.
    $payload['restBasePath'] = $uploadPath;
    if (false == $metadata || false == $parsedMeta) {
      // This is a simple media upload.
      $payload['content-type'] = $mimeType;
      $payload['data'] = $data;
    } else {
      // This is a multipart/related upload.
      $boundary = isset($params['boundary']) ? $params['boundary'] : mt_rand();
      $boundary = str_replace('"', '', $boundary);
      $payload['content-type'] = 'multipart/related; boundary=' . $boundary;

      $related = "--$boundary\r\n";
      $related .= "Content-Type: application/json; charset=UTF-8\r\n";
      $related .= "\r\n" . $metadata . "\r\n";
      $related .= "--$boundary\r\n";
      $related .= "Content-Type: $mimeType\r\n";
      $related .= "Content-Transfer-Encoding: base64\r\n";
      $related .= "\r\n" . base64_encode($data) . "\r\n";
      $related .= "--$boundary--";
      $payload['data'] = $related;
    }

    return $payload;
  }
}