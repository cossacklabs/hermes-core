#!usr/bin/python

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
parser.add_argument('--command', dest='command', required=True,
                    help='command [ba|br|bu|bd|rgr|rgu|rdr|rdu])')
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

if args.command == "ba" and args.meta is not None:
    block = open(args.doc_file_name, 'rb').read()
    mid_hermes.addBlock(args.doc_file_name.encode(), block, args.meta.encode())
elif args.command == "br":
    print(mid_hermes.getBlock(args.doc_file_name.encode()))
elif args.command == "bu":
    mid_hermes.updBlock(args.doc_file_name.encode(), block, args.meta.encode())
elif args.command == "bd":
    mid_hermes.delBlock(args.doc_file_name.encode())
elif args.command == "u":
    mid_hermes.rotateBlock(args.doc_file_name.encode())
elif args.command == "rgr":
    mid_hermes.grantReadAccess(
        args.doc_file_name.encode(), args.for_user.encode())
elif args.command == "rgu":
    mid_hermes.grantUpdateAccess(
        args.doc_file_name.encode(), args.for_user.encode())
elif args.command == "rdr":
    mid_hermes.denyReadAccess(
        args.doc_file_name.encode(), args.for_user.encode())
elif args.command == "rdu":
    mid_hermes.denyUpdateAccess(
        args.doc_file_name.encode(), args.for_user.encode())

print(mid_hermes)
