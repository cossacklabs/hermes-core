/*
* Copyright (c) 2017 Cossack Labs Limited
*
* This file is a part of Hermes-core.
*
* Hermes-core is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Hermes-core is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with Hermes-core.  If not, see <http://www.gnu.org/licenses/>.
*
*/


#include "common.h"
#include "test_data_store_db.h"

#include <hermes/common/errors.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#define __USE_GNU
#include <search.h>
#include <themis/themis.h>

typedef struct hm_test_ds_db_node_type{
  uint8_t id[BLOCK_ID_LENGTH];
  uint8_t block[MAX_BLOCK_LENGTH];
  size_t block_length;
  uint8_t meta[MAX_META_LENGTH];
  size_t meta_length;
  uint8_t mac[MAX_MAC_LENGTH];
  size_t mac_length;
}hm_test_ds_db_node_t;

typedef struct hm_test_ds_db_type{
  void* blocks;
}hm_test_ds_db_t;

int hm_ds_test_db_node_compare(const void* a, const void* b){
  if(!a){
    if(!b){
      return 0;
    }
    return 1;
  }
  if(!b){
    return -1;
  }
  return memcmp(((hm_test_ds_db_node_t*)a)->id, ((hm_test_ds_db_node_t*)b)->id, BLOCK_ID_LENGTH);
}


uint32_t hm_test_ds_db_insert_block(
                                    void* db,
                                    const uint8_t* block, const size_t block_length,
                                    const uint8_t* meta, const size_t meta_length,
                                    const uint8_t* mac, const size_t mac_length,
                                    uint8_t** id, size_t* id_length){
  if(!db || !block || !block_length || block_length>MAX_BLOCK_LENGTH || !meta || !meta_length || meta_length>MAX_META_LENGTH || !mac || !mac_length || mac_length>MAX_MAC_LENGTH || !id || !id_length){
    return HM_INVALID_PARAMETER;
  }
  hm_test_ds_db_node_t* node=calloc(1, sizeof(hm_test_ds_db_node_t));
  assert(node);
  hm_test_ds_db_node_t** searched_node=NULL;
  while(!searched_node || (*searched_node)!=node){  
    if(SOTER_SUCCESS!=soter_rand(node->id, BLOCK_ID_LENGTH)){
      free(node);
      return HM_FAIL;
    }
    searched_node = (hm_test_ds_db_node_t**)tsearch(node, &(((hm_test_ds_db_t*)db)->blocks), hm_ds_test_db_node_compare);
    if(!searched_node){
      free(node);
      return HM_FAIL;
    }
  }
  memcpy((*searched_node)->block, block, block_length);
  (*searched_node)->block_length=block_length;
  memcpy((*searched_node)->meta, meta, meta_length);
  (*searched_node)->meta_length=meta_length;
  memcpy((*searched_node)->mac, mac, mac_length);
  (*searched_node)->mac_length=mac_length;
  *id=malloc(BLOCK_ID_LENGTH);
  assert(*id);
  memcpy(*id, (*searched_node)->id, BLOCK_ID_LENGTH);
  *id_length=BLOCK_ID_LENGTH;
  return HM_SUCCESS;
}

uint32_t hm_test_ds_db_insert_block_with_id(
                                            void* db,
                                            const uint8_t* id, const size_t id_length,
                                            const uint8_t* block, const size_t block_length,
                                            const uint8_t* meta, const size_t meta_length,
                                            const uint8_t* mac, const size_t mac_length){
  if(!db || !block || !block_length || block_length>MAX_BLOCK_LENGTH || !meta || !meta_length || meta_length>MAX_META_LENGTH || !mac || !mac_length || mac_length>MAX_MAC_LENGTH || !id || id_length!=BLOCK_ID_LENGTH){
    return HM_INVALID_PARAMETER;
  }
  hm_test_ds_db_node_t* node=calloc(1, sizeof(hm_test_ds_db_node_t));
  assert(node);
  memcpy(node->id, id, id_length);
  memcpy(node->block, block, block_length);
  node->block_length=block_length;
  memcpy(node->meta, meta, meta_length);
  node->meta_length=meta_length;
  memcpy(node->mac, mac, mac_length);
  node->mac_length=mac_length;
  hm_test_ds_db_node_t** searched_node=(hm_test_ds_db_node_t**)tsearch(node, &(((hm_test_ds_db_t*)db)->blocks), hm_ds_test_db_node_compare);
  if(!searched_node || (*searched_node)!=node){  
      free(node);
      return HM_FAIL;
  }
  return HM_SUCCESS;
}


