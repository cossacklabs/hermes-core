/*
 * Copyright (c) 2015 Cossack Labs Limited
 */

#include <stdlib.h>
#include <string.h>
#include <hermes/utils.h>
#include <hermes/key_service.h>
#include <hermes/functions_collection.h>

typedef struct{
  void* handler_pointer_;
  void* call_cxt_;
} key_service_handler_ctx_t;

key_service_handler_ctx_t* key_service_handler_ctx_create(void* handler_pointer, void* call_cxt){
  key_service_handler_ctx_t* ctx=malloc(sizeof(key_service_handler_ctx_t));
  HERMES_CHECK(ctx, return NULL);
  ctx->handler_pointer_=handler_pointer;
  ctx->call_cxt_= call_cxt;
  return ctx;
}

function_t hermes__key_functions[]={{"get_read_key", key_service_get_read_key_stub},
				    {"get_update_key", key_service_get_update_key_stub},
				    {"grant_read_access",key_service_grant_read_access_stub},
				    {"grant_update_access", key_service_grant_update_access_stub},
				    {"revoke_read_access", key_service_revoke_read_access_stub},
				    {"revoke_update_access", key_service_revoke_update_access_stub}};

functions_collection_t hermes__key_service_functions_collection={6,hermes__key_functions};

HERMES_FUNC(key_service, get_read_key)
HERMES_FUNC(key_service, get_update_key)
HERMES_FUNC(key_service, grant_read_access)
HERMES_FUNC(key_service, grant_update_access)
HERMES_FUNC(key_service, revoke_read_access)
HERMES_FUNC(key_service, revoke_update_access)

