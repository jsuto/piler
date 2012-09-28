<?php
/*
 * Copyright (c) 2010 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

require_once 'service/apiModel.php';
require_once 'service/apiService.php';
require_once 'service/apiServiceRequest.php';


  /**
   * The "text" collection of methods.
   * Typical usage is:
   *  <code>
   *   $freebaseService = new apiFreebaseService(...);
   *   $text = $freebaseService->text;
   *  </code>
   */
  class TextServiceResource extends apiServiceResource {


    /**
     * Returns blob attached to node at specified id as HTML (text.get)
     *
     * @param string $id The id of the item that you want data about
     * @param array $optParams Optional parameters. Valid optional parameters are listed below.
     *
     * @opt_param string maxlength The max number of characters to return. Valid only for 'plain' format.
     * @opt_param string format Sanitizing transformation.
     * @return ContentserviceGet
     */
    public function get($id, $optParams = array()) {
      $params = array('id' => $id);
      $params = array_merge($params, $optParams);
      $data = $this->__call('get', array($params));
      if ($this->useObjects()) {
        return new ContentserviceGet($data);
      } else {
        return $data;
      }
    }
  }


  /**
   * The "mqlread" collection of methods.
   * Typical usage is:
   *  <code>
   *   $freebaseService = new apiFreebaseService(...);
   *   $mqlread = $freebaseService->mqlread;
   *  </code>
   */
  class MqlreadServiceResource extends apiServiceResource {
    /**
     * Performs MQL Queries. (mqlread.mqlread)
     *
     * @param string $query An envelope containing a single MQL query.
     * @param array $optParams Optional parameters. Valid optional parameters are listed below.
     *
     * @opt_param string lang The language of the results - an id of a /type/lang object.
     * @opt_param bool html_escape Whether or not to escape entities.
     * @opt_param string indent How many spaces to indent the json.
     * @opt_param string uniqueness_failure How MQL responds to uniqueness failures.
     * @opt_param string dateline The dateline that you get in a mqlwrite response to ensure consistent results.
     * @opt_param string cursor The mql cursor.
     * @opt_param string callback JS method name for JSONP callbacks.
     * @opt_param bool cost Show the costs or not.
     * @opt_param string as_of_time Run the query as it would've been run at the specified point in time.
     */
    public function mqlread($query, $optParams = array()) {
      $params = array('query' => $query);
      $params = array_merge($params, $optParams);
      $data = $this->__call('mqlread', array($params));
      return $data;
    }

  }

  /**
   * The "image" collection of methods.
   * Typical usage is:
   *  <code>
   *   $freebaseService = new apiFreebaseService(...);
   *   $image = $freebaseService->image;
   *  </code>
   */
  class ImageServiceResource extends apiServiceResource {
    /**
     * Returns the scaled/cropped image attached to a freebase node. (image.image)
     *
     * @param string $id Freebase entity or content id, mid, or guid.
     * @param array $optParams Optional parameters. Valid optional parameters are listed below.
     *
     * @opt_param string maxwidth Maximum width in pixels for resulting image.
     * @opt_param string maxheight Maximum height in pixels for resulting image.
     * @opt_param string fallbackid Use the image associated with this secondary id if no image is associated with the primary id.
     * @opt_param bool pad A boolean specifying whether the resulting image should be padded up to the requested dimensions.
     * @opt_param string mode Method used to scale or crop image.
     */
    public function image($id, $optParams = array()) {
      $params = array('id' => $id);
      $params = array_merge($params, $optParams);
      $data = $this->__call('image', array($params));
      return $data;
    }

  }


/**
 * Service definition for Freebase (v1).
 *
 * <p>
 * Lets you access the Freebase repository of open data.
 * </p>
 *
 * <p>
 * For more information about this service, see the
 * <a href="http://wiki.freebase.com/wiki/New_Freebase_API" target="_blank">API Documentation</a>
 * </p>
 *
 * @author Google, Inc.
 */
class apiFreebaseService extends apiService {
  public $mqlread;
  public $image;
  public $text;
  /**
   * Constructs the internal representation of the Freebase service.
   *
   * @param apiClient apiClient
   */
  public function __construct(apiClient $apiClient) {
    $this->rpcPath = '/rpc';
    $this->restBasePath = '/freebase/v1/';
    $this->version = 'v1';
    $this->serviceName = 'freebase';

    $apiClient->addService($this->serviceName, $this->version);
    $this->text = new TextServiceResource($this, $this->serviceName, 'text', json_decode('{"methods": {"get": {"parameters": {"format": {"default": "plain", "enum": ["html", "plain", "raw"], "location": "query", "type": "string"}, "id": {"repeated": true, "required": true, "type": "string", "location": "path"}, "maxlength": {"format": "uint32", "type": "integer", "location": "query"}}, "id": "freebase.text.get", "httpMethod": "GET", "path": "text{/id*}", "response": {"$ref": "ContentserviceGet"}}}}', true));
    $this->mqlread = new MqlreadServiceResource($this, $this->serviceName, 'mqlread', json_decode('{"httpMethod": "GET", "parameters": {"lang": {"default": "/lang/en", "type": "string", "location": "query"}, "cursor": {"type": "string", "location": "query"}, "indent": {"format": "uint32", "default": "0", "maximum": "10", "location": "query", "type": "integer"}, "uniqueness_failure": {"default": "hard", "enum": ["hard", "soft"], "location": "query", "type": "string"}, "dateline": {"type": "string", "location": "query"}, "html_escape": {"default": "true", "type": "boolean", "location": "query"}, "callback": {"type": "string", "location": "query"}, "cost": {"default": "false", "type": "boolean", "location": "query"}, "query": {"required": true, "type": "string", "location": "query"}, "as_of_time": {"type": "string", "location": "query"}}, "path": "mqlread", "id": "freebase.mqlread"}', true));
    $this->image = new ImageServiceResource($this, $this->serviceName, 'image', json_decode('{"httpMethod": "GET", "parameters": {"maxwidth": {"format": "uint32", "type": "integer", "location": "query", "maximum": "4096"}, "maxheight": {"format": "uint32", "type": "integer", "location": "query", "maximum": "4096"}, "fallbackid": {"default": "/freebase/no_image_png", "type": "string", "location": "query"}, "pad": {"default": "false", "type": "boolean", "location": "query"}, "mode": {"default": "fit", "enum": ["fill", "fillcrop", "fillcropmid", "fit"], "location": "query", "type": "string"}, "id": {"repeated": true, "required": true, "type": "string", "location": "path"}}, "path": "image{/id*}", "id": "freebase.image"}', true));
  }
}

class ContentserviceGet extends apiModel {
  public $result;
  public function setResult($result) {
    $this->result = $result;
  }
  public function getResult() {
    return $this->result;
  }
}
