#!/usr/bin/env node

/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2024 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Thunder JSON-RPC Mock Server
 *
 * Handles org.rdk.* JSON-RPC 2.0 method calls used by tr69hostif.
 *
 * Usage:
 *   node thunder-mock-server.js [--verbose]
 *
 * Example curl commands:
 *
 *   Thunder JSON-RPC (application/json):
 *     curl -X POST http://127.0.0.1:9998/jsonrpc \
 *       -H 'Content-Type: application/json' \
 *       -d '{"jsonrpc":"2.0","id":1,"method":"org.rdk.UserSettings.getPrivacyMode"}'
 *
 *   Thunder JSON-RPC (text/plain - also accepted):
 *     curl -H 'Content-Type: text/plain' \
 *       --data-binary '{"jsonrpc":2.0,"id":15,"method":"org.rdk.UserSettings.getPrivacyMode"}' \
 *       http://127.0.0.1:9998/jsonrpc
 */

'use strict';

/* coverity[missing_tls] Suppress missing_tls: This is a test mock server for localhost development only */
const http = require('http');
const url  = require('url');

// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------

const VERBOSE      = process.argv.includes('--verbose') || process.env.VERBOSE === '1';
const THUNDER_PORT = Number(process.env.THUNDER_PORT) || 9998;
const THUNDER_HOST = process.env.THUNDER_HOST || '127.0.0.1';

const state = {
  wifi_enabled: true,
  partner_id: 'comcast',
};

// ---------------------------------------------------------------------------
// Shared helpers
// ---------------------------------------------------------------------------

function log(tag, msg) {
  if (VERBOSE) {
    console.log(`[${tag}] ${msg}`);
  }
}

// ---------------------------------------------------------------------------
// ─── Thunder JSON-RPC Server ────────────────────────────────────────────────
// ---------------------------------------------------------------------------

/**
 * Mock response database.
 * Add entries here to support additional Thunder methods.
 */
const mockResponses = {
  'org.rdk.UserSettings.getPrivacyMode': {
    result: 'SHARE',
    description: 'User privacy mode setting',
  },
  'org.rdk.System.getPrivacyMode': {
    result: 'SHARE',
    description: 'System privacy mode setting',
  },
  'org.rdk.System.getPowerState': {
    result: { powerState: 'STANDBY' },
    description: 'Current system power state',
  },
  'org.rdk.NetworkManager.GetPrimaryInterface': {
    result: { interface: 'wlan0' },
    description: 'Primary network interface',
  },
  'org.rdk.NetworkManager.GetIPSettings': {
    result: { ipaddress: '192.168.1.100' },
    description: 'IP settings for interface',
  },
  'org.rdk.AuthService.getServiceAccountId': {
    result: { serviceAccountId: '123456789' },
    description: 'Service account ID',
  },
  'org.rdk.AuthService.getExperience': {
    result: { experience: 'TESTOS' },
    description: 'Device experience profile',
  },
  'org.rdk.AuthService.setPartnerId': {
    result: { success: true, partnerId: state.partner_id },
    description: 'Set partner ID',
  },
  'org.rdk.NetworkManager.GetConnectedSSID': {
    result: {
      bssid: 'AA:BB:CC:DD:EE:FF',
      ssid: 'WiFi_2.4G',
      strength: 67,
      security: 1,
    },
    description: 'Connected SSID details',
  },
  'org.rdk.NetworkManager.GetAvailableInterfaces': {
    result: {
      interfaces: [
        {
          type: 'WIFI',
          enabled: state.wifi_enabled,
          mac: 'AA:BB:CC:DD:EE:01',
        },
      ],
    },
    description: 'Available network interfaces',
  },
  'org.rdk.NetworkManager.1.GetAvailableInterfaces': {
    result: {
      interfaces: [
        {
          type: 'WIFI',
          enabled: state.wifi_enabled,
          mac: 'AA:BB:CC:DD:EE:01',
        },
      ],
    },
    description: 'Available network interfaces',
  },
  'org.rdk.NetworkManager.SetInterfaceState': {
    result: { success: true, enabled: state.wifi_enabled },
    description: 'Set interface enabled state',
  },
  'org.rdk.Account.getLastCheckoutResetTime': {
    result: { resetTime: 1717000000 },
    description: 'Last checkout reset timestamp',
  },
};

