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

#include <hermes/common/errors.h>
#include <hermes/key_store/client.h>
#include <hermes/rpc/client.h>

#include <assert.h>

#include "functions.h"

struct hm_key_store_client_sync_type{
  hm_rpc_client_sync_t* c;
};

hm_key_store_client_sync_t* hm_key_store_client_sync_create(hm_rpc_transport_t* transport){
  if(!transport){
    return NULL;
  }
  hm_key_store_client_sync_t* c=calloc(sizeof(hm_key_store_client_sync_t), 1);
  assert(c);
  c->c = hm_rpc_client_sync_create(transport);
  if(!(c->c)){
    hm_key_store_client_sync_destroy(&c);
    return NULL;
  }
  return c;
}

uint32_t hm_key_store_client_sync_destroy(hm_key_store_client_sync_t** c){
  if(!c || !(*c)){
    return HM_INVALID_PARAMETER;
  }
  hm_rpc_client_sync_destroy(&((*c)->c));
  free(*c);
  *c=NULL;
  return HM_SUCCESS;
}


uint32_t hm_key_store_client_sync_call_set_rtoken(hm_key_store_client_sync_t* c, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length, const uint8_t* rtoken, const size_t rtoken_length){
  if(!c){
    return HM_INVALID_PARAMETER;
  }
  return hm_key_store_set_rtoken_sync_proxy(c->c, block_id, block_id_length, user_id, user_id_length, owner_id, owner_id_length, rtoken, rtoken_length);
}

uint32_t hm_key_store_client_sync_call_set_wtoken(hm_key_store_client_sync_t* c, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length, const uint8_t* wtoken, const size_t wtoken_length){
  if(!c){
    return HM_INVALID_PARAMETER;
  }
  return hm_key_store_set_wtoken_sync_proxy(c->c, block_id, block_id_length, user_id, user_id_length, owner_id, owner_id_length, wtoken, wtoken_length);
}

uint32_t hm_key_store_client_sync_call_get_rtoken(hm_key_store_client_sync_t* c, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, uint8_t** rtoken, size_t* rtoken_length, uint8_t** owner_id, size_t* owner_id_length){
  if(!c){
    return HM_INVALID_PARAMETER;
  }
  return hm_key_store_get_rtoken_sync_proxy(c->c, block_id, block_id_length, user_id, user_id_length, rtoken, rtoken_length, owner_id, owner_id_length);
}

uint32_t hm_key_store_client_sync_call_get_wtoken(hm_key_store_client_sync_t* c, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, uint8_t** wtoken, size_t* wtoken_length, uint8_t** owner_id, size_t* owner_id_length){
  if(!c){
    return HM_INVALID_PARAMETER;
  }
  return hm_key_store_get_wtoken_sync_proxy(c->c, block_id, block_id_length, user_id, user_id_length, wtoken, wtoken_length, owner_id, owner_id_length);
}

uint32_t hm_key_store_client_sync_call_del_rtoken(hm_key_store_client_sync_t* c, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length){
  if(!c){
    return HM_INVALID_PARAMETER;
  }
  return hm_key_store_del_rtoken_sync_proxy(c->c, block_id, block_id_length, user_id, user_id_length, owner_id, owner_id_length); 
}

uint32_t hm_key_store_client_sync_call_del_wtoken(hm_key_store_client_sync_t* c, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length){
  if(!c){
    return HM_INVALID_PARAMETER;
  }
  return hm_key_store_del_wtoken_sync_proxy(c->c, block_id, block_id_length, user_id, user_id_length, owner_id, owner_id_length); 
}

uint32_t hm_key_store_client_sync_call_get_indexed_rights(hm_key_store_client_sync_t* c, const uint8_t* block_id, const size_t block_id_length, const size_t index, uint8_t** user_id, size_t* user_id_length, uint32_t* rights_mask){
  if(!c){
    return HM_INVALID_PARAMETER;
  }
  return hm_key_store_get_indexed_rights_proxy(c->c, block_id, block_id_length, index, user_id, user_id_length, rights_mask); 
}
