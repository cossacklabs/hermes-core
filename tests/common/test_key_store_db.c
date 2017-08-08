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


#include "test_key_store_db.h"
#include "common.h"

#include <hermes/common/errors.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>
#define __USE_GNU
#include <search.h>

typedef struct hm_test_ks_db_token_type{
  uint8_t user_id[USER_ID_LENGTH];
  uint8_t rtoken[MAX_TOKEN_LENGTH];
  size_t rtoken_length;
  uint8_t rtoken_owner[USER_ID_LENGTH];
  uint8_t wtoken[MAX_TOKEN_LENGTH];
  size_t wtoken_length;
  uint8_t wtoken_owner[USER_ID_LENGTH];
}hm_test_ks_db_token_t;

typedef struct hm_test_ks_db_node_type{
  uint8_t block_id[BLOCK_ID_LENGTH];
  void* users;
}hm_test_ks_db_node_t;

typedef struct hm_test_ks_db_type{
  void* blocks;
} hm_test_ks_db_t;

int hm_ks_test_db_node_compare(const void* a, const void* b){
  if(!a){
    if(!b){
      return 0;
    }
    return 1;
  }
  if(!b){
    return -1;
  }
  return memcmp(((hm_test_ks_db_node_t*)a)->block_id, ((hm_test_ks_db_node_t*)b)->block_id, BLOCK_ID_LENGTH);
}

void hm_ks_test_db_node_destroy(void* a){
  tdestroy(((hm_test_ks_db_node_t*)a)->users, free);
  free(a);
}

int hm_ks_test_db_token_compare(const void* a, const void* b){
  if(!a){
    if(!b){
      return 0;
    }
    return 1;
  }
  if(!b){
    return -1;
  }
  return memcmp(((hm_test_ks_db_token_t*)a)->user_id, ((hm_test_ks_db_token_t*)b)->user_id, USER_ID_LENGTH);
}

uint32_t db_del_rtoken(
                       void* db,
                       const uint8_t* block_id, const size_t block_id_length,
                       const uint8_t* user_id, const size_t user_id_length,
                       const uint8_t* owner_id, const size_t owner_id_length){
  if(!db || !block_id || BLOCK_ID_LENGTH != block_id_length || !user_id || USER_ID_LENGTH != user_id_length || !owner_id || USER_ID_LENGTH != owner_id_length){
    return HM_INVALID_PARAMETER;
  }
  hm_test_ks_db_node_t* block_node=calloc(1, sizeof(hm_test_ks_db_node_t));
  assert(block_node);
  memcpy(block_node->block_id, block_id, block_id_length);
  hm_test_ks_db_node_t** serached_block_node = (hm_test_ks_db_node_t**)tfind(block_node, &(((hm_test_ks_db_t*)db)->blocks), hm_ks_test_db_node_compare);
  if(!serached_block_node){
    free(block_node);
    return HM_FAIL;
  }
  free(block_node);
  hm_test_ks_db_token_t* token=calloc(1, sizeof(hm_test_ks_db_token_t));
  assert(token);
  memcpy(token->user_id, user_id, USER_ID_LENGTH);
  hm_test_ks_db_token_t** searched_token= (hm_test_ks_db_token_t**)(tfind)(token, &((*serached_block_node)->users), hm_ks_test_db_token_compare);
  if(!searched_token){
    free(token);
    return HM_FAIL;
  }
  free(token);
  tdelete((*searched_token), &((*serached_block_node)->users), hm_ks_test_db_token_compare);
  free(*searched_token);
  return HM_SUCCESS;
}

uint32_t db_set_rtoken(
                       void* ks,
                       const uint8_t* block_id, const size_t block_id_length,
                       const uint8_t* user_id, const size_t user_id_length,
                       const uint8_t* owner_id, const size_t owner_id_length,
                       const uint8_t* token, const size_t token_length){
  if(!ks || !block_id || BLOCK_ID_LENGTH != block_id_length || !user_id || USER_ID_LENGTH != user_id_length || !owner_id || USER_ID_LENGTH != owner_id_length){
    return HM_INVALID_PARAMETER;
  }
  if(token && token_length){
    hm_test_ks_db_node_t* block_node=calloc(1, sizeof(hm_test_ks_db_node_t));
    assert(block_node);
    memcpy(block_node->block_id, block_id, block_id_length);
    hm_test_ks_db_node_t** serached_block_node = (hm_test_ks_db_node_t**)tsearch(block_node, &(((hm_test_ks_db_t*)ks)->blocks), hm_ks_test_db_node_compare);
    assert(serached_block_node);
    if((*serached_block_node) != block_node){
      free(block_node);
    }
    hm_test_ks_db_token_t* node_token=calloc(1, sizeof(hm_test_ks_db_token_t));
    assert(node_token);
    memcpy(node_token->user_id, user_id, USER_ID_LENGTH);
    hm_test_ks_db_token_t** searched_token = (hm_test_ks_db_token_t**)(tsearch)(node_token, &((*serached_block_node)->users), hm_ks_test_db_token_compare);
    assert(searched_token);
    if((*searched_token) != node_token){
      free(node_token);
    }
    memcpy((*searched_token)->rtoken, token, token_length);
    (*searched_token)->rtoken_length=token_length;
    memcpy((*searched_token)->rtoken_owner, owner_id, USER_ID_LENGTH);
    return HM_SUCCESS;
  }
  return db_del_rtoken(ks, block_id, block_id_length, user_id, user_id_length, owner_id, owner_id_length);
}

