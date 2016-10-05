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
import logging
import string
import base64

import ctypes
import datetime
hermes_api=ctypes.cdll.LoadLibrary('../../build/libmid_hermes.so');


hermes_storages_create = hermes_api.hermes_storages_create
hermes_storages_create.restype = ctypes.POINTER(ctypes.c_int)

hermes_storages_destroy = hermes_api.hermes_storages_destroy
hermes_storages_destroy.argtypes = [ctypes.POINTER(ctypes.c_int)]

hermes_client_create = hermes_api.hermes_client_create
hermes_client_create.restype = ctypes.POINTER(ctypes.c_int)
hermes_client_create.artgtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_byte), ctypes.c_size_t]

hermes_client_destroy = hermes_api.hermes_client_destroy
hermes_client_destroy.argtypes = [ctypes.POINTER(ctypes.POINTER(ctypes.c_int))]

hermes_client_create_document = hermes_api.hermes_client_create_document
hermes_client_create_document.restype = ctypes.c_int
#hermes_client_create_document.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_byte), ctypes.c_size_t, ctypes.POINTER(ctypes.c_char_p)]

hermes_client_read_document = hermes_api.hermes_client_read_document
hermes_client_read_document.restype = ctypes.c_int
#hermes_client_read_document.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.POINTER(ctypes.POINTER(ctypes.c_char)), ctypes.POINTER(ctypes.c_size_t)]

hermes_client_update_document = hermes_api.hermes_client_update_document
hermes_client_update_document.restype = ctypes.c_int
#hermes_client_update_document.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t]

hermes_client_delete_document = hermes_api.hermes_client_delete_document
hermes_client_delete_document.restype = ctypes.c_int
#hermes_client_delete_document.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p]

hermes_client_create_document_block = hermes_api.hermes_client_create_document_block
hermes_client_create_document_block.restype = ctypes.c_int
#hermes_client_create_document_block.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.c_int, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t, ctypes.POINTER(ctypes.POINTER(ctypes.c_char))]

hermes_client_read_document_block = hermes_api.hermes_client_read_document_block
hermes_client_read_document_block.restype = ctypes.c_int
#hermes_client_read_document_block.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(ctypes.POINTER(ctypes.c_char)), ctypes.POINTER(ctypes.c_size_t)]

hermes_client_update_document_block = hermes_api.hermes_client_update_document_block
hermes_client_update_document_block.restype = ctypes.c_int
#hermes_client_update_document_block.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t]

hermes_client_delete_document_block = hermes_api.hermes_client_update_document_block
hermes_client_delete_document_block.restype = ctypes.c_int
#hermes_client_delete_document_block.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.c_char_p]

hermes_client_grant_access = hermes_api.hermes_client_grant_access_to_document_block
hermes_client_grant_access.restype = ctypes.c_int
#hermes_client_grant_access.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_int]

hermes_client_deny_access = hermes_api.hermes_client_deny_access_to_document_block
hermes_client_deny_access.restype = ctypes.c_int
#hermes_client_deny_access.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]


class hermes_client:
    def __init__(self, user_id, private_key, hermes_storages):
        self.hc = hermes_client_create(ctypes.c_char_p(user_id.encode("utf-8")), base64.b64decode(private_key), len(base64.b64decode(private_key)), hermes_storages)
        if self.hc is None:
            raise Exception("hermes client not created")

    def __del__(self):
        hermes_client_destroy(self.hc)

    def create_document(self, data):
#        data_buf = ctypes.create_string_buffer(data)
        new_id = ctypes.c_char_p()
        if 0 != hermes_client_create_document(self.hc, data.encode("utf-8"), len(data), ctypes.byref(new_id)):
            raise Exception("hermes client creating document error")
        return new_id.value

    def read_document(self, doc_id):
        data = ctypes.c_char_p()
        data_length = ctypes.c_size_t()
        if 0 != hermes_client_read_document(self.hc, ctypes.c_char_p(doc_id.encode("utf-8")), ctypes.byref(data), ctypes.byref(data_length)):
            raise Exception("hermes client reading document error")
        return ctypes.string_at(data, data_length.value)

    def update_document(self, doc_id, data):
        data_buf = ctypes.create_string_buffer(data)
        if 0 != hermes_client_update_document(self.hc, ctypes.c_char_p(doc_id.encode("utf-8")), ctypes.byref(data_buf), len(data)):
            raise Exception("hermes client updating document error")

    def delete_document(self, doc_id):
        if 0 != hermes_client_delete_document(self.hc, ctypes.c_char_p(doc_id.encode("utf-8"))):
            raise Exception("hermes client deleting document error")
        return new_id.value

    def create_document_block(self, data, doc_id, is_private):
        data_buf = ctypes.create_string_buffer(data)
        new_id = ctypes.c_char_p()
        if 0 != hermes_client_create_document_block(self.hc, ctypes.c_char_p(doc_id.encode("utf-8")), is_private, ctypes.byref(data_buf), len(data), ctypes.byref(new_id)):
            raise Exception("hermes client creating document block error")
        return new_id.value

    def read_document_block(self, doc_id, block_id):
        data = ctypes.c_char_p()
        data_length = ctypes.c_size_t()
        if 0 != hermes_client_read_document_block(self.hc, ctypes.c_char_p(doc_id.encode("utf-8")), ctypes.c_char_p(block_id.encode("utf-8")), ctypes.byref(data), ctypes.byref(data_length)):
            raise Exception("hermes client reading document block error")
        return ctypes.string_at(data, data_length.value)

    def update_document_block(self, doc_id, data):
        data_buf = ctypes.create_string_buffer(data)
        new_id = ctypes.c_char_p()
        if 0 != hermes_client_update_document_block(self.hc, ctypes.c_char_p(doc_id.encode("utf-8")), ctypes.c_char_p(block_id.encode("utf-8")), ctypes.byref(data_buf), len(data)):
            raise Exception("hermes client updating document block error")

    def delete_document_block(self, doc_id, block_id):
        if 0 != hermes_client_delete_document_block(self.hc, ctypes.c_char_p(doc_id.encode("utf-8")), ctypes.c_char_p(block_id.encode("utf-8"))):
            raise Exception("hermes client deleting document block error")

    def grand_access(self, doc_id, block_id, user_id, mask):
        if 0 != hermes_client_grant_access(self.hc, ctypes.c_char_p(doc_id.encode("utf-8")), ctypes.c_char_p(block_id.encode("utf-8")), ctypes.c_char_p(user_id.encode("utf-8")), mask):
            raise Exception("hermes client granting access error")

    def deny_access(self, doc_id, block_id, user_id):
        if 0 != hermes_client_deny_access(self.hc, ctypes.c_char_p(doc_id.encode("utf-8")), ctypes.c_char_p(block_id.encode("utf-8")), ctypes.c_char_p(user_id.encode("utf-8"))):
            raise Exception("hermes client deneing access error")
