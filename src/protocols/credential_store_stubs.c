/*
 * Copyright (c) 2015 Cossack Labs Limited
 */

#include <stdlib.h>
#include <string.h>
#include <hermes/utils.h>
#include <hermes/credential_store_protocol.h>
#include <hermes/functions_collection.h>

typedef struct{
  void* handler_pointer_;
  void* call_cxt_;
} credential_store_handler_ctx_t;

credential_store_handler_ctx_t* credential_store_handler_ctx_create(void* handler_pointer, void* call_cxt){
  credential_store_handler_ctx_t* ctx=malloc(sizeof(credential_store_handler_ctx_t));
  HERMES_CHECK(ctx, return NULL);
  ctx->handler_pointer_=handler_pointer;
  ctx->call_cxt_= call_cxt;
  return ctx;
}

function_t hermes__credential_store_functions[]={{"get_public_key", credential_store_get_public_key_stub},
						 {"get_users_list", credential_store_get_users_list_stub}};

functions_collection_t hermes__credential_store_functions_collection={2,hermes__credential_store_functions};

HERMES_FUNC(credential_store, get_public_key)
HERMES_FUNC(credential_store, get_users_list)

