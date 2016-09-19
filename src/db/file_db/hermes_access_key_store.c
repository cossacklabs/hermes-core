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
#include <hermes/hermes_access_key_store.h>
#include <stdlib.h>
#include <stdio.h>

#define ACCESS_KEY_STORE_FOLDER "./db/access_rights/"

int hermes_access_key_store_write_token(const char* filename, const uint8_t* token, const size_t token_length){
  HERMES_CHECK(filename, return -1);
  FILE* token_file = fopen(filename, "wb");
  HERMES_CHECK(token_file, return -1);
  HERMES_CHECK(token_length == fwrite(token, 1, token_length, token_file), fclose(token_file); return -1);
  fclose(token_file);
  return 0;    
}

int hermes_access_key_store_read_token(const char* filename, uint8_t** token, size_t* token_length){
  HERMES_CHECK(filename, return -1);
  FILE* token_file = fopen(filename, "rb");
  HERMES_CHECK(token_file, return -1);

  //determine publik key file size
  fseek(token_file, 0L, SEEK_END);
  *token_length = ftell(token_file); //last data in block file is MAC, that must not return; 
  fseek(token_file, 0L, SEEK_SET);

  *token = malloc(*token_length);
  HERMES_CHECK(*token, fclose(token_file); return -1);
  HERMES_CHECK((*token_length) == fread(*token, 1, *token_length, token_file), free(*token); *token=NULL; fclose(token_file); return -1);
  fclose(token_file);
  return 0;
}

int hermes_access_key_store_remove_token(const char* filename){
  HERMES_CHECK(filename, return -1);
  remove(filename);
  return 0;
}

struct hermes_access_key_store_t_{ 
  int a; //unused
};

hermes_access_key_store_t* hermes_access_key_store_create(){
  hermes_access_key_store_t* store = calloc(sizeof(hermes_access_key_store_t),1);
  return store;
}

void hermes_access_key_store_destroy(hermes_access_key_store_t** store){
  free(*store);
}

int hermes_access_key_store_set_read_token(hermes_access_key_store_t* store, const char* doc_id, const char* block_id, const char* user_id, const uint8_t* token, const size_t token_length){
  HERMES_CHECK(store, return -1);
  char token_file_name[1024];
  sprintf(token_file_name, "%s%s_%s_%s_r", ACCESS_KEY_STORE_FOLDER, doc_id, block_id, user_id);
  return hermes_access_key_store_write_token(token_file_name, token, token_length);
}

int hermes_access_key_store_set_update_token(hermes_access_key_store_t* store, const char* doc_id, const char* block_id, const char* user_id, const uint8_t* token, const size_t token_length){
  HERMES_CHECK(store, return -1);
  char token_file_name[1024];
  sprintf(token_file_name, "%s%s_%s_%s_u", ACCESS_KEY_STORE_FOLDER, doc_id, block_id, user_id);
  return hermes_access_key_store_write_token(token_file_name, token, token_length);
}

int hermes_access_key_store_get_read_token(hermes_access_key_store_t* store, const char* doc_id, const char* block_id, const char* user_id, uint8_t** token, size_t* token_length){
  HERMES_CHECK(store, return -1);
  char token_file_name[1024];
  sprintf(token_file_name, "%s%s_%s_%s_r", ACCESS_KEY_STORE_FOLDER, doc_id, block_id, user_id);
  return hermes_access_key_store_read_token(token_file_name, token, token_length);
}

int hermes_access_key_store_get_update_token(hermes_access_key_store_t* store, const char* doc_id, const char* block_id, const char* user_id, uint8_t** token, size_t* token_length){
  HERMES_CHECK(store, return -1);
  char token_file_name[1024];
  sprintf(token_file_name, "%s%s_%s_%s_u", ACCESS_KEY_STORE_FOLDER, doc_id, block_id, user_id);
  return hermes_access_key_store_read_token(token_file_name, token, token_length);
}

int hermes_access_key_store_rem_read_token(hermes_access_key_store_t* store, const char* doc_id, const char* block_id, const char* user_id, const char* user_id_remoing_token_to){
  HERMES_CHECK(store, return -1);
  char token_file_name[1024];
  sprintf(token_file_name, "%s%s_%s_%s_r", ACCESS_KEY_STORE_FOLDER, doc_id, block_id, user_id_remoing_token_to);
  return hermes_access_key_store_remove_token(token_file_name);
}

int hermes_access_key_store_rem_update_token(hermes_access_key_store_t* store, const char* doc_id, const char* block_id, const char* user_id, const char* user_id_remoing_token_to){
  HERMES_CHECK(store, return -1);
  char token_file_name[1024];
  sprintf(token_file_name, "%s%s_%s_%s_u", ACCESS_KEY_STORE_FOLDER, doc_id, block_id, user_id_remoing_token_to);
  return hermes_access_key_store_remove_token(token_file_name);
}

