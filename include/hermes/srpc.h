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

#ifndef SRPC_RPC_CTX_H_
#define SRPC_RPC_CTX_H_

#include "functions_collection.h"

typedef int srpc_status_t;

#define SRPC_SUCCESS 0
#define SRPC_FAIL -1
#define SRPC_INVALID_PARAM -2

typedef struct srpc_ctx_t srpc_ctx_t;
typedef void (*call_handler_t)(void* ctx, const uint8_t* packed_result, const size_t packed_result_length);


srpc_ctx_t* srpc_ctx_create();
srpc_status_t srpc_ctx_bind(srpc_ctx_t* ctx, functions_collection_t* coll, void* call_ctx);
srpc_status_t srpc_ctx_connect(srpc_ctx_t* ctx, const char* endpoint, const char* id);
srpc_status_t srpc_ctx_sync_connect(srpc_ctx_t* ctx, const char* endpoint, const char* id);
srpc_status_t srpc_ctx_call(srpc_ctx_t* ctx, const uint8_t* packed_params, const size_t packed_params_length, call_handler_t handler, void* call_ctx);
srpc_status_t srpc_ctx_sync_call(srpc_ctx_t* ctx, const uint8_t* packed_params, const size_t packed_params_length, uint8_t** res, size_t* res_len);
srpc_status_t srpc_ctx_destroy(srpc_ctx_t* ctx);

const char* srpc_ctx_get_user_id(srpc_ctx_t* ctx);


#endif /* SRPC_RPC_CTX_H_ */
