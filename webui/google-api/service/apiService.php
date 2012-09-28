<?php
/*
 * Copyright 2010 Google Inc.
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

require_once 'service/apiServiceResource.php';
require_once 'service/apiServiceRequest.php';
require_once 'service/apiBatch.php';

/**
 * This class parses the service end points of the api discovery document and constructs
 * serviceResource variables for all of them.
 *
 * For instance when calling with the service document for Plus, it will create apiServiceResource's
 * for $this->activities, $this->comments, $this->people, etc.
 *
 * @author Chris Chabot <chabotc@google.com>
 *
 */
class apiService {
  public $version = null;
  public $restBasePath;
  public $rpcPath;
  public $resource = null;

  public function __construct($serviceName, $discoveryDocument) {
    global $apiConfig;
    if (!isset($discoveryDocument['version']) || !isset($discoveryDocument['restBasePath']) || !isset($discoveryDocument['rpcPath'])) {
      throw new apiServiceException("Invalid discovery document");
    }
    $this->version = $discoveryDocument['version'];
    $this->restBasePath = $apiConfig['basePath'] . $discoveryDocument['restBasePath'];
    $this->rpcPath = $apiConfig['basePath'] . $discoveryDocument['rpcPath'];
    foreach ($discoveryDocument['resources'] as $resourceName => $resourceTypes) {
      $this->$resourceName = new apiServiceResource($this, $serviceName, $resourceName, $resourceTypes);
    }
  }

  /**
   * @return string $restBasePath
   */
  public function getRestBasePath() {
    return $this->restBasePath;
  }

  /**
   * @return string $rpcPath
   */
  public function getRpcPath() {
    return $this->rpcPath;
  }
}
