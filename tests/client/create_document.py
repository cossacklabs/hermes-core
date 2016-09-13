#!/usr/bin/python3
#
# Copyright (c) 2016 Cossack Labs Limited
#

import argparse
import logging
import random
import string
import asyncio
import base64

import jinja2
from aiohttp import web
import aiohttp_jinja2

from pythemis import ssession
from pythemis import skeygen

import ctypes
from ctypes.util import find_library

import pymongo
import datetime
from bson.json_util import dumps
from bson.objectid import ObjectId
from bson import binary

from pymongo import monitoring

class CommandLogger(monitoring.CommandListener):

    def started(self, event):
        logging.info("Command {0.command_name} with request id "
                     "{0.request_id} started on server "
                     "{0.connection_id}".format(event))

    def succeeded(self, event):
        logging.info("Command {0.command_name} with request id "
                     "{0.request_id} on server {0.connection_id} "
                     "succeeded in {0.duration_micros} "
                     "microseconds".format(event))

    def failed(self, event):
        logging.info("Command {0.command_name} with request id "
                     "{0.request_id} on server {0.connection_id} "
                     "failed in {0.duration_micros} "
                     "microseconds".format(event))

hermes_api=ctypes.cdll.LoadLibrary('libs/libHermesClient.so');

hermes_config_create = hermes_api.config_create
hermes_config_create.restypes = ctypes.POINTER(ctypes.c_int)
hermes_config_create.argtypes = [ctypes.c_char_p]

hermes_config_destroy = hermes_api.config_destroy
hermes_config_destroy.argtypes = [ctypes.POINTER(ctypes.c_int)]

hermes_config_get_private_key = hermes_api.config_get_private_key
#hermes_config_get_private_key.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.POINTER(ctypes.c_int)), ctypes.POINTER(ctypes.c_int)]

hermes_config_get_public_key = hermes_api.config_get_public_key
#hermes_config_get_public_key.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.POINTER(ctypes.c_int)), ctypes.POINTER(ctypes.c_int)]


hermes_client_create = hermes_api.hermes_client_create
hermes_client_create.restype = ctypes.POINTER(ctypes.c_int)
hermes_client_create.artgtypes = [ctypes.POINTER(ctypes.c_int)]

hermes_client_destroy = hermes_api.hermes_client_destroy
hermes_client_destroy.argtypes = [ctypes.POINTER(ctypes.POINTER(ctypes.c_int))]

hermes_client_get_pub_key_by_id = hermes_api.hermes_client_get_public_key;
hermes_client_get_pub_key_by_id.restype = ctypes.c_int;
hermes_client_get_pub_key_by_id.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.POINTER(ctypes.POINTER(ctypes.c_int)), ctypes.POINTER(ctypes.c_int)];

hermes_client_get_docs_list = hermes_api.hermes_client_get_docs_list;
hermes_client_get_docs_list.restype = ctypes.c_int;
hermes_client_get_docs_list.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_char_p)];

hermes_client_get_users_list = hermes_api.hermes_client_get_users_list;
hermes_client_get_users_list.restype = ctypes.c_int;
hermes_client_get_users_list.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.POINTER(ctypes.c_char_p)];

hermes_client_read_doc = hermes_api.hermes_client_read_doc;
hermes_client_read_doc.restype = ctypes.c_int;
hermes_client_read_doc.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_char_p)];

hermes_client_add_doc = hermes_api.hermes_client_add_doc;
hermes_client_add_doc.restype = ctypes.c_int;
hermes_client_add_doc.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t];

hermes_client_update_doc = hermes_api.hermes_client_update_doc;
hermes_client_update_doc.restype = ctypes.c_int;
hermes_client_update_doc.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t, ctypes.c_char_p, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t];

hermes_client_add_public_block = hermes_api.hermes_client_add_public_block;
hermes_client_add_public_block.restype = ctypes.c_int;
hermes_client_add_public_block.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t, ctypes.c_char_p, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t];

hermes_client_add_private_block = hermes_api.hermes_client_add_private_block;
hermes_client_add_private_block.restype = ctypes.c_int;
hermes_client_add_private_block.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t, ctypes.c_char_p, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t];

hermes_client_add_access = hermes_api.hermes_client_add_access;
hermes_client_add_access.restype = ctypes.c_int;
hermes_client_add_access.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t, ctypes.c_char_p, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t];

