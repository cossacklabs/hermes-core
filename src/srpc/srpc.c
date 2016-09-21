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

#include <stdint.h>
#include <hermes/utils.h>

#include <hermes/srpc.h>

#ifndef SRPC_THREADS_COUNT
#define SRPC_THREADS_COUNT 3
#endif
#include <hermes/connection_manager.h>


#define MAX_USER_ID_LENGTH 256

srpc_status_t srpc_ctx_destroy(srpc_ctx_t* ctx);

struct srpc_ctx_t{
  connection_manager_t* conn_manager_;
  functions_collection_t* server_functions_;
  char user_id_[MAX_USER_ID_LENGTH];
  void* call_ctx_;
};

srpc_status_t srpc_ctx_destroy(srpc_ctx_t* ctx){
  HERMES_CHECK(ctx, return SRPC_INVALID_PARAM);
  if(ctx->conn_manager_)
    connection_manager_destroy(ctx->conn_manager_);
  free(ctx);
  return SRPC_SUCCESS;
}

srpc_ctx_t* srpc_ctx_create(){
  srpc_ctx_t* ctx=malloc(sizeof(srpc_ctx_t));
  HERMES_CHECK(ctx, return NULL);
  ctx->conn_manager_=connection_manager_create(SRPC_THREADS_COUNT);
  HERMES_CHECK(ctx->conn_manager_, srpc_ctx_destroy(ctx); return NULL);
  ctx->server_functions_=NULL;
  ctx->call_ctx_=NULL;
  memset(ctx->user_id_, 0, MAX_USER_ID_LENGTH);
  return ctx;  
}

void srpc_server_func(void* ctx, const char* id, const uint8_t* request, const size_t request_length, uint8_t** response, size_t* response_length){
  *response_length=0;
  HERMES_CHECK(ctx, return);
  HERMES_CHECK(FUNCTIONS_COLLECTION_SUCCESS==functions_collection_exec(((srpc_ctx_t*)(ctx))->server_functions_, id, request, request_length, response, response_length, ((srpc_ctx_t*)(ctx))->call_ctx_), *response_length=0; return);
}

srpc_status_t srpc_ctx_bind(srpc_ctx_t* ctx, const char* endpoint, functions_collection_t* coll, void* call_ctx){
  HERMES_CHECK(ctx && endpoint && coll && ctx->conn_manager_, return SRPC_INVALID_PARAM);
  ctx->server_functions_=coll;
  ctx->call_ctx_=call_ctx;
  HERMES_CHECK(CONNECTION_MANAGER_SUCCESS==connection_manager_bind(ctx->conn_manager_, endpoint, srpc_server_func, ctx), return SRPC_FAIL);
  return SRPC_SUCCESS;
}

srpc_status_t srpc_ctx_connect(srpc_ctx_t* ctx, const char* endpoint, const char* id){
  HERMES_CHECK(ctx && endpoint && ctx->conn_manager_, return SRPC_INVALID_PARAM);
  HERMES_CHECK(CONNECTION_MANAGER_SUCCESS==connection_manager_connect(ctx->conn_manager_, endpoint, id), return SRPC_FAIL);
  return SRPC_SUCCESS;  
}

srpc_status_t srpc_ctx_sync_connect(srpc_ctx_t* ctx, const char* endpoint, const char* id){
  HERMES_CHECK(ctx && endpoint && ctx->conn_manager_, return SRPC_INVALID_PARAM);
  HERMES_CHECK(CONNECTION_MANAGER_SUCCESS==connection_manager_sync_connect(ctx->conn_manager_, endpoint, id), return SRPC_FAIL);
  return SRPC_SUCCESS;  
}

srpc_status_t srpc_ctx_call(srpc_ctx_t* ctx, const uint8_t* packed_params, const size_t packed_params_length, call_handler_t handler, void* call_ctx){
  HERMES_CHECK(ctx && packed_params && packed_params_length>0 && handler && ctx->conn_manager_, return SRPC_INVALID_PARAM);
  HERMES_CHECK(CONNECTION_MANAGER_SUCCESS==connection_manager_send_request(ctx->conn_manager_, packed_params, packed_params_length, handler, (void*)call_ctx), return SRPC_FAIL);
  return SRPC_SUCCESS;    
}

srpc_status_t srpc_ctx_sync_call(srpc_ctx_t* ctx, const uint8_t* packed_params, const size_t packed_params_length, uint8_t** res, size_t* res_len){
  HERMES_CHECK(ctx && packed_params && packed_params_length>0 && ctx->conn_manager_, return SRPC_INVALID_PARAM);
  HERMES_CHECK(CONNECTION_MANAGER_SUCCESS==connection_manager_send_sync_request(ctx->conn_manager_, packed_params, packed_params_length, res, res_len), return SRPC_FAIL);
  return SRPC_SUCCESS;    
}

const char* srpc_ctx_get_user_id(srpc_ctx_t* ctx){
    HERMES_CHECK(ctx, return NULL);
    return ctx->user_id_;
}

