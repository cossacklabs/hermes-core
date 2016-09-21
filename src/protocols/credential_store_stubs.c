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