hermes_client_rm_access = hermes_api.hermes_client_rm_access;
hermes_client_rm_access.restype = ctypes.c_int;
hermes_client_rm_access.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t, ctypes.c_char_p];

hermes_client_get_public_block = hermes_api.hermes_client_get_public_block;
hermes_client_get_public_block.restype = ctypes.c_int;
hermes_client_get_public_block.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(ctypes.POINTER(ctypes.c_int)), ctypes.POINTER(ctypes.c_int)];

hermes_client_get_private_block = hermes_api.hermes_client_get_private_block;
hermes_client_get_private_block.restype = ctypes.c_int;
hermes_client_get_private_block.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(ctypes.POINTER(ctypes.c_int)), ctypes.POINTER(ctypes.c_int)];

hermes_client_rm_public_block = hermes_api.hermes_client_rm_public_block;
hermes_client_rm_public_block.restype = ctypes.c_int;
hermes_client_rm_public_block.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t];

hermes_client_rm_private_block = hermes_api.hermes_client_rm_private_block;
hermes_client_rm_private_block.restype = ctypes.c_int;
hermes_client_rm_private_block.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t];

db=pymongo.MongoClient('localhost', 27017, event_listeners=[CommandLogger()]).hermes

class Transport(ssession.mem_transport):  # necessary callback
    def get_pub_key_by_id(self, user_id):
        key=ctypes.POINTER(ctypes.c_int)()
        key_length=ctypes.c_int(0)
        if 0==hermes_client_get_pub_key_by_id(hermes, user_id, ctypes.byref(key), ctypes.byref(key_length)):
            db.log.insert_one({"readed":0, "op_name": "get_pub_key_by_id", "params": [user_id], "result": "success", "date":datetime.datetime.utcnow()});
            return ctypes.string_at(key, key_length.value);
        db.log.insert_one({"readed":0, "op_name": "get_pub_key_by_id", "params": [user_id], "result": "fail", "date":datetime.datetime.utcnow()});
        return None

class Transport2(ssession.mem_transport):  # necessary callback
    def get_pub_key_by_id(self, user_id):
        return base64.b64decode(user_id.decode("utf8"))

id_symbols = string.ascii_letters + string.digits
def generate_id():
    return ''.join([random.choice(id_symbols) for _ in range(32)])

def get_docs_list(param_list):
    logger.info("get_doc_list called with params: {}".format(param_list))
    data=ctypes.c_char_p()
    if 0==hermes_client_get_docs_list(hermes, ctypes.byref(data)):
        logger.info("get_doc_list return '{}'".format(data.value));
        db.log.insert_one({"readed":0, "op_name": "get_docs_list", "params": [param_list], "result": "success", "date":datetime.datetime.utcnow()});
        return data.value.decode("UTF-8")
    db.log.insert_one({"readed":0, "op_name": "get_docs_list", "params": [param_list], "result": "fail", "date":datetime.datetime.utcnow()});
    return None

def get_users_list(param_list):
    logger.info("get_users_list called with params: {}".format(param_list))
    data1=ctypes.c_char_p()
    if 0==hermes_client_get_users_list(hermes, param_list[0].encode("UTF-8"), ctypes.byref(data1)):
        logger.info("get_users_list return '{}'".format(data1.value));
        db.log.insert_one({"readed":0, "op_name": "get_users_list", "params": [param_list], "result": "success", "date":datetime.datetime.utcnow()});
        return data1.value.decode("UTF-8")
    db.log.insert_one({"readed":0, "op_name": "get_users_list", "params": [param_list], "result": "fail", "date":datetime.datetime.utcnow()});
    return None

def read_doc(param_list):
    logger.info("read_doc called with params: {}".format(param_list))
    data=ctypes.c_char_p()
    if 0==hermes_client_read_doc(hermes, param_list[0].encode("UTF-8"), param_list[1].encode("UTF-8"), ctypes.byref(data)):
        logger.info("read_doc return '{}'".format(data.value));
        db.log.insert_one({"readed":0, "op_name": "read_doc", "params": [param_list], "result": "success", "date":datetime.datetime.utcnow()});
        return data.value.decode("UTF-8")    
    db.log.insert_one({"readed":0, "op_name": "read_doc", "params": [param_list], "result": "fail", "date":datetime.datetime.utcnow()});
    return None