uint32_t db_set_wtoken(
                       void* ks,
                       const uint8_t* block_id, const size_t block_id_length,
                       const uint8_t* user_id, const size_t user_id_length,
                       const uint8_t* owner_id, const size_t owner_id_length,
                       const uint8_t* token, const size_t token_length){
  if(!ks || !block_id || BLOCK_ID_LENGTH != block_id_length || !user_id || USER_ID_LENGTH != user_id_length || !owner_id || USER_ID_LENGTH != owner_id_length){
    return HM_INVALID_PARAMETER;
  }
  hm_test_ks_db_node_t* block_node=calloc(1, sizeof(hm_test_ks_db_node_t));
  assert(block_node);
  memcpy(block_node->block_id, block_id, block_id_length);
  hm_test_ks_db_node_t** serached_block_node = (hm_test_ks_db_node_t**)tfind(block_node, &(((hm_test_ks_db_t*)ks)->blocks), hm_ks_test_db_node_compare);
  if(!serached_block_node){
    free(block_node);
    return HM_FAIL;
  }
  free(block_node);
  hm_test_ks_db_token_t* node_token=calloc(1, sizeof(hm_test_ks_db_token_t));
  assert(node_token);
  memcpy(node_token->user_id, user_id, USER_ID_LENGTH);
  hm_test_ks_db_token_t** searched_token= (hm_test_ks_db_token_t**)(tfind)(node_token, &((*serached_block_node)->users), hm_ks_test_db_token_compare);
  if(!searched_token){
    free(node_token);
    return HM_FAIL;
  }
  free(node_token);
  memcpy((*searched_token)->wtoken, token, token_length);
  (*searched_token)->wtoken_length=token_length;
  memcpy((*searched_token)->wtoken_owner, owner_id, USER_ID_LENGTH);
  return HM_SUCCESS;
}

uint32_t db_get_rtoken(
                       void* ks,
                       const uint8_t* block_id, const size_t block_id_length,
                       const uint8_t* user_id, const size_t user_id_length,
                       uint8_t** token, size_t* token_length,
                       uint8_t** owner_id, size_t* owner_id_length){
  if(!ks || !block_id || BLOCK_ID_LENGTH != block_id_length || !user_id || USER_ID_LENGTH != user_id_length || !owner_id || !owner_id_length || !token || !token_length){
    return HM_INVALID_PARAMETER;
  }
  hm_test_ks_db_node_t* block_node=calloc(1, sizeof(hm_test_ks_db_node_t));
  assert(block_node);
  memcpy(block_node->block_id, block_id, block_id_length);
  hm_test_ks_db_node_t** serached_block_node = (hm_test_ks_db_node_t**)tfind(block_node, &(((hm_test_ks_db_t*)ks)->blocks), hm_ks_test_db_node_compare);
  if(!serached_block_node){
    free(block_node);
    return HM_FAIL;
  }
  free(block_node);
  hm_test_ks_db_token_t* node_token=calloc(1, sizeof(hm_test_ks_db_token_t));
  assert(node_token);
  memcpy(node_token->user_id, user_id, USER_ID_LENGTH);
  hm_test_ks_db_token_t** searched_token= (hm_test_ks_db_token_t**)(tfind)(node_token, &((*serached_block_node)->users), hm_ks_test_db_token_compare);
  if(!searched_token){
    free(node_token);
    return HM_FAIL;
  }
  free(node_token);
  *token=malloc((*searched_token)->rtoken_length);
  assert(*token);
  *token_length=(*searched_token)->rtoken_length;
  memcpy(*token, (*searched_token)->rtoken, (*searched_token)->rtoken_length);
  *owner_id=malloc(USER_ID_LENGTH);
  *owner_id_length=USER_ID_LENGTH;
  assert(*owner_id);
  memcpy(*owner_id, (*searched_token)->rtoken_owner, USER_ID_LENGTH);
  //  fprintf(stderr, "%i %i\n", *token_length, );
  return HM_SUCCESS;
}

