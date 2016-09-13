/*
 * Copyright (c) 2015 Cossack Labs 
 */

#include <stdlib.h>
#include <string.h>
#include <hermes/utils.h>
#include <hermes/record_set.h>

protocol_status_t record_set_destroy(record_set_t* ctx);

record_set_t* record_set_create(){
  record_set_t* serv=malloc(sizeof(record_set_t));
  HERMES_CHECK(serv, return NULL);
  serv->srpc_ctx_=srpc_ctx_create();
  HERMES_CHECK(serv->srpc_ctx_, record_set_destroy(serv); return NULL);
  serv->tmp_buf_=NULL;
  serv->tmp_buf_length_=0;
  return serv;
}

protocol_status_t record_set_bind(record_set_t* ctx, const char* endpoint, void* call_ctx){
  HERMES_CHECK(ctx && ctx->srpc_ctx_ && endpoint, return PROTOCOL_INVALID_PARAM);
  return srpc_ctx_bind(ctx->srpc_ctx_, endpoint, &record_set_functions_collection, (void*)call_ctx);
}

protocol_status_t record_set_connect(record_set_t* ctx, const char* endpoint, const char* id){
  HERMES_CHECK(ctx && ctx->srpc_ctx_ && endpoint, return PROTOCOL_INVALID_PARAM);  
  return srpc_ctx_connect(ctx->srpc_ctx_, endpoint, id);
}

protocol_status_t record_set_sync_connect(record_set_t* ctx, const char* endpoint, const char* id){
  HERMES_CHECK(ctx && ctx->srpc_ctx_ && endpoint, return PROTOCOL_INVALID_PARAM);  
  return srpc_ctx_sync_connect(ctx->srpc_ctx_, endpoint, id);
}

protocol_status_t record_set_call(record_set_t* ctx, const char* func_name, const uint8_t* param_buf, const size_t param_buf_length, void (*proxy_handler)(void* ctx, const uint8_t* res_buf, const size_t res_buf_length), void* call_ctx){
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

protocol_status_t record_set_sync_call(record_set_t* ctx, const char* func_name, const uint8_t* param_buf, const size_t param_buf_length, uint8_t** res_buf, size_t* res_buf_length){
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


protocol_status_t record_set_destroy(record_set_t* ctx){
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

