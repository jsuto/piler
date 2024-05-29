#!/usr/bin/python3
# -*- coding: utf-8 -*-

import json
import requests
import time
import msal


def get_access_token(client_id, client_secret, tenant_id):
    authority = f"https://login.microsoftonline.com/{tenant_id}"
    app = msal.ConfidentialClientApplication(client_id, authority=authority, client_credential=client_secret)
    result = app.acquire_token_for_client(scopes=["https://graph.microsoft.com/.default"])
    return result['access_token']

def get_user_folders(access_token, user_id):
    headers = {
        'Authorization': f'Bearer {access_token}',
        'Content-Type': 'application/json'
    }

    endpoint = f"https://graph.microsoft.com/v1.0/users/{user_id}/mailFolders"

    folders = []

    while endpoint:
        response = requests.get(endpoint, headers=headers)
        if response.status_code == 200:
            data = response.json()
            folders.extend(data['value'])
            endpoint = data.get('@odata.nextLink', None)
        else:
            print(f"Error fetching folders for user {user_id}: {response.status_code}")
            break
    return folders

def get_emails_from_folder(access_token, user_id, folder_id):
    headers = {
        'Authorization': f'Bearer {access_token}',
        'Content-Type': 'application/json'
    }
    endpoint = f"https://graph.microsoft.com/v1.0/users/{user_id}/mailFolders/{folder_id}/messages"
    emails = []
    while endpoint:
        response = requests.get(endpoint, headers=headers)
        if response.status_code == 200:
            data = response.json()
            emails.extend(data['value'])
            endpoint = data.get('@odata.nextLink', None)
        elif response.status_code == 429:  # Too Many Requests
            retry_after = int(response.headers.get('Retry-After', 10))
            time.sleep(retry_after)
        else:
            print(f"Error fetching emails from folder {folder_id} for user {user_id}: {response.status_code}")
            break
    return emails

def save_emails_to_disk(emails, user_id):
    with open(f"{user_id}_emails.json", 'w') as file:
        json.dump(emails, file)

def get_all_users(access_token):
    headers = {
        'Authorization': f'Bearer {access_token}',
        'Content-Type': 'application/json'
    }

    endpoint = "https://graph.microsoft.com/v1.0/users"

    users = []

    while endpoint:
        response = requests.get(endpoint, headers=headers)
        if response.status_code == 200:
            data = response.json()
            users.extend(data['value'])
            endpoint = data.get('@odata.nextLink', None)
        else:
            break
    return users

def get_user_emails_from_all_folders(access_token, user_id):
    folders = get_user_folders(access_token, user_id)
    all_emails = []
    for folder in folders:
        folder_id = folder['id']
        folder_name = folder['displayName']
        print(f"Fetching emails from folder: {folder_name} ({folder_id})")
        emails = get_emails_from_folder(access_token, user_id, folder_id)
        all_emails.extend(emails)
    return all_emails

def download_all_emails(client_id, client_secret, tenant_id):
    access_token = get_access_token(client_id, client_secret, tenant_id)
    users = get_all_users(access_token)
    for user in users:
        user_id = user['id']
        user_email = user['userPrincipalName']
        print(f"Fetching emails for user: {user_email} ({user_id})")
        emails = get_user_emails_from_all_folders(access_token, user_id)
        save_emails_to_disk(emails, user_id)


def main():
    client_id = "your-client-id"
    client_secret = "your-client-secret"
    tenant_id = "your-tenant-id"
    download_all_emails(client_id, client_secret, tenant_id)

if __name__ == "__main__":
    main()
