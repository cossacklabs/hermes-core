/*
 * Copyright (c) 2017 Cossack Labs Limited
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

#include "test_data_store_db.h"

#include <hermes/common/errors.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>

struct hm_ds_db_type{
  char db_dir[256];
};

#define ID_LENGTH 32

void gen_id(char **id) {
  char charset[] = "0123456789"
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  *id=malloc(ID_LENGTH);
  int length=ID_LENGTH;
  while (length-- > 0) {
    size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
    *(*id)++ = charset[index];
  }
  **id = '\0';
}

bool is_file_exists(const char* file_name){
  struct stat sb;
  if(stat(file_name, &sb) != 0){
    return true;
  }
  return false;
}

uint32_t write_file(const char* file_name, const uint8_t* data, const size_t data_length){
  FILE* file=fopen(file_name, "w+b");
  if(!file){
    return HM_FAIL;
  }
  if(data_length!=fwrite(data, data_length, 1, file)){
    fclose(file);
    return HM_FAIL;
  }
  fclose(file);
  return HM_SUCCESS;
}

uint32_t read_file(const char* file_name, uint8_t** data, size_t* data_length){
  FILE* file=fopen(file_name, "rb");
  if(!file){
    return HM_FAIL;
  }
  fseek(file, 0L, SEEK_END);
  *data_length = ftell(file);
  fseek(file, 0L, SEEK_SET);
  *data=malloc(*data_length);
  assert(*data);
  if((*data_length)!=fread(*data, *data_length, 1, file)){
    free(*data);
    fclose(file);
    return HM_FAIL;
  }
  fclose(file);
  return HM_SUCCESS;
}

hm_ds_db_t* hm_test_ds_db_create(const char* data_dir_name){
  if(!data_dir_name && strlen(data_dir_name)>=(256-1)){
    return NULL;
  }
  hm_ds_db_t* db=calloc(sizeof(hm_ds_db_t), 1);
  assert(db);
  struct stat sb;
  if (stat(data_dir_name, &sb) != 0 || !(S_ISDIR(sb.st_mode))){
    hm_test_ds_db_destroy(&db);
    return NULL;
  }
  memcpy(db->db_dir, data_dir_name, strlen(data_dir_name));
  return db;
}

uint32_t hm_test_ds_db_destroy(hm_ds_db_t** db){
  if(!db || !(*db)){
    return HM_INVALID_PARAMETER;
  }
  free(*db);
  *db=NULL;
  return HM_SUCCESS;
}

uint32_t hm_ds_db_insert_block(hm_ds_db_t* db, const uint8_t* block, const size_t block_length, const uint8_t* mac, const size_t mac_length, uint8_t** id, size_t* id_length){
  if(!db || !block || !block_length || !mac || !mac_length || !id){
    return HM_INVALID_PARAMETER;
  }
  *id=NULL;
  char block_file_name[256];
  char mac_file_name[256];
  do{
    free(*id);
    gen_id((char**)(id));
    sprintf(block_file_name, "%s/%s_blk", db->db_dir, *id);
    sprintf(mac_file_name, "%s/%s_mac", db->db_dir, *id);
  }while(is_file_exists(block_file_name)); 
  *id_length=ID_LENGTH;
  if(HM_SUCCESS!=write_file(block_file_name, block, block_length)){
    free(*id);
    return HM_FAIL;
  }
  if(HM_SUCCESS!=write_file(mac_file_name, mac, mac_length)){
    free(*id);
    return HM_FAIL;
  }  
  return HM_SUCCESS;
}

uint32_t hm_ds_db_read_block(hm_ds_db_t* db, const uint8_t* id, const size_t id_length, uint8_t** block, size_t*  block_length){
  if(!db || !id || !id_length || !block){
    return HM_INVALID_PARAMETER;
  }
  char file_name[256];
  sprintf(file_name, "%s/%s_blk", db->db_dir, id);
  if(!is_file_exists(file_name) || HM_SUCCESS!=read_file(file_name, block, block_length)){
    return HM_FAIL;
  } 
  return HM_SUCCESS;
}

uint32_t hm_ds_db_read_block_mac(hm_ds_db_t* db, const uint8_t* id, const size_t id_length, uint8_t** mac, size_t*  mac_length){
  if(!db || !id || !id_length || !mac){
    return HM_INVALID_PARAMETER;
  }
  char file_name[256];
  sprintf(file_name, "%s/%s_mac", db->db_dir, id);
  if(!is_file_exists(file_name) || HM_SUCCESS!=read_file(file_name, mac, mac_length)){
    return HM_FAIL;
  } 
  return HM_SUCCESS;
}

uint32_t hm_ds_db_update_block(hm_ds_db_t* db, const uint8_t* id, const size_t id_length, const uint8_t* block, const size_t block_length, const uint8_t* mac, const size_t mac_length){
  if(!id || !id_length || !block || !block_length || !mac || !mac_length){
    return HM_INVALID_PARAMETER;
  }
  char block_file_name[256];
  char mac_file_name[256];
  sprintf(block_file_name, "%s/%s_blk", db->db_dir, id);
  sprintf(mac_file_name, "%s/%s_mac", db->db_dir, id);
  if(!is_file_exists(block_file_name) || !is_file_exists(mac_file_name)){
    return HM_FAIL;
  }
  if(HM_SUCCESS!=write_file(block_file_name, block, block_length)){
    return HM_FAIL;
  }
  if(HM_SUCCESS!=write_file(mac_file_name, mac, mac_length)){
    return HM_FAIL;
  }    
  return HM_SUCCESS;
}

uint32_t hm_ds_db_delete_block(hm_ds_db_t* db, const uint8_t* id, const size_t id_length){
  if(!db || !id || !id_length){
    return HM_INVALID_PARAMETER;
  }
  char block_file_name[256];
  char mac_file_name[256];
  sprintf(block_file_name, "%s/%s_blk", db->db_dir, id);
  sprintf(mac_file_name, "%s/%s_mac", db->db_dir, id);
  if(!is_file_exists(block_file_name) || !is_file_exists(mac_file_name)){
    return HM_FAIL;
  }
  if(0!=remove(block_file_name) || 0!=remove(mac_file_name)){
    return HM_FAIL;
  }
  return HM_SUCCESS;
}

