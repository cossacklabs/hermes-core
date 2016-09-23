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

#include <stdlib.h>
#include <string.h>
#include <hermes/utils.h>
#include <hermes/key_service.h>

protocol_status_t key_service_destroy(key_service_t* ctx);

key_service_t* key_service_create(){
  key_service_t* serv=malloc(sizeof(key_service_t));
  HERMES_CHECK(serv, return NULL);
  serv->srpc_ctx_=srpc_ctx_create();
  HERMES_CHECK(serv->srpc_ctx_, key_service_destroy(serv); return NULL);
  serv->tmp_buf_=NULL;
  serv->tmp_buf_length_=0;
  return serv;
}

protocol_status_t key_service_bind(key_service_t* ctx, void* call_ctx){
  HERMES_CHECK(ctx && ctx->srpc_ctx_, return PROTOCOL_INVALID_PARAM);
  return srpc_ctx_bind(ctx->srpc_ctx_, &hermes__key_service_functions_collection, (void*)call_ctx);
}

protocol_status_t key_service_connect(key_service_t* ctx, const char* endpoint, const char* id){
  HERMES_CHECK(ctx && ctx->srpc_ctx_ && endpoint, return PROTOCOL_INVALID_PARAM);  
  return srpc_ctx_connect(ctx->srpc_ctx_, endpoint, id);
}

protocol_status_t key_service_sync_connect(key_service_t* ctx, const char* endpoint, const char* id){
  HERMES_CHECK(ctx && ctx->srpc_ctx_ && endpoint, return PROTOCOL_INVALID_PARAM);  
  return srpc_ctx_sync_connect(ctx->srpc_ctx_, endpoint, id);
}

protocol_status_t key_service_call(key_service_t* ctx, const char* func_name, const uint8_t* param_buf, const size_t param_buf_length, void (*proxy_handler)(void* ctx, const uint8_t* res_buf, const size_t res_buf_length), void* call_ctx){
  HERMES_CHECK(ctx && ctx->srpc_ctx_ && func_name && param_buf && param_buf_length!=0 && proxy_handler, return PROTOCOL_INVALID_PARAM);
  if((strlen(func_name)+1+param_buf_length)>ctx->tmp_buf_length_){
    ctx->tmp_buf_length_=(strlen(func_name)+1+param_buf_length);
    ctx->tmp_buf_=realloc(ctx->tmp_buf_, ctx->tmp_buf_length_);
    HERMES_CHECK(ctx->tmp_buf_, return PROTOCOL_BAD_ALLOC);    
  }
  memcpy(ctx->tmp_buf_, func_name, strlen(func_name)+1);
  memcpy(ctx->tmp_buf_+strlen(func_name)+1, param_buf, param_buf_length);
  return srpc_ctx_call(ctx->srpc_ctx_, ctx->tmp_buf_, strlen(func_name)+1+param_buf_length, proxy_handler, call_ctx);
}

protocol_status_t key_service_sync_call(key_service_t* ctx, const char* func_name, const uint8_t* param_buf, const size_t param_buf_length, uint8_t** res_buf, size_t* res_buf_length){
  HERMES_CHECK(ctx && ctx->srpc_ctx_ && func_name && param_buf && param_buf_length!=0, return PROTOCOL_INVALID_PARAM);
  if((strlen(func_name)+1+param_buf_length)>ctx->tmp_buf_length_){
    ctx->tmp_buf_length_=(strlen(func_name)+1+param_buf_length);
    ctx->tmp_buf_=realloc(ctx->tmp_buf_, ctx->tmp_buf_length_);
    HERMES_CHECK(ctx->tmp_buf_, return PROTOCOL_BAD_ALLOC);    
  }
  memcpy(ctx->tmp_buf_, func_name, strlen(func_name)+1);
  memcpy(ctx->tmp_buf_+strlen(func_name)+1, param_buf, param_buf_length);
  return srpc_ctx_sync_call(ctx->srpc_ctx_, ctx->tmp_buf_, strlen(func_name)+1+param_buf_length, res_buf, res_buf_length);
}

protocol_status_t key_service_destroy(key_service_t* ctx){
  HERMES_CHECK(ctx, return PROTOCOL_INVALID_PARAM);
  if(ctx->srpc_ctx_){
    srpc_ctx_destroy(ctx->srpc_ctx_);
  }
  if(ctx->tmp_buf_){
    free(ctx->tmp_buf_);
  }
  free(ctx);
  return PROTOCOL_SUCCESS;
}

const char* key_service_get_user_id(key_service_t* ctx){
    HERMES_CHECK(ctx && ctx->srpc_ctx_, return NULL);
    return srpc_ctx_get_user_id(ctx->srpc_ctx_);
}