uint32_t hm_test_ds_db_read_block(void* db, const uint8_t* id, const size_t id_length, uint8_t** block, size_t*  block_length, uint8_t** meta, size_t* meta_length){
  if(!db || !id || id_length!=BLOCK_ID_LENGTH ||  !block || !block_length || !meta || !meta_length){
    return HM_INVALID_PARAMETER;
  }
  hm_test_ds_db_node_t* node=calloc(1, sizeof(hm_test_ds_db_node_t));
  assert(node);
  memcpy(node->id, id, id_length);
  hm_test_ds_db_node_t** searched_node=(hm_test_ds_db_node_t**)tfind(node, &(((hm_test_ds_db_t*)db)->blocks), hm_ds_test_db_node_compare);
  free(node);
  if(!searched_node){
    return HM_FAIL;
  }
  *block=malloc((*searched_node)->block_length);
  assert(*block);
  *meta=malloc((*searched_node)->meta_length);
  assert(*meta);
  memcpy(*block, (*searched_node)->block, (*searched_node)->block_length);
  *block_length=(*searched_node)->block_length;
  memcpy(*meta, (*searched_node)->meta, (*searched_node)->meta_length);
  *meta_length=(*searched_node)->meta_length;
  return HM_SUCCESS;
}

uint32_t hm_test_ds_db_update_block(
   void* db, 
   const uint8_t* id, const size_t id_length, 
   const uint8_t* block, const size_t block_length, 
   const uint8_t* meta, const size_t meta_length, 
   const uint8_t* mac, const size_t mac_length, 
   const uint8_t* old_mac, const size_t old_mac_length){
  if(!id || id_length!=BLOCK_ID_LENGTH || !block || !block_length || block_length>MAX_BLOCK_LENGTH || !meta || !meta_length || meta_length>MAX_META_LENGTH || !mac || !mac_length || mac_length>MAX_MAC_LENGTH || !old_mac || !old_mac_length || old_mac_length>MAX_MAC_LENGTH){
    return HM_INVALID_PARAMETER;
  }
  hm_test_ds_db_node_t* node=calloc(1, sizeof(hm_test_ds_db_node_t));
  assert(node);
  memcpy(node->id, id, id_length);
  hm_test_ds_db_node_t** searched_node=(hm_test_ds_db_node_t**)tfind(node, &(((hm_test_ds_db_t*)db)->blocks), hm_ds_test_db_node_compare);
  free(node);
  if(!searched_node || (*searched_node)->mac_length != old_mac_length || 0!=memcmp((*searched_node)->mac, old_mac, (*searched_node)->mac_length)){
    return HM_FAIL;
  }
  memcpy((*searched_node)->block, block, block_length);
  (*searched_node)->block_length=block_length;
  memcpy((*searched_node)->meta, meta, meta_length);
  (*searched_node)->meta_length=meta_length;
  memcpy((*searched_node)->mac, mac, mac_length);
  (*searched_node)->mac_length=mac_length;
  return HM_SUCCESS;
}

uint32_t hm_test_ds_db_delete_block(void* db, const uint8_t* id, const size_t id_length, const uint8_t* old_mac, const size_t old_mac_length){
  if(!id || id_length!=BLOCK_ID_LENGTH || !old_mac || !old_mac_length || old_mac_length>MAX_MAC_LENGTH){
    return HM_INVALID_PARAMETER;
  }
  hm_test_ds_db_node_t* node=calloc(1, sizeof(hm_test_ds_db_node_t));
  assert(node);
  memcpy(node->id, id, id_length);
  hm_test_ds_db_node_t** searched_node=(hm_test_ds_db_node_t**)tfind(node, &(((hm_test_ds_db_t*)db)->blocks), hm_ds_test_db_node_compare);
  free(node);
  if(!searched_node || (*searched_node)->mac_length != old_mac_length || 0!=memcmp((*searched_node)->mac, old_mac, (*searched_node)->mac_length)){
    return HM_FAIL;
  }
  tdelete(*searched_node, &(((hm_test_ds_db_t*)db)->blocks), hm_ds_test_db_node_compare);
  free(*searched_node);
  return HM_SUCCESS;
}

hm_ds_db_t* hm_test_ds_db_create(){
  hm_ds_db_t* db=calloc(1, sizeof(hm_ds_db_t));
  assert(db);
  db->user_data=calloc(1, sizeof(hm_test_ds_db_t));
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
  tdestroy(((hm_test_ds_db_t*)((*db)->user_data))->blocks, free);
  free((*db)->user_data);
  free(*db);
  *db=NULL;
  return HM_SUCCESS;
}
