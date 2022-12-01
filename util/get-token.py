#!/usr/bin/python3

# Based on the https://github.com/UvA-FNWI/M365-IMAP project

from msal import ConfidentialClientApplication, SerializableTokenCache
import http.server
import os
import sys
import threading
import urllib.parse


redirect_uri = "http://localhost:8745"

ClientId = "c6843299-05c4-4c2e-9398-64dd42f14b6d"  # Fix this value only
ClientSecret = ""
Scopes = ['https://outlook.office.com/IMAP.AccessAsUser.All']
AccessTokenFileName = "access_token"


cache = SerializableTokenCache()
app = ConfidentialClientApplication(ClientId,
                                    client_credential=ClientSecret,
                                    token_cache=cache, authority=None)

url = app.get_authorization_request_url(Scopes, redirect_uri=redirect_uri)

print("Navigate to the following url in a web browser:\n", url)


class Handler(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        parsed_url = urllib.parse.urlparse(self.path)
        parsed_query = urllib.parse.parse_qs(parsed_url.query)
        global code
        code = next(iter(parsed_query['code']), '')

        response_body = b'Success. Look back at your terminal.\r\n'
        self.send_response(200)
        self.send_header('Content-Type', 'text/plain')
        self.send_header('Content-Length', len(response_body))
        self.end_headers()
        self.wfile.write(response_body)

        global httpd
        t = threading.Thread(target=lambda: httpd.shutdown())
        t.start()


code = ''

server_address = ('', 8745)
httpd = http.server.HTTPServer(server_address, Handler)

if not os.getenv('SSH_CONNECTION'):
    httpd.serve_forever()

if code == '':
    print('After login, you will be redirected to a blank (or error) page ' +
          'with a url containing an access code. Paste the url below.')
    resp = input('Response url: ')

    i = resp.find('code') + 5
    code = resp[i: resp.find('&', i)] if i > 4 else resp

token = app.acquire_token_by_authorization_code(code, Scopes,
                                                redirect_uri=redirect_uri)

if 'error' in token:
    print(token)
    sys.exit("Failed to get access token")

# print(token['refresh_token'])

with open(AccessTokenFileName, 'w') as f:
    f.write(token['access_token'])
