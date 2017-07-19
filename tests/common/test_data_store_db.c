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

#define MAX_ID_LENGTH 32
#define MAX_BLOCK_LENGTH 256
#define MAX_META_LENGTH 256
#define MAX_MAC_LENGTH 64
#define MAX_BLOCK_COUNT 256

typedef struct hm_test_ds_db_block_type{
  uint8_t mac[MAX_MAC_LENGTH];
  uint8_t data[MAX_BLOCK_LENGTH];
  uint8_t meta[MAX_META_LENGTH];
}hm_test_ds_db_block_t;

typedef struct hm_test_ds_db_type{
  hm_test_ds_db_block_t blocks[MAX_BLOCK_COUNT];
  size_t used_blocks;
}hm_test_ds_db_t;

uint32_t hm_test_ds_db_insert_block(void* db, const uint8_t* block, const size_t block_length, const uint8_t* meta, const size_t meta_length,const uint8_t* mac, const size_t mac_length, uint8_t** id, size_t* id_length){
  if(!db || !block || !block_length || block_length>MAX_BLOCK_LENGTH || !meta || !meta_length || meta_length>MAX_META_LENGTH || !mac || !mac_length || mac_length>MAX_MAC_LENGTH || !id){
    return HM_INVALID_PARAMETER;
  }
  if(((hm_test_ds_db_t*)db)->used_blocks == MAX_BLOCK_COUNT){
    return HM_FAIL;
  }
  *id=malloc(sizeof(uint32_t));
  assert(*id);
  memcpy(*id, &(((hm_test_ds_db_t*)db)->used_blocks), sizeof(uint32_t));
  *id_length=sizeof(uint32_t);
  memcpy(((hm_test_ds_db_t*)db)->blocks[((hm_test_ds_db_t*)db)->used_blocks].data, block, block_length);
  memcpy(((hm_test_ds_db_t*)db)->blocks[((hm_test_ds_db_t*)db)->used_blocks].meta, meta, meta_length);
  memcpy(((hm_test_ds_db_t*)db)->blocks[((hm_test_ds_db_t*)db)->used_blocks].mac, mac, mac_length);
  ++(((hm_test_ds_db_t*)db)->used_blocks);
  return HM_SUCCESS;
}

uint32_t hm_test_ds_db_insert_block_with_id(void* db, const uint8_t* id, const size_t id_length, const uint8_t* block, const size_t block_length, const uint8_t* meta, const size_t meta_length, const uint8_t* mac, const size_t mac_length){
  return HM_FAIL;
}


uint32_t hm_test_ds_db_read_block(void* db, const uint8_t* id, const size_t id_length, uint8_t** block, size_t*  block_length, uint8_t** meta, size_t* meta_length){
  if(!db || !id || !id_length || id_length!=sizeof(uint32_t) || !block || !meta){
    return HM_INVALID_PARAMETER;
  }
  if(*((uint32_t*)id)>=(((hm_test_ds_db_t*)db)->used_blocks)){
    return HM_FAIL;
  }
  *block=malloc(MAX_BLOCK_LENGTH);
  assert(*block);
  *meta=malloc(MAX_META_LENGTH);
  assert(*meta);
  memcpy(*block, ((hm_test_ds_db_t*)db)->blocks[*((uint32_t*)id)].data, MAX_BLOCK_LENGTH);
  *block_length=MAX_BLOCK_LENGTH;
  memcpy(*meta, ((hm_test_ds_db_t*)db)->blocks[*((uint32_t*)id)].meta, MAX_META_LENGTH);
  *meta_length=MAX_META_LENGTH;
  return HM_SUCCESS;
}

uint32_t hm_test_ds_db_update_block(
   void* db, 
   const uint8_t* id, const size_t id_length, 
   const uint8_t* block, const size_t block_length, 
   const uint8_t* meta, const size_t meta_length, 
   const uint8_t* mac, const size_t mac_length, 
   const uint8_t* old_mac, const size_t old_mac_length){
  if(!id || !id_length || !block || !block_length || block_length>MAX_BLOCK_LENGTH || !meta || !meta_length || meta_length>MAX_META_LENGTH || !mac || !mac_length || mac_length>MAX_MAC_LENGTH || !old_mac || !old_mac_length || old_mac_length>MAX_MAC_LENGTH){
    return HM_INVALID_PARAMETER;
  }
  if(*((uint32_t*)id)>=(((hm_test_ds_db_t*)db)->used_blocks) || 0!=memcmp(((hm_test_ds_db_t*)db)->blocks[*((uint32_t*)id)].mac, old_mac, old_mac_length)){
    return HM_FAIL;
  }
  memcpy(((hm_test_ds_db_t*)db)->blocks[*((uint32_t*)id)].data, block, block_length);
  memcpy(((hm_test_ds_db_t*)db)->blocks[*((uint32_t*)id)].meta, meta, meta_length);
  memcpy(((hm_test_ds_db_t*)db)->blocks[*((uint32_t*)id)].mac, mac, mac_length);
  return HM_SUCCESS;
}

uint32_t hm_test_ds_db_delete_block(void* db, const uint8_t* id, const size_t id_length, const uint8_t* okd_mac, const size_t old_mac_length){
  return HM_FAIL;
}

hm_ds_db_t* hm_test_ds_db_create(const char* data_dir_name){
  if(!data_dir_name && strlen(data_dir_name)>=(256-1)){
    return NULL;
  }
  hm_ds_db_t* db=calloc(sizeof(hm_ds_db_t), 1);
  assert(db);
  db->user_data=calloc(sizeof(hm_test_ds_db_t), 1);
  assert(db->user_data);
  db->insert_block=hm_test_ds_db_insert_block;
  db->insert_block_with_id=hm_test_ds_db_insert_block_with_id;
  db->get_block=hm_test_ds_db_read_block;
  db->update_block=hm_test_ds_db_update_block;
  db->rem_block=hm_test_ds_db_delete_block;
  return db;
}

uint32_t hm_test_ds_db_destroy(hm_ds_db_t** db){
  if(!db || !(*db)){
    return HM_INVALID_PARAMETER;
  }
  free((*db)->user_data);
  free(*db);
  *db=NULL;
  return HM_SUCCESS;
}
