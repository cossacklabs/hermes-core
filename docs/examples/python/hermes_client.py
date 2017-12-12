#!/usr/bin/python

#
# Copyright (c) 2017 Cossack Labs Limited
#
# This file is a part of Hermes-core.
#
# Hermes-core is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Hermes-core is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with Hermes-core.  If not, see <http://www.gnu.org/licenses/>.
#
#

import socket
import argparse
import base64
import json
import hermes


class Transport(object):
    def __init__(self, host, port):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect((host, port))

    def __del__(self):
        if self.socket:
            self.socket.close()

    def send(self, msg):
        total_sent = 0
        message_length = len(msg)
        while total_sent < message_length:
            sent = self.socket.send(msg[total_sent:])
            if not sent:
                raise RuntimeError("socket connection broken")
            total_sent = total_sent + sent

    def receive(self, needed_length):
        chunks = []
        bytes_recieved = 0
        while bytes_recieved < needed_length:
            chunk = self.socket.recv(needed_length - bytes_recieved)
            if not chunk:
                raise RuntimeError("socket connection broken")
            chunks.append(chunk)
            bytes_recieved = bytes_recieved + len(chunk)
        return b''.join(chunks)


parser = argparse.ArgumentParser(description='Hermes client example.')
parser.add_argument('--id', dest='id', required=True, help='user identificator')
parser.add_argument('--private_key', '-pk', dest='private_key', required=True,
                    help='path to private key)')

parser.add_argument('--add', '-a', action='store_true', default=False,
                    dest='add')
parser.add_argument('--read', '-r', action='store_true', default=False,
                    dest='read')
parser.add_argument('--update', '-u', action='store_true', default=False,
                    dest='update')
parser.add_argument('--delete', '-d', action='store_true', default=False,
                    dest='delete')

parser.add_argument('--rotate', '-rt', action='store_true', default=False, dest='rotate')

parser.add_argument('--grant_read', '-gr', action='store_true', default=False,
                    dest='grant_read')
parser.add_argument('--grant_update', '-gu', action='store_true', default=False,
                    dest='grant_update')
parser.add_argument('--revoke_read', '-rr', action='store_true', default=False,
                    dest='revoke_read')
parser.add_argument('--revoke_update', '-ru', action='store_true', default=False,
                    dest='revoke_update')


parser.add_argument('--doc', dest='doc_file_name', required=True,
                    help='document file name')
parser.add_argument('--meta', dest='meta', help='document meta data')
parser.add_argument('--for_user', dest='for_user',
                    help='peer user identifier')


parser.add_argument('--config', dest='config', help='Path to config file', default='config.json')

args = parser.parse_args()

with open(args.private_key, 'rb') as f:
    private_key = f.read()

with open(args.config, 'r') as f:
    config = json.load(f)

credential_store_transport = Transport(config['credential_store_host'], config['credential_store_port'])
key_store_transport = Transport(config['key_store_host'], config['key_store_port'])
data_store_transport = Transport(config['data_store_host'], config['data_store_port'])

credential_store_secure_transport = hermes.SecureHermesTransport(
    args.id.encode('utf-8'), private_key, config['credential_store_id'].encode("utf-8"),
    base64.b64decode(config['credential_store_public_key'].encode('utf-8')), credential_store_transport, False)
key_store_secure_transport = hermes.SecureHermesTransport(
    args.id.encode('utf-8'), private_key, config['key_store_id'].encode("utf-8"),
    base64.b64decode(config['key_store_public_key'].encode('utf-8')), key_store_transport, False)
data_store_secure_transport = hermes.SecureHermesTransport(
    args.id.encode('utf-8'), private_key, config['data_store_id'].encode("utf-8"),
    base64.b64decode(config['data_store_public_key'].encode('utf-8')), data_store_transport, False)


mid_hermes = hermes.MidHermes(
    args.id, private_key, credential_store_secure_transport,
    data_store_secure_transport, key_store_secure_transport)

if not (args.add or args.read or args.update or args.delete or args.rotate or
            args.grant_read or args.grant_update or args.revoke_update or
            args.revoke_read):
    print("choose any command add|read|update|delete|rotate|grant_read|grant_update|"
          "revoke_read|revoke_update")
    exit(1)


if args.add and args.meta is not None:
    block = open(args.doc_file_name, 'rb').read()
    mid_hermes.addBlock(args.doc_file_name.encode(), block, args.meta.encode())
    print('added <{}> with meta <{}>'.format(args.doc_file_name, args.meta))
elif args.read:
    print(mid_hermes.getBlock(args.doc_file_name.encode()))
elif args.update:
    block = open(args.doc_file_name, 'rb').read()
    mid_hermes.updBlock(args.doc_file_name.encode(), block, args.meta.encode())
    print('updated <{}> with meta <{}>'.format(args.doc_file_name, args.meta))
elif args.delete:
    mid_hermes.delBlock(args.doc_file_name.encode())
    print('deleted <{}>'.format(args.doc_file_name))
elif args.rotate:
    mid_hermes.rotateBlock(args.doc_file_name.encode())
    print('rotated <{}>'.format(args.doc_file_name))
elif args.grant_read:
    mid_hermes.grantReadAccess(
        args.doc_file_name.encode(), args.for_user.encode())
    print('granted read access <{}> for user <{}>'.format(
        args.doc_file_name, args.for_user))
elif args.grant_update:
    mid_hermes.grantUpdateAccess(
        args.doc_file_name.encode(), args.for_user.encode())
    print('granted update access <{}> for user <{}>'.format(
        args.doc_file_name, args.for_user))
elif args.revoke_read:
    mid_hermes.denyReadAccess(
        args.doc_file_name.encode(), args.for_user.encode())
    print('revoked read access <{}> for user <{}>'.format(
        args.doc_file_name, args.for_user))
elif args.revoke_update:
    mid_hermes.denyUpdateAccess(
        args.doc_file_name.encode(), args.for_user.encode())
    print('revoked update access <{}> for user <{}>'.format(
        args.doc_file_name, args.for_user))

print('done')