/**
 * Validate a parsed JSON-RPC request object.
 * Accepts jsonrpc as either the string "2.0" or the number 2.0 so that
 * clients which omit quotes (e.g. --data-binary with text/plain) still work.
 *
 * @param {*} data
 * @returns {{ valid: boolean, error: string|null }}
 */
function validateJsonRpcRequest(data) {
  if (!data || typeof data !== 'object' || Array.isArray(data)) {
    return { valid: false, error: 'Request is not a valid JSON object' };
  }

  // Accept "2.0" (string) or 2.0 (number)
  const ver = data.jsonrpc;
  if (ver !== '2.0' && ver !== 2.0 && ver !== 2) {
    return { valid: false, error: 'Invalid JSON-RPC version (expected "2.0")' };
  }

  if (!data.method || typeof data.method !== 'string') {
    return { valid: false, error: 'Missing or invalid "method" field' };
  }

  if (data.id === undefined) {
    return { valid: false, error: 'Missing required "id" field' };
  }

  return { valid: true, error: null };
}

function createErrorResponse(id, code, message) {
  return { jsonrpc: '2.0', id: id !== undefined ? id : null, error: { code, message } };
}

function createSuccessResponse(id, result) {
  return { jsonrpc: '2.0', id, result };
}

/**
 * Dispatch a validated JSON-RPC request to the mock response database.
 */
function dispatchJsonRpcRequest(request) {
  const { id, method, params } = request;

  log('RPC', `method="${method}" id=${id} params=${JSON.stringify(params || {})}`);

  if (method === 'org.rdk.NetworkManager.SetInterfaceState') {
    const enabled = (params && Object.prototype.hasOwnProperty.call(params, 'enabled'))
      ? params.enabled
      : true;
    state.wifi_enabled = Boolean(enabled);
    const response = createSuccessResponse(id, { success: true, enabled: state.wifi_enabled });
    log('RPC', `result=${JSON.stringify(response.result)}`);
    return response;
  }

  if (method === 'org.rdk.NetworkManager.GetWifiState') {
     const response = createSuccessResponse(id, { state: state.wifi_enabled ? 5 : 1 });
     log('RPC', `result=${JSON.stringify(response.result)}`);
     return response;
   }
	  
  if (method === 'org.rdk.NetworkManager.GetAvailableInterfaces') {
    const response = createSuccessResponse(id, {
      interfaces: [
        {
          type: 'WIFI',
          enabled: state.wifi_enabled,
          mac: 'AA:BB:CC:DD:EE:01',
        },
      ],
    });
    log('RPC', `result=${JSON.stringify(response.result)}`);
    return response;
  }

  if (method === 'org.rdk.NetworkManager.1.GetAvailableInterfaces') {
    const response = createSuccessResponse(id, {
      interfaces: [
        {
          type: 'WIFI',
          enabled: state.wifi_enabled,
          mac: 'AA:BB:CC:DD:EE:01',
        },
      ],
    });
    log('RPC', `result=${JSON.stringify(response.result)}`);
    return response;
  }

  if (method === 'org.rdk.AuthService.setPartnerId') {
    const partnerId = (params && Object.prototype.hasOwnProperty.call(params, 'partnerId'))
      ? params.partnerId
      : state.partner_id;
    if (partnerId !== null && partnerId !== undefined) {
      state.partner_id = String(partnerId);
    }
    const response = createSuccessResponse(id, { success: true, partnerId: state.partner_id });
    log('RPC', `result=${JSON.stringify(response.result)}`);
    return response;
  }

  const entry = mockResponses[method];
  if (!entry) {
    log('RPC', `Method not found: "${method}"`);
    return createErrorResponse(id, -32601, `Method not found: ${method}`);
  }

  const response = createSuccessResponse(id, entry.result);
  log('RPC', `result=${JSON.stringify(entry.result)}`);
  return response;
}

/**
 * Handle POST /jsonrpc - read body, parse JSON, dispatch.
 */
