#!/usr/bin/python3
#
# Copyright (c) 2016 Cossack Labs Limited
#
# This file is part of Hermes.
#
# Hermes is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Hermes is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with Hermes.  If not, see <http://www.gnu.org/licenses/>.
#
#

import argparse

import pyhermes

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='hermes client')
    parser.add_argument('-u', '--user', required=True, help='user id')
    parser.add_argument('-p', '--private_key', required=True, help='private key in base64 format')
    parser.add_argument('-c', '--command', required=True, help='command')
    parser.add_argument('-d', '--doc_id', help='document id')
    parser.add_argument('-b', '--block_id', help='block id')
    parser.add_argument('-cc', '--context', help='context')
    parser.add_argument('-uu', '--peer_user_id', help='peer_user_id')
    parser.add_argument('-r',  '--access_mask',  help='access mask')
    args = parser.parse_args()

    hermes_storages = pyhermes.hermes_storages_create();
    if hermes_storages is None:
        raise Exception("hermes storages object not created");
    hermes_client = pyhermes.hermes_client(args.user, args.private_key, hermes_storages);
    if args.command == 'cd': #create_document
        print("document created with id: {}".format(hermes_client.create_document(args.context)))
    elif args.command == 'rd': #read_document
        print('readed document context is: {}'.format(hermes_client.read_document(args.doc_id)))
    elif args.command == 'ud': #update_document
        hermes_client.update_document(args.doc_id, args.context)
        print('document updated')
    elif args.command == 'dd': #delete_document
        hermes_client.delete_document(args.doc_id)
        print('document deleted')
    elif args.command == 'cb': #create_block
        print('document block created with id: {}'.format(hermes_client.create_document_block(args.doc_id, args.context)))
    elif args.command == 'rb': #read_block
        print('readed document block context is: ', hermes_client.read_document_block(args.doc_id, args.block_id))
    elif args.command == 'ub': #update_block
        hermes_client.update_document_block(args.doc_id, ards.block_id, args.context)
        print('document block updated')
    elif args.command == 'db': #delete_block
        hermes_client.delete_document_block(args.doc_id, args.block_id)
        print('document block deleted')
    elif args.command == 'ge': #grand access
        hermes_client.grand_access(args.doc_id, args.block_id, args.peer_user_id, args.access_mask)
        print('access granted')
    elif args.command == 'de': #grand access
        hermes_client.deny_access(args.doc_id, args.block_id, args.peer_user_id)
        print('access denied')
        
