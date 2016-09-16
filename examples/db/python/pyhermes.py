#!/usr/bin/python3
#
# Copyright (c) 2016 Cossack Labs Limited
#

import argparse
import logging
import string
import base64

import ctypes
import datetime
hermes_api=ctypes.cdll.LoadLibrary('libs/libmid_hermes.so');


hermes_client_create = hermes_api.hermes_client_create
hermes_client_create.restype = ctypes.POINTER(ctypes.c_int)
hermes_client_create.artgtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t]

hermes_client_destroy = hermes_api.hermes_client_destroy
hermes_client_destroy.argtypes = [ctypes.POINTER(ctypes.POINTER(ctypes.c_int))]

hermes_client_create_document = hermes_api.hermes_client_create_document
hermes_client_create_document.restype = ctypes.c_int
hermes_client_create_document.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_char), ctypes.c_size_t, ctypes.POINTER(ctypes.POINTER(ctypes.c_char))]

hermes_client_read_document = hermes_api.hermes_client_read_document
hermes_client_read_document.restype = ctypes.c_int
hermes_client_read_document.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.POINTER(ctypes.POINTER(ctypes.c_char)), ctypes.POINTER(ctypes.c_size_t)]

hermes_client_update_document = hermes_api.hermes_client_update_document
hermes_client_update_document.restype = ctypes.c_int
hermes_client_update_document.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t]

hermes_client_delete_document = hermes_api.hermes_client_delete_document
hermes_client_delete_document.restype = ctypes.c_int
hermes_client_delete_document.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p]

hermes_client_create_document_block = hermes_api.hermes_client_create_document_block
hermes_client_create_document_block.restype = ctypes.c_int
hermes_client_ucreate_document_block.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.c_int, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t, ctypes.POINTER(ctypes.POINTER(ctypes.c_char))]

hermes_client_read_document_block = hermes_api.hermes_client_read_document_block
hermes_client_read_document_block.restype = ctypes.c_int
hermes_client_read_document_block.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(ctypes.POINTER(ctypes.c_char)), ctypes.POINTER(ctypes.c_size_t)]

hermes_client_update_document_block = hermes_api.hermes_client_update_document_block
hermes_client_update_document_block.restype = ctypes.c_int
hermes_client_update_document_block.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t]

hermes_client_delete_document_block = hermes_api.hermes_client_update_document_block
hermes_client_delete_document_block.restype = ctypes.c_int
hermes_client_update_document_block.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.c_char_p]

hermes_client_grant_access = hermes_api.hermes_client_grant_access
hermes_client_grant_access.restype = ctypes.c_int
hermes_client_grant_access.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_int]

hermes_client_deny_access = hermes_api.hermes_client_deny_access
hermes_client_deny_access.restype = ctypes.c_int
hermes_client_deny_access.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]


