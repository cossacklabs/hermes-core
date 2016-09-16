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


#include <hermes/utils.h>
#include <hermes/buffer.h>
#include <hermes/mid_hermes.h>
#include <hermes/hermes_document_block.h>
#include <themis/themis.h>

#include <string.h>

char* str(const char* str){
  if(str){
    char* str_ = calloc(strlen(str)+1,1);
    HERMES_CHECK(str_, return NULL);
    memcpy(str_, str, strlen(str)+1);
    return str_;
  }
  return NULL;
}

uint8_t* blob(const uint8_t* blob, const size_t blob_size){
  HERMES_CHECK(blob, return NULL);
  uint8_t* blob_=calloc(blob_size,1);
  HERMES_CHECK(blob_, return NULL);
  memcpy(blob_, blob, blob_size);
  return blob_;
}

hermes_document_block_t* hermes_document_block_create_new(const char* doc_id, const char* name, const bool is_private, const uint8_t* data, const size_t data_length){
  hermes_document_block_t *block=calloc(sizeof(hermes_document_block_t),1);
  HERMES_CHECK(block, return NULL);
  block->doc_id = str(doc_id);
  HERMES_CHECK(block->name = str(name), hermes_document_block_destroy(&block); return NULL);
  block->is_private = is_private;
  block->data = blob(data, data_length);
  HERMES_CHECK(block->data, hermes_document_block_destroy(&block); return NULL);
  block->data_length = data_length;
  block->is_changed=true;
  return block;
}

hermes_document_block_t* hermes_document_block_create(const char* owner_id, const char* doc_id, const char* name, const bool is_private, const uint8_t* data, const size_t data_length){
  hermes_document_block_t *block=calloc(sizeof(hermes_document_block_t),1);
  HERMES_CHECK(block, return NULL);
  block->doc_id = str(doc_id);
  HERMES_CHECK(block->name = str(name), hermes_document_block_destroy(&block); return NULL);
  HERMES_CHECK(block->owner_id = str(owner_id), hermes_document_block_destroy(&block); return NULL);
  HERMES_CHECK(block->user_id = str(owner_id), hermes_document_block_destroy(&block); return NULL);
  block->is_private = is_private;
  HERMES_CHECK(block->data = blob(data, data_length), hermes_document_block_destroy(&block); return NULL);
  block->data_length = data_length;
  block->is_changed=true;
  return block;
}

hermes_document_block_t* hermes_document_block_create_from_blob(const uint8_t* blob, const size_t blob_length){
  buffer_t* buf = buffer_create_with_data(blob, blob_length);
  HERMES_CHECK(buf, return NULL);
  const char *name, *owner_id;
  const uint8_t* data;
  uint32_t is_private=0;
  size_t data_length;
  HERMES_CHECK(BUFFER_SUCCESS == buffer_pop_string(buf, &name), buffer_destroy(buf); return NULL);
  HERMES_CHECK(BUFFER_SUCCESS == buffer_pop_string(buf, &owner_id), buffer_destroy(buf); return NULL);
  HERMES_CHECK(BUFFER_SUCCESS == buffer_pop_status(buf, &is_private), buffer_destroy(buf); return NULL);
  HERMES_CHECK(BUFFER_SUCCESS == buffer_pop_data(buf, &data, &data_length), buffer_destroy(buf); return NULL);
  hermes_document_block_t* block = hermes_document_block_create(owner_id, NULL, name, is_private, data, data_length);
  HERMES_CHECK(block, buffer_destroy(buf); return NULL);
  buffer_destroy(buf);
  block->is_loaded=true;
  block->is_changed=false;
  return block;
}

void hermes_document_block_destroy(hermes_document_block_t** block){
  HERMES_CHECK(*block, return);
  free((*block)->id);
  free((*block)->doc_id);
  free((*block)->owner_id);
  free((*block)->user_id);
  free((*block)->name);
  free((*block)->data);
  free((*block)->read_token);
  free((*block)->update_token);
  free((*block)->mac);
  free((*block)->new_mac);
  free(*block);
  (*block)=NULL;
}

int hermes_document_block_set_data(hermes_document_block_t* block, const uint8_t* data, const size_t data_length){
  HERMES_CHECK(block && data && data_length, return -2);
  block->data = realloc(block->data, data_length);
  HERMES_CHECK(block->data, return -1);
  memcpy(block->data, data, data_length);
  block->data_length = data_length;
  block->is_changed = true;
  return 0;  
}

int hermes_document_block_to_blob(hermes_document_block_t* block, uint8_t** blob, size_t* blob_length){
  HERMES_CHECK(block, return -1);
  buffer_t* buf = buffer_create();
  HERMES_CHECK(buf, return -4);
  HERMES_CHECK(BUFFER_SUCCESS == buffer_push_string(buf, block->name), buffer_destroy(buf); return -1);
  HERMES_CHECK(BUFFER_SUCCESS == buffer_push_string(buf, block->owner_id), buffer_destroy(buf); return -1);
  HERMES_CHECK(BUFFER_SUCCESS == buffer_push_status(buf, (block->is_private)?1:0), buffer_destroy(buf); return -1);
  HERMES_CHECK(BUFFER_SUCCESS == buffer_push_data(buf, block->data, block->data_length), buffer_destroy(buf); return -1);
  HERMES_CHECK(BUFFER_SUCCESS == buffer_push_data(buf, block->new_mac, block->new_mac_length), buffer_destroy(buf); return -1);
  *blob_length = buffer_get_size(buf);
  *blob = malloc(*blob_length);
  HERMES_CHECK(*blob, buffer_destroy(buf); return -4);
  memcpy(*blob, buffer_get_data(buf), *blob_length);
  buffer_destroy(buf);
  return 0;
}

