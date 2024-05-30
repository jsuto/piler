#!/usr/bin/python3
# -*- coding: utf-8 -*-

import logging
import os
import requests
import msal
import config


def get_access_token(client_id, client_secret, tenant_id):
    authority = f"https://login.microsoftonline.com/{tenant_id}"
    app = msal.ConfidentialClientApplication(client_id, authority=authority, client_credential=client_secret)
    result = app.acquire_token_for_client(scopes=["https://graph.microsoft.com/.default"])
    if "access_token" in result:
        return result['access_token']
    else:
        raise Exception("Could not obtain access token", result)

def get_user_folders(access_token, user_id, parent_folder_id=None):
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
            folders.extend(data['value'])
            endpoint = data.get('@odata.nextLink', None)
        else:
            logging.error(f"Error fetching folders for user {user_id}: {response.status_code} - {response.text}")
            break
    return folders

def get_messages_from_folder(access_token, user_id, folder_id):
    headers = {
        'Authorization': f'Bearer {access_token}',
        'Content-Type': 'application/json'
    }

    endpoint = f"https://graph.microsoft.com/v1.0/users/{user_id}/mailFolders/{folder_id}/messages"
    messages = []
    while endpoint:
        response = requests.get(endpoint, headers=headers)
        if response.status_code == 200:
            data = response.json()
            messages.extend(data['value'])
            endpoint = data.get('@odata.nextLink', None)
        else:
            logging.error(f"Error fetching messages from folder {folder_id} for user {user_id}: {response.status_code} - {response.text}")
            break
    return messages

def download_message_as_eml(access_token, user_id, message_id, save_path):
    headers = {
        'Authorization': f'Bearer {access_token}',
        'Accept': 'application/vnd.ms-outlook'
    }

    endpoint = f"https://graph.microsoft.com/v1.0/users/{user_id}/messages/{message_id}/$value"
    response = requests.get(endpoint, headers=headers)
    if response.status_code == 200:
        with open(save_path, 'wb') as eml_file:
            eml_file.write(response.content)
        logging.info(f"Saved message {message_id} to {save_path}")
    else:
        logging.error(f"Error downloading message {message_id} for user {user_id}: {response.status_code} - {response.text}")

def get_user_emails_from_folder(access_token, user_id, folder_id, folder_name):
    messages = get_messages_from_folder(access_token, user_id, folder_id)
    for message in messages:
        message_id = message['id']
        save_path = os.path.join("emails", f"{user_id}/{folder_name}/{message_id}.eml")
        os.makedirs(os.path.dirname(save_path), exist_ok=True)
        download_message_as_eml(access_token, user_id, message_id, save_path)

def process_folder_and_subfolders(access_token, user_id, folder_id, folder_name):
    # Fetch and save messages in the current folder
    messages = get_messages_from_folder(access_token, user_id, folder_id)
    for message in messages:
        message_id = message['id']
        save_path = os.path.join("emails", f"{user_id}/{folder_name}/{message_id}.eml")
        os.makedirs(os.path.dirname(save_path), exist_ok=True)
        download_message_as_eml(access_token, user_id, message_id, save_path)

    # Fetch subfolders and process them recursively
    subfolders = get_user_folders(access_token, user_id, folder_id)
    for subfolder in subfolders:
        subfolder_id = subfolder['id']
        subfolder_name = f"{folder_name}/{subfolder['displayName']}"
        process_folder_and_subfolders(access_token, user_id, subfolder_id, subfolder_name)

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

def main():
    logging.basicConfig(level=logging.INFO)

    access_token = get_access_token(config.client_id, config.client_secret, config.tenant_id)

    users = get_all_users(access_token)
    for user in users:
        user_id = user['id']
        user_email = user['userPrincipalName']
        print(f"Fetching emails for user: {user_email} ({user_id})")

        root_folders = get_user_folders(access_token, user_id)
        for folder in root_folders:
            folder_id = folder['id']
            folder_name = folder['displayName']
            process_folder_and_subfolders(access_token, user_id, folder_id, folder_name)


if __name__ == "__main__":
    main()
