/*
 * Copyright (c) 2015 Cossack Labs Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
srpc_status_t srpc_ctx_bind(srpc_ctx_t* ctx, const char* encpoint, functions_collection_t* coll, void* call_ctx);
srpc_status_t srpc_ctx_connect(srpc_ctx_t* ctx, const char* endpoint, const char* id);
srpc_status_t srpc_ctx_sync_connect(srpc_ctx_t* ctx, const char* endpoint, const char* id);
srpc_status_t srpc_ctx_call(srpc_ctx_t* ctx, const uint8_t* packed_params, const size_t packed_params_length, call_handler_t handler, void* call_ctx);
srpc_status_t srpc_ctx_sync_call(srpc_ctx_t* ctx, const uint8_t* packed_params, const size_t packed_params_length, uint8_t** res, size_t* res_len);
srpc_status_t srpc_ctx_destroy(srpc_ctx_t* ctx);

const char* srpc_ctx_get_user_id(srpc_ctx_t* ctx);


#endif /* SRPC_RPC_CTX_H_ */
