#!/usr/bin/python

import socket
import hermes
import argparse
import base64

class transport:
    def __init__(self, host, port):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((host, port))

    def __del__(self):
        if(self.sock):
            self.sock.close()

    def send(self, msg):
        totalsent = 0
        while totalsent < len(msg):
            sent = self.sock.send(msg[totalsent:])
            if sent == 0:
                raise RuntimeError("socket connection broken")
            totalsent = totalsent + sent

    def receive(self, needed_length):
        data = self.sock.recv(needed_length)
        if not data:
            raise RuntimeError("socket connection broken")
        return data

    
parser = argparse.ArgumentParser(description='Hermes client example.')
parser.add_argument('--id', dest='id', required=True, help='user identificator)')
parser.add_argument('--sk', dest='sk', required=True, help='base64 encoded user private key)')
parser.add_argument('--command', dest='command', required=True, help='command [ba|br|bu|bd|rgr|rgu|rdr|rdu])')
parser.add_argument('--doc', dest='doc_file_name', required=True, help='document file name')
parser.add_argument('--meta', dest='meta', help='document meta data')
parser.add_argument('--for_user', dest='for_user', help='peer user identificator')
args = parser.parse_args()

cs_transport=transport("127.0.0.1", 8888);
ds_transport=transport("127.0.0.1", 8889);
ks_transport=transport("127.0.0.1", 8890);

mh=hermes.MidHermes(args.id, base64.b64decode(args.sk), cs_transport, ds_transport, ks_transport)

if args.command=="ba" and args.meta is not None:
    block = open(args.doc_file_name, 'rb').read()
    mh.addBlock(args.doc_file_name.encode(), block, args.meta.encode())
elif args.command=="br":
    print(mh.getBlock(args.doc_file_name.encode()))
elif args.command=="bu":
    mh.updBlock(args.doc_file_name.encode(), block, args.meta.encode())
elif args.command=="bd":
    mh.delBlock(args.doc_file_name.encode())
elif args.command=="u":
    mh.rotateBlock(args.doc_file_name.encode())
elif args.command=="rgr":
    mh.grantReadAccess(args.doc_file_name.encode(), args.for_user.encode())
elif args.command=="rgu":
    mh.grantUpdateAccess(args.doc_file_name.encode(), args.for_user.encode())
elif args.command=="rdr":
    mh.denyReadAccess(args.doc_file_name.encode(), args.for_user.encode())
elif args.command=="rdu":
    mh.denyUpdateAccess(args.doc_file_name.encode(), args.for_user.encode())
    
print(mh)