def add_doc(param_list):
    logger.info("add_doc called with params: {}".format(param_list))
    if 0==hermes_client_add_doc(hermes, param_list[0].encode("UTF-8"), base64.b64decode(param_list[1]), base64.b64decode(param_list[2]), len(base64.b64decode(param_list[2])), base64.b64decode(param_list[3]), len(base64.b64decode(param_list[3])), base64.b64decode(param_list[4]), len(base64.b64decode(param_list[4])), base64.b64decode(param_list[5]), len(base64.b64decode(param_list[5]))):
        logger.info("add_doc return success");
        db.log.insert_one({"readed":0, "op_name": "add_doc", "params": [param_list], "result": "success", "date":datetime.datetime.utcnow()});
        return "success"
    db.log.insert_one({"readed":0, "op_name": "add_doc", "params": [param_list], "result": "fail", "date":datetime.datetime.utcnow()});
    return None

def update_doc(param_list):
    logger.info("update_doc called with params: {}".format(param_list))
    if 0==hermes_client_update_doc(hermes, param_list[0].encode("UTF-8"), base64.b64decode(param_list[1]), base64.b64decode(param_list[2]), len(base64.b64decode(param_list[2])), param_list[3].encode("UTF-8"), base64.b64decode(param_list[4]), len(base64.b64decode(param_list[4]))):
        logger.info("update_doc return success");
        db.log.insert_one({"readed":0, "op_name": "update_doc", "params": [param_list], "result": "success", "date":datetime.datetime.utcnow()});
        return "success"    
    db.log.insert_one({"readed":0, "op_name": "update_doc", "params": [param_list], "result": "fail", "date":datetime.datetime.utcnow()});
    return None

def add_public_block(param_list):
    logger.info("add_public_block called with params: {}".format(param_list))
    if 0==hermes_client_add_public_block(hermes, param_list[0].encode("UTF-8"), param_list[1].encode("UTF-8"), base64.b64decode(param_list[2]), len(base64.b64decode(param_list[2])), param_list[3].encode("UTF-8"), base64.b64decode(param_list[4]), len(base64.b64decode(param_list[4]))):
        logger.info("add_public_block return success");
        db.log.insert_one({"readed":0, "op_name": "add_public_block", "params": [param_list], "result": "success", "date":datetime.datetime.utcnow()});
        return "success"    
    db.log.insert_one({"readed":0, "op_name": "add_public_block", "params": [param_list], "result": "fail", "date":datetime.datetime.utcnow()});
    return None

def add_private_block(param_list):
    logger.info("add_private_block called with params: {}".format(param_list))
    if 0==hermes_client_add_private_block(hermes, param_list[0].encode("UTF-8"), param_list[1].encode("UTF-8"), base64.b64decode(param_list[2]), len(base64.b64decode(param_list[2])), param_list[3].encode("UTF-8"), base64.b64decode(param_list[4]), len(base64.b64decode(param_list[4]))):
        logger.info("add_private_block return success");
        db.log.insert_one({"readed":0, "op_name": "add_private_block", "params": [param_list], "result": "success", "date":datetime.datetime.utcnow()});
        return "success"    
    db.log.insert_one({"readed":0, "op_name": "add_private_block", "params": [param_list], "result": "fail", "date":datetime.datetime.utcnow()});
    return None

def add_access(param_list):
    logger.info("add_access called with params: {}".format(param_list))
    if 0==hermes_client_add_access(hermes, param_list[0].encode("UTF-8"), param_list[1].encode("UTF-8"), base64.b64decode(param_list[2]), len(base64.b64decode(param_list[2])), param_list[3].encode("UTF-8"), base64.b64decode(param_list[4]), len(base64.b64decode(param_list[4])), base64.b64decode(param_list[5]), len(base64.b64decode(param_list[5])), base64.b64decode(param_list[6]), len(base64.b64decode(param_list[6]))):
        logger.info("add_access return success");
        db.log.insert_one({"readed":0, "op_name": "add_access", "params": [param_list], "result": "success", "date":datetime.datetime.utcnow()});
        return "success"    
    db.log.insert_one({"readed":0, "op_name": "add_access", "params": [param_list], "result": "fail", "date":datetime.datetime.utcnow()});
    return None

