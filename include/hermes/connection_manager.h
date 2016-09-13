/*
 * Copyright (c) 2015 Cossack Labs Limited
 */

#ifndef SRPC_CONNECTION_MANAGER_H_
#define SRPC_CONNECTION_MANAGER_H_

#include <czmq.h>

typedef int connection_manager_status_t;

#define CONNECTION_MANAGER_SUCCESS 0
#define CONNECTION_MANAGER_FAIL -1
#define CONNECTION_MANAGER_BAD_ALLOC -2
#define CONNECTION_MANAGER_INVALID_PARAM -3

typedef struct connection_manager_t connection_manager_t;

typedef void (*server_function_t)(void* user_data, const char* id, const uint8_t* request, const size_t request_length, uint8_t** response, size_t* response_length);
typedef void (*response_handler_t)(void* user_data, const uint8_t* response, const size_t response_length);

connection_manager_t* connection_manager_create(uint64_t threads_count);
connection_manager_status_t connection_manager_destroy(connection_manager_t* manager);
connection_manager_status_t connection_manager_bind(connection_manager_t* manager, const char* endpoint, server_function_t server_function, void* user_data);
connection_manager_status_t connection_manager_connect(connection_manager_t* manager, const char* endpoint, const char* id);
connection_manager_status_t connection_manager_send_request(connection_manager_t* manager, const uint8_t* request, const size_t request_length, response_handler_t response_handler, void* user_data);
connection_manager_status_t connection_manager_send_sync_request(connection_manager_t* manager, const uint8_t* request, const size_t request_length, uint8_t** response, size_t* response_length);


#endif /* SRPC_CONNECTION_MANAGER_H_ */
