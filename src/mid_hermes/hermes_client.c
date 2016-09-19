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

#include <hermes/hermes_client.h>
#include <hermes/hermes_user.h>
#include <hermes/utils.h>

#include <stdlib.h>

struct hermes_client_t_{
  hermes_user_t* user;
}; 

hermes_client_t* hermes_client_create(const char* user_id, const uint8_t* private_key, const size_t private_key_length){
  hermes_client_t* client = calloc(sizeof(hermes_client_t), 1);
  HERMES_CHECK(client, return NULL);
  client->user = hermes_user_create(user_id, private_key, private_key_length);
  HERMES_CHECK(client->user, hermes_client_destroy(&client); return NULL);
  return client;
}

void hermes_client_destroy(hermes_client_t** ctx){
  HERMES_CHECK(*ctx, return);
  if((*ctx)->user)
    hermes_user_destroy(&((*ctx)->user));
  free(*ctx);
  *ctx = NULL;
}

//Document CRUD
int hermes_client_create_document(hermes_client_t* ctx, const uint8_t* data, const size_t data_length, char** new_id){
  return hermes_client_create_document_block(ctx, NULL, false, data, data_length, new_id);
}

int hermes_client_read_document(hermes_client_t* ctx, const char* doc_id, uint8_t** data, size_t* data_length){
  return hermes_client_read_document_block(ctx, doc_id, doc_id, data, data_length);  
}

int hermes_client_update_document(hermes_client_t* ctx, const char* doc_id, const uint8_t* data, const size_t data_length){
  return hermes_client_update_document_block(ctx, doc_id, doc_id, data, data_length);
}

int hermes_client_delete_document(hermes_client_t* ctx, const char* doc_id){
  return hermes_client_delete_document_block(ctx, doc_id, doc_id); 
}

//block CRUD
int hermes_client_create_document_block(hermes_client_t* ctx, const char* doc_id, bool is_private, const uint8_t* data, const size_t data_length, char** new_block_id){
  HERMES_CHECK(ctx && ctx->user && data && data_length, return -2);
  hermes_document_block_t* block = hermes_document_block_create_new(doc_id, "new doc block", is_private, data, data_length);
  HERMES_CHECK(block, return -1);
  HERMES_CHECK(0 == hermes_user_save_block(ctx->user, block), hermes_document_block_destroy(&block); return -1);
  *new_block_id = malloc(strlen(block->id)+1);
  HERMES_CHECK(*new_block_id, hermes_document_block_destroy(&block); return -1);
  memcpy(*new_block_id, block->id, strlen(block->id)+1);
  hermes_document_block_destroy(&block);
  return 0;
}

int hermes_client_read_document_block(hermes_client_t* ctx, const char* doc_id, const char* block_id, uint8_t** data, size_t* data_length){
  HERMES_CHECK(ctx && ctx->user && data && data_length, return -2);
  hermes_document_block_t* block = NULL;
  HERMES_CHECK(0 == hermes_user_load_block(ctx->user, doc_id, block_id, &block), return -1);
  *data = malloc(block->data_length);
  HERMES_CHECK(*data, hermes_document_block_destroy(&block); return -4);
  memcpy(*data, block->data, block->data_length);
  *data_length = block->data_length;
  hermes_document_block_destroy(&block);
  return 0;
}

int hermes_client_update_document_block(hermes_client_t* ctx, const char* doc_id, const char* block_id, const uint8_t* data, const size_t data_length){
  HERMES_CHECK(ctx && ctx->user && data && data_length, return -2);
  hermes_document_block_t* block = NULL;
  HERMES_CHECK(0 == hermes_user_load_block(ctx->user, doc_id, block_id, &block), return -1);
  HERMES_CHECK(0 == hermes_document_block_set_data(block, data, data_length), hermes_document_block_destroy(&block); return -1);
  HERMES_CHECK(0 == hermes_user_save_block(ctx->user, block), hermes_document_block_destroy(&block); return -1);
  hermes_document_block_destroy(&block);
  return 0;
}

int hermes_client_delete_document_block(hermes_client_t* ctx, const char* doc_id, const char* block_id){
  HERMES_CHECK(ctx && ctx->user && doc_id && block_id, return -2);
  hermes_document_block_t* block = NULL;
  HERMES_CHECK(0 == hermes_user_load_block(ctx->user, doc_id, block_id, &block), return -1);
  HERMES_CHECK(0 == hermes_user_delete_block(ctx->user, block), hermes_document_block_destroy(&block); return -1);
  hermes_document_block_destroy(&block);
  return 0;  
}

//manipulate user rights
int hermes_client_grant_access_to_document(hermes_client_t* ctx, const char* doc_id, const char* user_id_granting_to, int rights_mask){
  return hermes_client_grant_access_to_document_block(ctx, doc_id, doc_id, user_id_granting_to, rights_mask);
}

int hermes_client_deny_access_to_document(hermes_client_t* ctx, const char* doc_id, const char* user_id_denying_to){
  return hermes_client_deny_access_to_document_block(ctx, doc_id, doc_id, user_id_denying_to);
}

int hermes_client_grant_access_to_document_block(hermes_client_t* ctx, const char* doc_id, const char* block_id, const char* user_id_granting_to, int rights_mask){
  HERMES_CHECK(ctx && ctx->user && doc_id && block_id && user_id_granting_to, return -2);
  hermes_document_block_t* block = NULL;
  HERMES_CHECK(0 == hermes_user_load_block(ctx->user, doc_id, block_id, &block), return -1);
  HERMES_CHECK(0 == hermes_user_grand_access_to_block(ctx->user, user_id_granting_to, block, rights_mask), hermes_document_block_destroy(&block); return -1);
  hermes_document_block_destroy(&block);
  return 0;
}

int hermes_client_deny_access_to_document_block(hermes_client_t* ctx, const char* doc_id, const char* block_id, const char* user_id_denying_to){
  HERMES_CHECK(ctx && ctx->user && doc_id && block_id && user_id_denying_to, return -2);
  hermes_document_block_t* block = NULL;
  HERMES_CHECK(0 == hermes_user_load_block(ctx->user, doc_id, block_id, &block), return -1);
  HERMES_CHECK(0 == hermes_user_deny_access_to_block(ctx->user, user_id_denying_to, block), hermes_document_block_destroy(&block); return -1);
  hermes_document_block_destroy(&block);
  return 0;
}