uint32_t db_get_wtoken(
                       void* ks,
                       const uint8_t* block_id, const size_t block_id_length,
                       const uint8_t* user_id, const size_t user_id_length,
                       uint8_t** token, size_t* token_length,
                       uint8_t** owner_id, size_t* owner_id_length){
  if(!ks || !block_id || BLOCK_ID_LENGTH != block_id_length || !user_id || USER_ID_LENGTH != user_id_length || !owner_id || !owner_id_length || !token || !token_length){
    return HM_INVALID_PARAMETER;
  }
  hm_test_ks_db_node_t* block_node=calloc(1, sizeof(hm_test_ks_db_node_t));
  assert(block_node);
  memcpy(block_node->block_id, block_id, block_id_length);
  hm_test_ks_db_node_t** serached_block_node = (hm_test_ks_db_node_t**)tfind(block_node, &(((hm_test_ks_db_t*)ks)->blocks), hm_ks_test_db_node_compare);
  if(!serached_block_node){
    free(block_node);
    return HM_FAIL;
  }
  free(block_node);
  hm_test_ks_db_token_t* node_token=calloc(1, sizeof(hm_test_ks_db_token_t));
  assert(node_token);
  memcpy(node_token->user_id, user_id, USER_ID_LENGTH);
  hm_test_ks_db_token_t** searched_token= (hm_test_ks_db_token_t**)(tfind)(node_token, &((*serached_block_node)->users), hm_ks_test_db_token_compare);
  if(!searched_token){
    free(node_token);
    return HM_FAIL;
  }
  free(node_token);
  if(!((*searched_token)->wtoken_length)){
    return HM_FAIL;
  }
  *token=malloc((*searched_token)->wtoken_length);
  assert(*token);
  *token_length=(*searched_token)->wtoken_length;
  memcpy(*token, (*searched_token)->wtoken, (*searched_token)->wtoken_length);
  *owner_id=malloc(USER_ID_LENGTH);
  assert(*owner_id);
  *owner_id_length=USER_ID_LENGTH;
  memcpy(*owner_id, (*searched_token)->wtoken_owner, USER_ID_LENGTH);
  return HM_SUCCESS;
}

uint32_t  db_get_indexed_rights(
                                void* db,
                                const uint8_t* block_id, const size_t block_id_length,
                                const size_t index,
                                uint8_t** user_id, size_t* user_id_length,
                                uint32_t* rights_mask){
  if(!db || !block_id || !block_id_length || !user_id || !user_id_length || !rights_mask){
    return HM_INVALID_PARAMETER;
  }
  return HM_FAIL;
}

uint32_t db_del_wtoken(
                       void* db,
                       const uint8_t* block_id, const size_t block_id_length,
                       const uint8_t* user_id, const size_t user_id_length,
                       const uint8_t* owner_id, const size_t owner_id_length){
  if(!db || !block_id || BLOCK_ID_LENGTH != block_id_length || !user_id || USER_ID_LENGTH != user_id_length || !owner_id || USER_ID_LENGTH != owner_id_length){
    return HM_INVALID_PARAMETER;
  }
  hm_test_ks_db_node_t* block_node=calloc(1, sizeof(hm_test_ks_db_node_t));
  assert(block_node);
  memcpy(block_node->block_id, block_id, block_id_length);
  hm_test_ks_db_node_t** serached_block_node = (hm_test_ks_db_node_t**)tfind(block_node, &(((hm_test_ks_db_t*)db)->blocks), hm_ks_test_db_node_compare);
  if(!serached_block_node){
    free(block_node);
    return HM_FAIL;
  }
  free(block_node);
  hm_test_ks_db_token_t* token=calloc(1, sizeof(hm_test_ks_db_token_t));
  assert(token);
  memcpy(token->user_id, user_id, USER_ID_LENGTH);
  hm_test_ks_db_token_t** searched_token= (hm_test_ks_db_token_t**)(tfind)(token, &((*serached_block_node)->users), hm_ks_test_db_token_compare);
  if(!searched_token){
    free(token);
    return HM_FAIL;
  }
  free(token);
  (*searched_token)->wtoken_length=0;
  return HM_SUCCESS;
}

hm_ks_db_t* hm_test_ks_db_create(){
  hm_ks_db_t* ks=calloc(1, sizeof(hm_ks_db_t));
  assert(ks);
  ks->user_data=calloc(1, sizeof(hm_test_ks_db_t));
  assert(ks->user_data);
  ks->set_rtoken=db_set_rtoken;
  ks->set_wtoken=db_set_wtoken;
  ks->get_rtoken=db_get_rtoken;
  ks->get_wtoken=db_get_wtoken;
  ks->get_indexed_rights=db_get_indexed_rights;
  ks->del_rtoken=db_del_rtoken;
  ks->del_wtoken=db_del_wtoken;
  return ks;
}

uint32_t hm_test_ks_db_destroy(hm_ks_db_t** ks){
  if(!ks || !(*ks)){
    return HM_FAIL;
  }
  tdestroy(((hm_test_ks_db_t*)((*ks)->user_data))->blocks, hm_ks_test_db_node_destroy);
  free((*ks)->user_data);
  free(*ks);
  *ks=NULL;
  return HM_SUCCESS;
}
