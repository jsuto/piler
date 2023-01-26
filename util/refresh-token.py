#!/usr/bin/python3

# Based on the https://github.com/UvA-FNWI/M365-IMAP project

from msal import ConfidentialClientApplication, SerializableTokenCache
import sys


ClientId = "c6843299-05c4-4c2e-9398-64dd42f14b6d"  # Fix this value only
ClientSecret = ""
Scopes = ['https://outlook.office.com/IMAP.AccessAsUser.All']
AccessTokenFileName = "access_token"
RefreshTokenFileName = "refresh_token"


cache = SerializableTokenCache()
app = ConfidentialClientApplication(ClientId,
                                    client_credential=ClientSecret,
                                    token_cache=cache, authority=None)

old_refresh_token = open(RefreshTokenFileName, 'r').read()

token = app.acquire_token_by_refresh_token(old_refresh_token, Scopes)


if 'error' in token:
    print(token)
    sys.exit("Failed to get access token")

with open(AccessTokenFileName, 'w') as f:
    f.write(token['access_token'])

with open(RefreshTokenFileName, 'w') as f:
    f.write(token['refresh_token'])
