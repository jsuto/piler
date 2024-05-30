## Setup

Create a new application with a secret in Azure portal

Grant the following application type permissions:
- Mail.Read
- User.ReadBasic.All


## Update config.py

Edit config.py, and set the values for client_id, client_secret and tenant_id.
Feel free to customise the list of SKIP_FOLDERS. These are IMAP folders that
won't be processed.


## Notes

### Access token claim

```
{
    "aud": "https://graph.microsoft.com",
    "iss": "https://sts.windows.net/63122682-bd96-47fd-97ab-95a76ce699a6/",
    "iat": 1717046428,
    "nbf": 1717046428,
    "exp": 1717050328,
    "aio": "E2NgYNhxO2fe9z25T5iljKc7a+zfDQA=",
    "app_displayname": "downloader",
    "appid": "1671f074-56e2-4c37-873b-b7b07a120b18",
    "appidacr": "1",
    "idp": "https://sts.windows.net/63122682-bd96-47fd-97ab-95a76ce699a6/",
    "idtyp": "app",
    "oid": "72243241-f194-45f3-987d-eeb347b83b69",
    "rh": "0.Aa4AgiYSY5a9_UeXq5WnbOaZpgMAAAAAAAAAwAAAAAAAAACtAAA.",
    "roles": [
        "User.ReadBasic.All",
        "Mail.Read"
    ],
    "sub": "72243241-f194-45f3-987d-eeb347b83b69",
    "tenant_region_scope": "EU",
    "tid": "63122682-bd96-47fd-97ab-95a76ce699a6",
    "uti": "4GOPt79GP0if650O0N7iAA",
    "ver": "1.0",
    "wids": [
        "0997a1d0-0d1d-4acb-b408-d5ca73121e90"
    ],
    "xms_tcdt": 1717004002,
    "xms_tdbr": "EU"
}
```

### User object

```
{
    "businessPhones": [],
    "displayName": "Alex Aaaa",
    "givenName": "Alex",
    "jobTitle": null,
    "mail": "alex@example.com",
    "mobilePhone": null,
    "officeLocation": null,
    "preferredLanguage": null,
    "surname": "Aaaa",
    "userPrincipalName": "alex@example.com",
    "id": "8f7cd945-c816-491b-8c8c-577bd664189a"
}
```

The folder object looks like

```
{
    "id": "AQMkADRmMTJjZTcwLTgzNzQtNGVmYy1iZjVkLTQ1MjYwNWRkADk2ZjcALgAAAwMiLjS4ZfVGk5jrGoS0GuQBAGcxSxRpztdJmjriQ3x-QkAAAAIBDAAAAA==",
    "displayName": "Inbox",
    "parentFolderId": "AQMkADRmMTJjZTcwLTgzNzQtNGVmYy1iZjVkLTQ1MjYwNWRkADk2ZjcALgAAAwMiLjS4ZfVGk5jrGoS0GuQBAGcxSxRpztdJmjriQ3x-QkAAAAIBCAAAAA==",
    "childFolderCount": 0,
    "unreadItemCount": 2,
    "totalItemCount": 2,
    "sizeInBytes": 235372,
    "isHidden": false
}
```
