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
#include <hermes/data_store/client.h>
#include <hermes/rpc/client.h>

#include <assert.h>

#include "functions.h"

struct hm_data_store_client_sync_type{
  hm_rpc_client_sync_t* c;
};

hm_data_store_client_sync_t* hm_data_store_client_sync_create(hm_rpc_transport_t* transport){
  if(!transport){
    return NULL;
  }
  hm_data_store_client_sync_t* c=calloc(sizeof(hm_data_store_client_sync_t), 1);
  assert(c);
  c->c = hm_rpc_client_sync_create(transport);
  if(!(c->c)){
    hm_data_store_client_sync_destroy(&c);
    return NULL;
  }
  return c;
}

uint32_t hm_data_store_client_sync_destroy(hm_data_store_client_sync_t** c){
  if(!c || !(*c)){
    return HM_INVALID_PARAMETER;
  }
  hm_rpc_client_sync_destroy(&((*c)->c));
  free(*c);
  *c=NULL;
  return HM_SUCCESS;
}

uint32_t hm_data_store_client_sync_call_create_block(hm_data_store_client_sync_t* c, const uint8_t* block, const size_t block_length, const uint8_t* mac, const size_t mac_length, uint8_t** id, size_t* id_length){
  if(!c || !block || !block_length || !mac || !mac_length || !id){
    return HM_INVALID_PARAMETER;
  }
  return hm_data_store_create_block_sync_proxy(c->c, block, block_length, mac,  mac_length, id, id_length);
}

uint32_t hm_data_store_client_sync_call_read_block(hm_data_store_client_sync_t* c, const uint8_t* id, const size_t id_length, uint8_t** block, size_t*  block_length){
  if(!c || !id || !id_length || !block){
    return HM_INVALID_PARAMETER;
  }
  return hm_data_store_read_block_sync_proxy(c->c, id, id_length, block, block_length);
}

uint32_t hm_data_store_client_sync_call_update_block(hm_data_store_client_sync_t* c, const uint8_t* id, const size_t id_length, const uint8_t* block, const size_t block_length, const uint8_t* mac, const size_t mac_length, const uint8_t* old_mac, const size_t old_mac_length){
  if(!c || !id || !id_length || !block || !block_length || !mac || !mac_length || !old_mac || !old_mac_length){
    return HM_INVALID_PARAMETER;
  }
  return hm_data_store_update_block_sync_proxy(c->c, id, id_length, block, block_length, mac, mac_length, old_mac, old_mac_length);
}

uint32_t hm_data_store_client_sync_call_delete_block(hm_data_store_client_sync_t* c, const uint8_t* id, const size_t id_length, const uint8_t* old_mac, const size_t old_mac_length){
  if(!c || !id || !id_length || !old_mac || !old_mac_length){
    return HM_INVALID_PARAMETER;
  }
  return hm_data_store_delete_block_sync_proxy(c->c, id, id_length, old_mac, old_mac_length);  
}
