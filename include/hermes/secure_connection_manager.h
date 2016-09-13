/*
 * Copyright (c) 2015 Cossack Labs Limited
 */

#ifndef SRPC_SECURE_CONNECTION_MANAGER_H_
#define SRPC_SECURE_CONNECTION_MANAGER_H_

#include <stdint.h>
#include <stdlib.h>

typedef int secure_connection_manager_status_t; 

#define SECURE_CONNECTION_MANAGER_SUCCESS 0
#define SECURE_CONNECTION_MANAGER_FAIL -1
#define SECURE_CONNECTION_MANAGER_INVALID_PARAM -2

typedef struct secure_connection_manager_t secure_connection_manager_t;

#define DEFAULT_THREADS_COUNT 10

typedef int (*public_key_by_id_geter_t)(const uint8_t* id, const size_t id_length, uint8_t* public_key, size_t public_key_length, void* user_data);
typedef void (*secure_server_function_t)(void* user_data, const uint8_t* request, const size_t request_length, uint8_t** response, size_t* response_length);
typedef void (*secure_response_handler_t)(void* user_data, const uint8_t* response, const size_t response_length);

secure_connection_manager_t* secure_connection_manager_create_with_treads(const uint8_t* id, const size_t id_length, const uint8_t* private_key, const size_t private_key_length, public_key_by_id_geter_t pub_key_geter, const uint64_t threads_count);
secure_connection_manager_t* secure_connection_manager_create(const uint8_t* id, const size_t id_length, const uint8_t* private_key, const size_t private_key_length, public_key_by_id_geter_t pub_key_geter);
secure_connection_manager_status_t secure_connection_manager_destroy(secure_connection_manager_t* manager);
secure_connection_manager_status_t secure_connection_manager_bind(secure_connection_manager_t* manager, const char* endpoint, secure_server_function_t server_function, void* user_data);
secure_connection_manager_status_t secure_connection_manager_connect(secure_connection_manager_t* manager, const char* endpoint);
secure_connection_manager_status_t secure_connection_manager_send_request(secure_connection_manager_t* manager, const uint8_t* request, const size_t request_length, secure_response_handler_t response_handler, void* user_data);



#endif /* SRPC_SECURE_CONNECTION_MANAGER_H_ */
