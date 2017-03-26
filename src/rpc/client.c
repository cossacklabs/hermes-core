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


#include <hermes/rpc/client.h>
#include <hermes/common/errors.h>

#include <assert.h>

struct hm_rpc_client_sync_type{
  hm_rpc_transport_t* transport;
};

hm_rpc_client_sync_t* hm_rpc_client_sync_create(hm_rpc_transport_t* transport){
  if(!transport){
    return NULL;
  }
  hm_rpc_client_sync_t* res = calloc(sizeof(hm_rpc_client_sync_t), 1);
  assert(res);
  res->transport=transport;
  return res;
}

uint32_t hm_rpc_client_sync_destroy(hm_rpc_client_sync_t** c){
  if(!c || !(*c)){
    return HM_INVALID_PARAMETER;
  }
  free(*c);
  *c=NULL;
  return HM_SUCCESS;
}

uint32_t hm_rpc_client_sync_call(hm_rpc_client_sync_t* c, const uint8_t* func_name, const size_t func_name_length, hm_param_pack_t* in_params, uint32_t *error, hm_param_pack_t** out_params){
  if(!c || !func_name || !func_name_length || !in_params){
    return HM_INVALID_PARAMETER;
  }
  uint32_t res=HM_SUCCESS;
  if(HM_SUCCESS!=(res=hm_rpc_transport_send(c->transport, (const uint8_t*)(&func_name_length), sizeof(uint32_t)))){
    return res;
  }
  if(HM_SUCCESS!=(res=hm_rpc_transport_send(c->transport, func_name, func_name_length))){
    return res;
  }
  if(HM_SUCCESS!=(res=hm_param_pack_send(in_params, c->transport))){
    return res;
  }
  if(HM_SUCCESS!=(res=hm_rpc_transport_recv(c->transport, (uint8_t*)error, sizeof(uint32_t)))){
    return res;
  }
  (*out_params)=hm_param_pack_receive(c->transport);
  if(!*out_params){
    return HM_FAIL;
  }
  return HM_SUCCESS;
}

