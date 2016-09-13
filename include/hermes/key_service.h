/*
 * Copyright (c) 2016 Cossack Labs Limited
 */

#ifndef MIDHERMES_KEY_SERVICE_H_
#define MIDHERMES_KEY_SERVICE_H_

#include <hermes/config.h>
#include <hermes/key_service_protocol.h>

typedef struct key_service_client_t_ key_service_client_t;

typedef void(*key_service_client_get_key_handler_t)(void* context, const uint8_t* read_key, const size_t read_key_length, const char* owner_id);
typedef void(*key_service_client_get_res_handler_t)(void* context, const int status, const char* mess1);

key_service_client_t* key_service_client_create(const config_t* config);
int key_service_client_get_read_key(key_service_client_t* ctx, const char* block_id, key_service_client_get_key_handler_t handler, void* context);
int key_service_client_get_update_key(key_service_client_t* ctx, const char* block_id, key_service_client_get_key_handler_t handler, void* context);
int key_service_client_grand_self_read_access(key_service_client_t* ctx, const char* block_id, const uint8_t* key, const size_t key_length, key_service_client_get_res_handler_t handler, void* context);
int key_service_client_grand_read_access(key_service_client_t* ctx, const char* user_id, const char* block_id, const uint8_t* user_pub_key, const size_t user_pub_key_length, key_service_client_get_res_handler_t handler, void* context);
int key_service_client_grand_update_access(key_service_client_t* ctx, const char* user_id, const char* block_id, const uint8_t* user_pub_key, const size_t user_pub_key_length, key_service_client_get_res_handler_t handler, void* context);
int key_service_client_revoke_read_access(key_service_client_t* ctx, const char* user_id, const char* block_id, key_service_client_get_res_handler_t handler, void* context);
int key_service_client_revoke_update_access(key_service_client_t* ctx, const char* user_id, const char* block_id, key_service_client_get_res_handler_t handler, void* context);
int key_service_client_destroy(key_service_client_t* ctx);

#endif /* MIDHERMES_KEY_SERVICE_H_ */
