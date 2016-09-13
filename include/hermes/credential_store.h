/*
 * Copyright (c) 2016 Cossack Labs Limited
 */

#ifndef MIDHERMES_CREDENTIAL_STORE_H_
#define MIDHERMES_CREDENTIAL_STORE_H_

#include "config.h"
#include "credential_store_protocol.h"

typedef credential_store_t credential_store_client_t;

credential_store_client_t* credential_store_client_create(const config_t* config);
int credential_store_client_get_pub_key(credential_store_client_t* ctx, const char* id, uint8_t** key, size_t* key_length);
int credential_store_client_get_users_list(credential_store_client_t* ctx, const char* id, char** res);
int credential_store_client_destroy(credential_store_client_t* ctx);

#endif /* MIDHERMES_CREDENTIAL_STORE_H_ */
