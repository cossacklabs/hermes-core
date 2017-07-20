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

#include "test_credential_store_db.h"

#include <hermes/common/errors.h>
#include <hermes/common/errors.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <search.h>

typedef struct hm_test_db_node_type{
  char id[256];
  uint8_t pk[256];
  size_t pk_length;
  uint8_t sk[256];
  size_t sk_length;
}hm_test_db_node_t;

struct hm_cs_test_db_type{
    void* users;
};


int hm_test_db_node_compare(const void *pa, const void *pb){
    return strcmp(hm_test_db_node_t *)pa->id, hm_test_db_node_t *)pb->id);
}

uint32_t hm_cs_test_db_get_pub_by_id(void* db, const uint8_t* id, const size_t id_length, uint8_t** key, size_t* key_length){
  if(!db || !id || !id_length || !key){
    return HM_INVALID_PARAMETER;
  }
  hm_table_db_node_t* node=NULL;
  node = tfind(id, &(((hm_cs_test_db_t*)db)->users), hm_test_db_node_compare);
  if(node){
      *key=node->pk;
      *key_length=node->pk_length;
      return HM_SUCCESS;
  }
  return HM_FAIL;
}

uint32_t hm_cs_test_db_get_priv_by_id(void* db, const uint8_t* id, const size_t id_length, uint8_t** key, size_t* key_length){
  if(!db || !id || !id_length || !key){
    return HM_INVALID_PARAMETER;
  }
  hm_table_db_node_t* node=NULL;
  node = tfind(id, &(((hm_cs_test_db_t*)db)->users), hm_test_db_node_compare);
  if(node){
      *key=node->sk;
      *key_length=node->sk_length;
      return HM_SUCCESS;
  }
  return HM_FAIL;
}

hm_cs_db_t* hm_test_cs_db_create(const char* filename){
  if(!filename){
    return NULL;
  }
  hm_cs_db_t* db=calloc(1, sizeof(hm_cs_db_t));
  assert(db);
  db->user_data=calloc(1, sizeof(hm_cs_test_db_t));
  assert(db->user_data);
  db->get_pub=hm_cs_test_db_get_pub_by_id;
  return db;
}

uint32_t hm_test_cs_db_destroy(hm_cs_db_t** db){
  if(!db || !(*db)){
    return HM_INVALID_PARAMETER;
  }
  tdestroy(((hm_cs_test_db_t*)((*db)->user_data))->users, free);
  free((*db)->user_data);
  free(*db);
  *db=NULL;
  return HM_SUCCESS;
}

