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
#include <hermes/record_set.h>
#include <hermes/functions_collection.h>

typedef struct{
  void* handler_pointer_;
  void* call_cxt_;
} record_set_handler_ctx_t;

record_set_handler_ctx_t* record_set_handler_ctx_create(void* handler_pointer, void* call_cxt){
  record_set_handler_ctx_t* ctx=malloc(sizeof(record_set_handler_ctx_t));
  HERMES_CHECK(ctx, return NULL);
  ctx->handler_pointer_=handler_pointer;
  ctx->call_cxt_= call_cxt;
  return ctx;
}

function_t record_set_functions[]={{"get_doc_breaf", record_set_get_doc_breaf_stub},
				    {"get_doc", record_set_get_doc_stub},
				    {"get_doc_data", record_set_get_doc_data_stub},
				    {"get_public_block", record_set_get_public_block_stub},
				    {"get_private_block", record_set_get_private_block_stub},
				    {"add_document", record_set_add_document_stub},
				    {"update_document", record_set_update_document_stub},
				    {"update_public_block", record_set_update_public_block_stub},
				    {"update_private_block", record_set_update_private_block_stub},
				    {"add_public_block", record_set_add_public_block_stub},
				    {"add_private_block", record_set_add_private_block_stub},
				    {"delete_public_block", record_set_delete_public_block_stub},
				    {"delete_private_block", record_set_delete_private_block_stub},
				    {"delete_document", record_set_delete_document_stub},
				    {"add_access", record_set_add_access_stub},
				    {"rm_access", record_set_rm_access_stub},
				    {"get_docs", record_set_get_docs_stub}};

functions_collection_t record_set_functions_collection={17,record_set_functions};


HERMES_FUNC(record_set, get_doc_breaf);
HERMES_FUNC(record_set, get_doc);
HERMES_FUNC(record_set, get_doc_data);
HERMES_FUNC(record_set, get_public_block);
HERMES_FUNC(record_set, get_private_block);
HERMES_FUNC(record_set, add_document);
HERMES_FUNC(record_set, update_document);
HERMES_FUNC(record_set, update_public_block);
HERMES_FUNC(record_set, update_private_block);
HERMES_FUNC(record_set, add_public_block);
HERMES_FUNC(record_set, add_private_block);
HERMES_FUNC(record_set, delete_public_block);
HERMES_FUNC(record_set, delete_private_block);
HERMES_FUNC(record_set, delete_document);
HERMES_FUNC(record_set, get_docs);
HERMES_FUNC(record_set, add_access);
HERMES_FUNC(record_set, rm_access);