def rm_access(param_list):
    logger.info("rm_access called with params: {}".format(param_list))
    if 0==hermes_client_rm_access(hermes, param_list[0].encode("UTF-8"), param_list[1].encode("UTF-8"), base64.b64decode(param_list[2]), len(base64.b64decode(param_list[2])), param_list[3].encode("UTF-8")):
        logger.info("rm_access return success");
        db.log.insert_one({"readed":0, "op_name": "rm_access", "params": [param_list], "result": "success", "date":datetime.datetime.utcnow()});
        return "success"
    db.log.insert_one({"readed":0, "op_name": "rm_access", "params": [param_list], "result": "fail", "date":datetime.datetime.utcnow()});
    return None

def get_public_block(param_list):
    logger.info("get_public_block called with params: {}".format(param_list))
    data=ctypes.POINTER(ctypes.c_int)()
    data_length=ctypes.c_int(0)
    if 0==hermes_client_get_public_block(hermes, param_list[0].encode("UTF-8"), param_list[1].encode("UTF-8"), param_list[2].encode("UTF-8"), ctypes.byref(data), ctypes.byref(data_length)):
        logger.info("get_public_block return success");
        db.log.insert_one({"readed":0, "op_name": "get_public_block", "params": [param_list], "result": "success", "date":datetime.datetime.utcnow()});        
        return base64.b64encode(ctypes.string_at(data, data_length.value)).decode("UTF-8");
    db.log.insert_one({"readed":0, "op_name": "get_public_block", "params": [param_list], "result": "fail", "date":datetime.datetime.utcnow()});        
    return None

def get_private_block(param_list):
    logger.info("get_private_block called with params: {}".format(param_list))
    data=ctypes.POINTER(ctypes.c_int)()
    data_length=ctypes.c_int(0)
    if 0==hermes_client_get_private_block(hermes, param_list[0].encode("UTF-8"), param_list[1].encode("UTF-8"), param_list[2].encode("UTF-8"), ctypes.byref(data), ctypes.byref(data_length)):
        logger.info("get_private_block return success");
        db.log.insert_one({"readed":0, "op_name": "get_private_key", "params": [param_list], "result": "success", "date":datetime.datetime.utcnow()});        
        return base64.b64encode(ctypes.string_at(data, data_length.value)).decode("UTF-8");
    db.log.insert_one({"readed":0, "op_name": "get_private_key", "params": [param_list], "result": "fail", "date":datetime.datetime.utcnow()});        
    return None

def rm_public_block(param_list):
    logger.info("rm_public_block called with params: {}".format(param_list))
    if 0==hermes_client_rm_public_block(hermes, param_list[0].encode("UTF-8"), param_list[1].encode("UTF-8"), param_list[2].encode("UTF-8"), base64.b64decode(param_list[3]), len(base64.b64decode(param_list[3]))):
        logger.info("rm_public return success");
        db.log.insert_one({"readed":0, "op_name": "rm_public_block", "params": [param_list], "result": "success", "date":datetime.datetime.utcnow()});        
        return "success"
    db.log.insert_one({"readed":0, "op_name": "rm_public_block", "params": [param_list], "result": "fail", "date":datetime.datetime.utcnow()});        
    return None

def rm_private_block(param_list):
    logger.info("rm_private_block called with params: {}".format(param_list))
    if 0==hermes_client_rm_private_block(hermes, param_list[0].encode("UTF-8"), param_list[1].encode("UTF-8"), param_list[2].encode("UTF-8"), base64.b64decode(param_list[3]), len(base64.b64decode(param_list[3]))):
        logger.info("rm_private return success");
        db.log.insert_one({"readed":0, "op_name": "rm_private_block", "params": [param_list], "result": "success", "date":datetime.datetime.utcnow()});        
        return "success"    
    db.log.insert_one({"readed":0, "op_name": "rm_private_block", "params": [param_list], "result": "fail", "date":datetime.datetime.utcnow()});        
    return None


commands={
    "get_docs_list": get_docs_list,
    "open_doc"     : read_doc,
    "new_doc_save"      : add_doc,
    "get_users_list" : get_users_list,
    "update_doc": update_doc,
    "add_public_block": add_public_block,
    "add_private_block": add_private_block,
    "add_access": add_access,
    "rm_access": rm_access,
    "get_public_block": get_public_block,
    "get_private_block": get_private_block,
    "rm_public_block" : rm_public_block,
    "rm_private_block" : rm_private_block,    
}

def save_request(param_list):
    request = db.users.find_one({'name':param_list[1]})
    if request is not None:
        return 0, "user with name \""+param_list[1]+"\" already exest"
    return 1, str(db.requests.insert_one({"name": param_list[1], "pub_key":param_list[0], "date": datetime.datetime.utcnow()}).inserted_id);
    

