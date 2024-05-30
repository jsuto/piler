#!/usr/bin/python3
# -*- coding: utf-8 -*-

import argparse
import base64
import json
import logging
import os
import requests
import time
import msal
import config


class TokenManager:
    def __init__(self, client_id, client_secret, tenant_id, debug=False):
        self.client_id = client_id
        self.client_secret = client_secret
        self.tenant_id = tenant_id
        self.debug = debug
        self.access_token = None
        self.token_expiry = None
        self._get_new_token()

    def _get_new_token(self):
        if self.debug:
            logging.info(f"Getting new access token")

        authority = f"https://login.microsoftonline.com/{self.tenant_id}"
        app = msal.ConfidentialClientApplication(self.client_id, authority=authority, client_credential=self.client_secret)
        result = app.acquire_token_for_client(scopes=["https://graph.microsoft.com/.default"])
        if "access_token" in result:
            self.access_token = result['access_token']
            self.token_expiry = time.time() + result['expires_in']
            if self.debug:
                self.decode_jwt()
        else:
            raise Exception("Could not obtain access token", result)

    def get_access_token(self):
        if self.token_expiry and (self.token_expiry - time.time()) < 600:
            self._get_new_token()
        return self.access_token

    def decode_jwt(self):
        parts = self.access_token.split('.')
        if len(parts) != 3:
            raise ValueError('Invalid token')

        payload = parts[1]
        padded_payload = payload + '=' * (4 - len(payload) % 4)  # Add padding if necessary
        decoded_payload = base64.urlsafe_b64decode(padded_payload)
        print(json.dumps(json.loads(decoded_payload), indent=4))


def get_user_folders(token_manager, user_id, parent_folder_id=None):
    access_token = token_manager.get_access_token()

    headers = {
        'Authorization': f'Bearer {access_token}',
        'Content-Type': 'application/json'
    }

    if parent_folder_id:
        endpoint = f"https://graph.microsoft.com/v1.0/users/{user_id}/mailFolders/{parent_folder_id}/childFolders"
    else:
        endpoint = f"https://graph.microsoft.com/v1.0/users/{user_id}/mailFolders"

    folders = []
    while endpoint:
        response = requests.get(endpoint, headers=headers)
        if response.status_code == 200:
            data = response.json()
            folders.extend([folder for folder in data['value'] if folder['displayName'] not in config.SKIP_FOLDERS])
            endpoint = data.get('@odata.nextLink', None)
        else:
            logging.error(f"Error fetching folders for user {user_id}: {response.status_code} - {response.text}")
            break
    return folders

def download_message_as_eml(token_manager, user_id, message_id, folder_name):
    access_token = token_manager.get_access_token()

    headers = {
        'Authorization': f'Bearer {access_token}',
        'Accept': 'application/vnd.ms-outlook'
    }
    endpoint = f"https://graph.microsoft.com/v1.0/users/{user_id}/messages/{message_id}/$value"
    response = requests.get(endpoint, headers=headers)
    if response.status_code == 200:
        save_path = os.path.join("emails", f"{user_id}/{folder_name}/{message_id}.eml")
        os.makedirs(os.path.dirname(save_path), exist_ok=True)
        with open(save_path, 'wb') as eml_file:
            eml_file.write(response.content)
        logging.info(f"Saved message {message_id} to {save_path}")
    else:
        logging.error(f"Error downloading message {message_id} for user {user_id}: {response.status_code} - {response.text}")

def get_messages_from_folder(token_manager, user_id, folder_id, folder_name):
    access_token = token_manager.get_access_token()

    headers = {
        'Authorization': f'Bearer {access_token}',
        'Content-Type': 'application/json'
    }

    endpoint = f"https://graph.microsoft.com/v1.0/users/{user_id}/mailFolders/{folder_id}/messages?$select=id,receivedDateTime,sender,subject"
    while endpoint:
        response = requests.get(endpoint, headers=headers)
        if response.status_code == 200:
            data = response.json()
            for message in data['value']:
                message_id = message['id']
                download_message_as_eml(token_manager, user_id, message_id, folder_name)
            endpoint = data.get('@odata.nextLink', None)
        else:
            logging.error(f"Error fetching messages from folder {folder_id} for user {user_id}: {response.status_code} - {response.text}")
            break

def process_folder_and_subfolders(token_manager, user_id, folder):
    folder_id = folder['id']
    folder_name = folder['displayName']
    size_str = ''

    if folder['totalItemCount']:
        size_str = folder['sizeInBytes']

    logging.info(f"{folder_name} has {folder['totalItemCount']} messages {size_str}")

    # Fetch and save messages in the current folder
    get_messages_from_folder(token_manager, user_id, folder_id, folder_name)

    # Fetch subfolders and process them recursively
    subfolders = get_user_folders(token_manager, user_id, folder_id)
    for subfolder in subfolders:
        subfolder['displayName'] = f"{folder_name}/{subfolder['displayName']}"
        process_folder_and_subfolders(token_manager, user_id, subfolder)

def get_users(token_manager, email=None):
    access_token = token_manager.get_access_token()

    headers = {
        'Authorization': f'Bearer {access_token}',
        'Content-Type': 'application/json'
    }

    endpoint = "https://graph.microsoft.com/v1.0/users"

    if email:
        endpoint = f"https://graph.microsoft.com/v1.0/users?$filter=mail eq '{email}'"

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

def get_user_by_email(token_manager, email):
    access_token = token_manager.get_access_token()

    headers = {
        'Authorization': f'Bearer {access_token}',
        'Content-Type': 'application/json'
    }

    endpoint = f"https://graph.microsoft.com/v1.0/users?$filter=mail eq '{email}'"
    response = requests.get(endpoint, headers=headers)

    if response.status_code == 200:
        data = response.json()
        if data['value']:
            return data['value'][0]
        else:
            logging.error(f"No user found with email: {email}")
            return None
    else:
        logging.error(f"Error fetching user with email {email}: {response.status_code} - {response.text}")
        return None

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-e", "--email", type=str, help="Download emails from this mailbox")
    args = parser.parse_args()

    logging.basicConfig(level=logging.INFO)

    token_manager = TokenManager(config.client_id, config.client_secret, config.tenant_id, config.DEBUG)

    users = get_users(token_manager, args.email)

    for user in users:
        user_id = user['id']
        user_email = user['userPrincipalName']
        logging.info(f"Fetching emails for user: {user_email} ({user_id})")

        root_folders = get_user_folders(token_manager, user_id)
        for folder in root_folders:
            process_folder_and_subfolders(token_manager, user_id, folder)


if __name__ == "__main__":
    main()
