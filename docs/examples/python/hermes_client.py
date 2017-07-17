#!usr/bin/python

#
# Copyright (c) 2017 Cossack Labs Limited
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


import socket
import argparse
import base64
import hermes


class Trasnport:
    def __init__(self, host, port):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect((host, port))

    def __del__(self):
        if self.socket:
            self.socket.close()

    def send(self, msg):
        total_sent = 0
        while total_sent < len(msg):
            sent = self.socket.send(msg[total_sent:])
            if not sent:
                raise RuntimeError("socket connection broken")
            total_sent = total_sent + sent

    def receive(self, needed_length):
        data = self.socket.recv(needed_length)
        if not data:
            raise RuntimeError("socket connection broken")
        return data


parser = argparse.ArgumentParser(description='Hermes client example.')
parser.add_argument('--id', dest='id', required=True, help='user identificator')
parser.add_argument('--sk', dest='sk', required=True,
                    help='base64 encoded user private key)')

parser.add_argument('--add', '-a', action='store_true', default=False,
                    dest='add')
parser.add_argument('--read', '-r', action='store_true', default=False,
                    dest='read')
parser.add_argument('--update', '-u', action='store_true', default=False,
                    dest='update')
parser.add_argument('--delete', '-d', action='store_true', default=False,
                    dest='delete')

# parser.add_argument('--rotate', '-rt', action='store_true', default=False,
#                     dest='rotate')

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
                    help='peer user identificator')
args = parser.parse_args()

credential_store_transport = Trasnport("127.0.0.1", 8888)
data_store_transport = Trasnport("127.0.0.1", 8889)
key_store_transport = Trasnport("127.0.0.1", 8890)

mid_hermes = hermes.MidHermes(
    args.id, base64.b64decode(args.sk), credential_store_transport,
    data_store_transport, key_store_transport)

if not (args.add or args.read or args.update or args.delete or
            args.grant_read or args.grant_update or args.revoke_update or
            args.revoke_read):# or args.rotate):
    print("choose any command add|read|update|delete|grant_read|grant_update|"
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
# elif args.rotate:
#     mid_hermes.rotateBlock(args.doc_file_name.encode())
#     print('rotated <{}>'.format(args.doc_file_name))
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