class herles_client:
    def __init__(self, user_id, private_key):
        self.hc = hermes_client_create(user_id, base64.b64decode(private_key), len(base64.b64decode(private_key)))
        if self.hc is None:
            raise Exception("hermes client not created")

    def __del__(self):
        hermes_client_destroy(self.hc)

    def create_document(data):
        data_buf = ctypes.create_string_buffer(data)
        new_id = ctypes.c_char_p()
        if 0 != hermes_client_create_document(self.hc, ctypes.byref(data_buf), len(data), ctypes.byref(new_id)):
            raise Exception("hermes client creating document error")
        return new_id.value

    def read_document(doc_id):
        data = ctypes.c_char_p()
        data_length = ctypes.c_size_t()
        if 0 != hermes_client_read_document(self.hc, doc_id, ctypes.byref(data), ctypes.byref(data_length)):
            raise Exception("hermes client reading document error")
        return ctypes.string_at(data, data_length)

    def update_document(doc_id, data):
        data_buf = ctypes.create_string_buffer(data)
        if 0 != hermes_client_update_document(self.hc, doc_id, ctypes.byref(data_buf), len(data)):
            raise Exception("hermes client updating document error")

    def delete_document(doc_id):
        if 0 != hermes_client_delete_document(self.hc, doc_id):
            raise Exception("hermes client deleting document error")
        return new_id.value

    def create_document_block(data, doc_id, is_private):
        data_buf = ctypes.create_string_buffer(data)
        new_id = ctypes.c_char_p()
        if 0 != hermes_client_create_document_block(self.hc, doc_id, is_private, ctypes.byref(data_buf), len(data), ctypes.byref(new_id)):
            raise Exception("hermes client creating document block error")
        rerurn new_id.value

    def read_document_block(doc_id, block_id):
        data = ctypes.c_char_p()
        data_length = ctypes.c_size_t()
        if 0 != hermes_client_read_document_block(self.hc, doc_id, block_id, ctypes.byref(data), ctypes.byref(data_length)):
            raise Exception("hermes client reading document block error")
        return ctypes.string_at(data, data_length)

    def update_document_block(doc_id, data):
        data_buf = ctypes.create_string_buffer(data)
        new_id = ctypes.c_char_p()
        if 0 != hermes_client_update_document_block(self.hc, doc_id, block_id, ctypes.byref(data_buf), len(data)):
            raise Exception("hermes client updating document block error")

    def delete_document_block(doc_id, block_id):
        if 0 != hermes_client_delete_document_block(self.hc, doc_id, block_id):
            raise Exception("hermes client deleting document block error")

    def grand_access(doc_id, block_id, user_id, mask):
        if 0 != hermes_client_grant_access(self.hc, doc_id, block_id, user_id, mask):
            raise Exception("hermes client granting access error")

    def deny_access(doc_id, block_id, user_id):
        if 0 != hermes_client_deny_access(self.hc, doc_id, block_id, user_id):
            raise Exception("hermes client deneing access error")


    

    
                                       

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='hermes client')
    parser.add_argument('-u', '--user', required=true, help='user id')
    parser.add_argument('-p', '--private_key', required=true, help='private key in base64 format')
    parser.add_argument('-c', '--command', required=true, help='command')
    parser.add_argument('-d', '--doc_id', help='document id')
    parser.add_argument('-b', '--block_id', help='block id')
    parser.add_argument('-c', '--context', help='context')
    parser.add_argument('-uu', '--peer_user_id', help='peer_user_id')
    parser.add_argument('-r',  '--access_mask',  help='access mask')
    args = parser.parse_args()

    hermes_client = hermes_client(args.user, args.private_key);
    if args.command == 'cd': #create_document
        print('document created with id: ', hermes_client.create_document(args.context))
    else if args.command == 'rd': #read_document
        print('readed document context is: ', hermes_client.read_document(args.doc_id))
    else if args.command == 'ud': #update_document
        hermes_client.update_document(args.doc_id, args.context)
        print('document updated')
    else if args.command == 'dd': #delete_document
        hermes_client.delete_document(args.doc_id)
        print('document deleted')
    else if args.command == 'cb': #create_block
        print('document block created with id: ', hermes_client.create_document_block(args.doc_id, args.context))
    else if args.command == 'rb': #read_block
        print('readed document block context is: ', hermes_client.read_document_block(args.doc_id, args.block_id))
    else if args.command == 'ub': #update_block
        hermes_client.update_document_block(args.doc_id, ards.block_id, args.context)
        print('document block updated')
    else if args.command == 'db': #delete_block
        hermes_client.delete_document_block(args.doc_id, args.block_id)
        print('document block deleted')
    else if args.command == 'ge': #grand access
        hermes_client.grand_access(args.doc_id, args.block_id, args.peer_user_id, args.access_mask)
        print('access granted')
    else if args.command == 'de': #grand access
        hermes_client.deny_access(args.doc_id, args.block_id, args.peer_user_id)
        print('access denied')
        
