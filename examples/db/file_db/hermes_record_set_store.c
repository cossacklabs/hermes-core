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
#include <hermes/hermes_record_set_store.h>
#include <themis/themis.h>
#include <stdlib.h>
#include <stdio.h>

#define RECORD_SET_STORE_FOLDER "./db/blocks/"

int check_file_mac(hermes_record_set_store_t* store, const char* doc_id, const char* block_id, const uint8_t* mac, size_t mac_length){
  HERMES_CHECK(store, return -1);
  char block_file_name[1024];
  sprintf(block_file_name, "%s%s_%s", RECORD_SET_STORE_FOLDER, doc_id, block_id);
  FILE* block_file = fopen(block_file_name, "rb");
  HERMES_CHECK(block_file, return -1);
  fseek(block_file, 0-mac_length, SEEK_END);
  uint8_t mac_[1024];
  HERMES_CHECK(mac_length == fread(mac_, 1, mac_length, block_file), fclose(block_file); return -1);
  fclose(block_file);
  if(0 == memcmp(mac_, mac, mac_length)){
    return 0;
  }
  return -1;
}

int store_block(hermes_record_set_store_t* store, const char* doc_id, const char* block_id, const uint8_t* blob, const size_t blob_length){
  HERMES_CHECK(store, return -1);
  char block_file_name[1024];
  sprintf(block_file_name, "%s%s_%s", RECORD_SET_STORE_FOLDER, doc_id, block_id);
  FILE* block_file = fopen(block_file_name, "wb");
  HERMES_CHECK(block_file, return -1);
  HERMES_CHECK(blob_length == fwrite(blob, 1, blob_length, block_file), fclose(block_file); return -1);
  fclose(block_file);
  return 0;  
}

int delete_block(hermes_record_set_store_t* store, const char* doc_id, const char* block_id){
  HERMES_CHECK(store, return -1);
  char block_file_name[1024];
  sprintf(block_file_name, "%s%s_%s", RECORD_SET_STORE_FOLDER, doc_id, block_id);
  remove(block_file_name);
  return 0;
}

struct hermes_record_set_store_t_{ 
  int a; //unused
};

hermes_record_set_store_t* hermes_record_set_store_create(){
  hermes_record_set_store_t* store = calloc(sizeof(hermes_record_set_store_t),1);
  return store;
}

void hermes_record_set_store_destroy(hermes_record_set_store_t** store){
  free(*store);
}

int hermes_record_set_set_block(hermes_record_set_store_t* store, const char* doc_id, const char* block_id, const uint8_t* mac, const size_t mac_length, const uint8_t* blob, const size_t blob_length){
  HERMES_CHECK(store, return -1);
  HERMES_CHECK(0==check_file_mac(store, doc_id, block_id, mac, mac_length), return -1);
  HERMES_CHECK(0==store_block(store, doc_id, block_id, blob, blob_length), return -1);  
  return 0;
}

int hermes_record_set_set_new_block(hermes_record_set_store_t* store, const char* doc_id, const uint8_t* blob, const size_t blob_length, char** new_block_id){
  HERMES_CHECK(store, return -1);
  uint32_t new_block_id_=0;
  HERMES_CHECK(THEMIS_SUCCESS == soter_rand((uint8_t*)(&new_block_id_), 4), return -1);
  *new_block_id = malloc(20);
  HERMES_CHECK(*new_block_id, return -1);
  sprintf(*new_block_id, "%u", new_block_id_);
  HERMES_CHECK(0==store_block(store, doc_id?doc_id:(*new_block_id), *new_block_id, blob, blob_length), free(*new_block_id); *new_block_id = NULL; return -1);
  return 0;
}

int hermes_record_set_get_block(hermes_record_set_store_t* store, const char* doc_id, const char* block_id, uint8_t** blob, size_t* blob_length){
  HERMES_CHECK(store, return -1);
  char block_file_name[1024];
  sprintf(block_file_name, "%s%s_%s", RECORD_SET_STORE_FOLDER, doc_id, block_id);
  FILE* block_file = fopen(block_file_name, "rb");
  HERMES_CHECK(block_file, return -1);

  //determine public key file size
  fseek(block_file, 0L, SEEK_END);
  *blob_length = ftell(block_file)-16; //last data in block file is MAC, that must not return; 
  fseek(block_file, 0L, SEEK_SET);

  *blob = malloc(*blob_length);
  HERMES_CHECK(*blob, fclose(block_file); return -1);
  HERMES_CHECK((*blob_length) == fread(*blob, 1, *blob_length, block_file), free(*blob); *blob=NULL; fclose(block_file); return -1);
  fclose(block_file);
  return 0;
}

int hermes_record_set_delete_block(hermes_record_set_store_t* store, const char* doc_id, const char* block_id, const uint8_t* mac, const size_t mac_length){
  HERMES_CHECK(store, return -1);
  HERMES_CHECK(0==check_file_mac(store, doc_id, block_id, mac, mac_length), return -1);
  delete_block(store, doc_id, block_id);
  return 0;
}