commands2={
    "request" : save_request
}

def get_requests_list(param_list):
    return 1, dumps(list(db.requests.find()))

def accept_request(param_list):
    request = db.requests.find_one({'_id':ObjectId(param_list[0])})
    if request is not None:
        db.requests.delete_one({'_id':ObjectId(param_list[0])})
        return 1, str(db.users.insert_one({"name": request['name'], "pub_key":binary.Binary(base64.b64decode(request['pub_key']))}).inserted_id);
    return 0, "request not found in db";

def decline_request(param_list):
    db.requests.delete_one({'_id':ObjectId(param_list[0])})
    return 1, "success"

def get_log(param_list):
    a=dumps(list(db.log.find({'readed':0})))
    db.log.update_many({'readed':0}, {'$set': {'readed':1}})
    return 1, a

commands3={
    "get_requests_list" : get_requests_list,
    "accept_request" : accept_request,
    "decline_request" : decline_request,
    "get_log": get_log
}

@asyncio.coroutine
def wshandler(request):
    logger.info('new connection')
    ws_response = web.WebSocketResponse()
    yield from ws_response.prepare(request)
    pub_key = ""
    session = ssession.ssession(b'server', server_private_key, Transport())
    while True:
        message = yield from ws_response.receive()
        if message.tp == web.MsgType.text:
            msg = session.unwrap(base64.b64decode(message.data))
            if msg.is_control:
               ws_response.send_str(base64.b64encode(msg).decode("UTF-8"));
            else:
                msg = msg.decode("UTF-8")
                logger.info('request:' + msg)
                message_params = msg.split()
                command = message_params.pop(0)
                data = commands[command](message_params)
                if data is None:
                    ws_response.send_str(base64.b64encode(session.wrap("ERROR srpc call failed".encode("UTF-8"))).decode("UTF-8"))
                    logger.info('srpc call failed')
                else:
                    ws_response.send_str(base64.b64encode(session.wrap((command+" "+data).encode("UTF-8"))).decode("UTF-8"))
        elif message.tp == web.MsgType.closed or message.tp == web.MsgType.close:
            logger.info('connection closed')
            break
    return ws_response

@asyncio.coroutine
def wshandler2(request):
    logger.info('new connection')
    ws_response = web.WebSocketResponse()
    yield from ws_response.prepare(request)
    pub_key = ""
    session = ssession.ssession(b'server', server_private_key, Transport2())
    while True:
        message = yield from ws_response.receive()
        if message.tp == web.MsgType.text:
            msg = session.unwrap(base64.b64decode(message.data))
            if msg.is_control:
               ws_response.send_str(base64.b64encode(msg).decode("UTF-8"));
            else:
                msg = msg.decode("UTF-8")
                logger.info('request:' + msg)
                message_params = msg.split()
                command = message_params.pop(0)
                status, data = commands2[command](message_params)
                if status == 0:
                    ws_response.send_str(base64.b64encode(session.wrap(("error "+data).encode("UTF-8"))).decode("UTF-8"))
                    logger.info('srpc call failed')
                else:
                    ws_response.send_str(base64.b64encode(session.wrap((command+" "+data).encode("UTF-8"))).decode("UTF-8"))
        elif message.tp == web.MsgType.closed or message.tp == web.MsgType.close:
            logger.info('connection closed')
            break
    return ws_response

@asyncio.coroutine
def wshandler3(request):
    logger.info('new connection')
    ws_response = web.WebSocketResponse()
    yield from ws_response.prepare(request)
    pub_key = ""
    session = ssession.ssession(b'server', server_private_key, Transport2())
    while True:
        message = yield from ws_response.receive()
        if message.tp == web.MsgType.text:
            msg = session.unwrap(base64.b64decode(message.data))
            if msg.is_control:
               ws_response.send_str(base64.b64encode(msg).decode("UTF-8"));
            else:
                msg = msg.decode("UTF-8")
                logger.info('request:' + msg)
                message_params = msg.split()
                command = message_params.pop(0)
                status, data = commands3[command](message_params)
                if status == 0:
                    ws_response.send_str(base64.b64encode(session.wrap(("error "+data).encode("UTF-8"))).decode("UTF-8"))
                    logger.info('srpc call failed')
                else:
                    ws_response.send_str(base64.b64encode(session.wrap((command+" "+data).encode("UTF-8"))).decode("UTF-8"))
        elif message.tp == web.MsgType.closed or message.tp == web.MsgType.close:
            logger.info('connection closed')
            break
    return ws_response

