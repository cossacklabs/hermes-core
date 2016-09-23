/*
 * Copyright (c) 2016 Cossack Labs Limited
 *
 * This file is part of Hermes.
 *
 * Hermes is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Hermes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Hermes.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef PROTOCOLS_CREDENTIAL_STORE_H_
#define PROTOCOLS_CREDENTIAL_STORE_H_

#include "srpc.h"
#include "functions_collection.h"
#include "protocols.h"
#include "stubs_generator.h"

typedef struct{
  srpc_ctx_t* srpc_ctx_;
  uint8_t* tmp_buf_;
  size_t tmp_buf_length_;
}credential_store_t;

credential_store_t* credential_store_create();
protocol_status_t credential_store_bind(credential_store_t* credential_store_ctx, void* ctx);
protocol_status_t credential_store_connect(credential_store_t* ctx, const char* endpoint, const char* id);
protocol_status_t credential_store_call(credential_store_t* ctx, const char* func_name, const uint8_t* param_buf, const size_t param_buf_length, void (*proxy_handler)(void* ctx, const uint8_t* res_buf, const size_t res_buf_length), void* call_ctx);
protocol_status_t credential_store_sync_call(credential_store_t* ctx, const char* func_name, const uint8_t* param_buf, const size_t param_buf_length, uint8_t** res_buf, size_t* res_buf_length);
protocol_status_t credential_store_destroy(credential_store_t* ctx);

HERMES_FUNCTION_DEFINITIONS(credential_store, get_public_key);
HERMES_FUNCTION_DEFINITIONS(credential_store, get_users_list);

extern functions_collection_t hermes__credential_store_functions_collection;

#endif /* PROTOCOLS_CREDENTIAL_STORE_H_ */