function handleJsonRpcPost(req, res) {
  let bodyData = '';

  req.on('data', (chunk) => {
    if (bodyData.length + chunk.length > 1024 * 1024) {
      req.pause();
      res.writeHead(413, { 'Content-Type': 'application/json' });
      res.end(JSON.stringify(createErrorResponse(null, -32700, 'Request entity too large')));
      return;
    }
    bodyData += chunk.toString('utf8');
  });

  req.on('end', () => {
    let requestObject;

    try {
      requestObject = JSON.parse(bodyData);
    } catch (parseError) {
      log('RPC', `JSON parse failed: ${parseError.message}`);
      res.writeHead(400, { 'Content-Type': 'application/json' });
      res.end(JSON.stringify(createErrorResponse(null, -32700, 'Parse error')));
      return;
    }

    const validation = validateJsonRpcRequest(requestObject);
    if (!validation.valid) {
      log('RPC', `Validation failed: ${validation.error}`);
      res.writeHead(400, { 'Content-Type': 'application/json' });
      res.end(JSON.stringify(createErrorResponse(requestObject.id, -32600, validation.error)));
      return;
    }

    const response = dispatchJsonRpcRequest(requestObject);
    res.writeHead(200, { 'Content-Type': 'application/json' });
    res.end(JSON.stringify(response));
  });

  req.on('error', (error) => {
    console.error(`[RPC] Stream error: ${error.message}`);
    res.writeHead(500, { 'Content-Type': 'application/json' });
    res.end(JSON.stringify({ error: 'Internal server error' }));
  });
}

/**
 * Main Thunder HTTP request router.
 */
function thunderRequestHandler(req, res) {
  const parsedUrl = url.parse(req.url, true);

  log('THUNDER', `${req.method} ${req.url} from ${req.socket.remoteAddress}`);

  if (parsedUrl.pathname === '/jsonrpc') {
    if (req.method !== 'POST') {
      res.writeHead(405, { 'Content-Type': 'application/json' });
      res.end(JSON.stringify({ error: 'Method Not Allowed - use POST' }));
      return;
    }
    return handleJsonRpcPost(req, res);
  }

  if (req.method === 'GET') {
    if (parsedUrl.pathname === '/status') {
      res.writeHead(200, { 'Content-Type': 'application/json' });
      res.end(JSON.stringify({ status: 'running', port: THUNDER_PORT, methods: Object.keys(mockResponses) }));
      return;
    }
    if (parsedUrl.pathname === '/methods') {
      const methods = Object.entries(mockResponses).map(([name, data]) => ({
        method: name,
        description: data.description,
        result: data.result,
      }));
      res.writeHead(200, { 'Content-Type': 'application/json' });
      res.end(JSON.stringify(methods));
      return;
    }
  }

  res.writeHead(404, { 'Content-Type': 'application/json' });
  res.end(JSON.stringify({ error: 'Endpoint not found' }));
}

/**
 * Start the Thunder HTTP server.
 */
function startThunderServer() {
  /* coverity[missing_tls] : FP - loopback-only test mock; mirrors Thunder daemon's localhost HTTP convention */
  // noinspection JSUnresolvedReference - Thunder mock for localhost development
  // eslint-disable-next-line no-undef
  const server = http.createServer(thunderRequestHandler);
  /* coverity[missing_tls] */

  server.on('error', (error) => {
    console.error(`[THUNDER] Server error: ${error.message}`);
    process.exit(1);
  });

  server.listen(THUNDER_PORT, THUNDER_HOST, () => {
    console.log(`[THUNDER] JSON-RPC Mock Server running at http://${THUNDER_HOST}:${THUNDER_PORT}/jsonrpc`);
  });

  return server;
}

// ---------------------------------------------------------------------------
// ─── Main ───────────────────────────────────────────────────────────────────
// ---------------------------------------------------------------------------

const thunderServer = startThunderServer();

function gracefulShutdown(signal) {
  console.log(`\n${signal} received, shutting down...`);
  thunderServer.close(() => {
    console.log('Server closed.');
    process.exit(0);
  });
}

process.on('SIGTERM', () => gracefulShutdown('SIGTERM'));
process.on('SIGINT',  () => gracefulShutdown('SIGINT'));