@asyncio.coroutine
@aiohttp_jinja2.template('index.html')
def index(request):
    scheme = 'wss' if request.scheme == 'https' else 'ws'
    url = '{scheme}://{host}{url}'.format(
        scheme=scheme, host=request.host,
        url=request.app.router['websocket'].url()
    )
    return {'url':url,
            'server_id': 'server',
            'server_public_key': base64.b64encode(server_public_key).decode("UTF-8"),
            'static_resolver': app.router['static'].url}

@asyncio.coroutine
@aiohttp_jinja2.template('gen_key.html')
def gen_key(request):
    scheme = 'wss' if request.scheme == 'https' else 'ws'
    url = '{scheme}://{host}{url}'.format(
        scheme=scheme, host=request.host,
        url=request.app.router['websocket2'].url()
    )
    return {'url':url,
            'server_id': 'server',
            'server_public_key': base64.b64encode(server_public_key).decode("UTF-8"),
            'static_resolver': app.router['static'].url}

@aiohttp_jinja2.template('admin.html')
def admin(request):
    scheme = 'wss' if request.scheme == 'https' else 'ws'
    url = '{scheme}://{host}{url}'.format(
        scheme=scheme, host=request.host,
        url=request.app.router['websocket3'].url()
    )
    return {'url':url,
            'server_id': 'server',
            'server_public_key': base64.b64encode(server_public_key).decode("UTF-8"),
            'static_resolver': app.router['static'].url}

@asyncio.coroutine
def init(port, loop):
    app = web.Application(loop=loop)
    app.router.add_route('GET', '/', index)
    app.router.add_route('GET', '/ws', wshandler, name='websocket')
    app.router.add_route('GET', '/ws2', wshandler2, name='websocket2')
    app.router.add_route('GET', '/ws3', wshandler3, name='websocket3')
    app.router.add_route('GET', '/gen_key', gen_key)
    app.router.add_route('GET', '/admin', admin)
    app.router.add_static('/s/', 'static/', name='static')

    aiohttp_jinja2.setup(app, loader=jinja2.FileSystemLoader('templates/'))

    handler = app.make_handler()
    srv = yield from loop.create_server(handler, '0.0.0.0', port)
    logger.info("Server started at http://0.0.0.0:{}".format(port))
    return handler, app, srv


@asyncio.coroutine
def finish(app, srv, handler):
    print(1)
    yield from asyncio.sleep(0.1)
    srv.close()
    yield from handler.finish_connections()
    yield from srv.wait_closed()
    print(1)
    hermes_client_destroy(ctypes.byref(hermes));
    print(1)
    hermes_config_destroy(config);
    print(1)



if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Run server')

    parser.add_argument('-p', '--port', type=int, help='Port number',
                        default=5103)
    parser.add_argument('-v', '--verbose', action='store_true',
                        help='Output verbosity')
    args = parser.parse_args()
    port = args.port

    logging.basicConfig(level=logging.INFO if args.verbose else logging.WARNING)
    logger = logging.getLogger(__name__)

    config=ctypes.POINTER(ctypes.c_int)
    config=hermes_config_create("hermes.ini".encode("utf-8"));
    if config is None:
        quit()

    prkey=ctypes.POINTER(ctypes.c_int)()
    prkey_length=ctypes.c_int(0)
    hermes_config_get_private_key(config, ctypes.byref(prkey), ctypes.byref(prkey_length))
    server_private_key=ctypes.string_at(prkey, prkey_length.value);

    pukey=ctypes.POINTER(ctypes.c_int)()
    pukey_length=ctypes.c_int(0)
    hermes_config_get_public_key(config, ctypes.byref(pukey), ctypes.byref(pukey_length))
    server_public_key=ctypes.string_at(pukey, pukey_length.value);

    hermes = ctypes.POINTER(ctypes.c_int)
    hermes = hermes_client_create(config);
    if hermes is None:
        quit()
    
    loop = asyncio.get_event_loop()
    handler, app, srv = loop.run_until_complete(init(port, loop))
    try:
        loop.run_forever()
    except KeyboardInterrupt:
        loop.run_until_complete(finish(app, srv, handler))
