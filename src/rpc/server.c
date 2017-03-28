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


#include <hermes/rpc/server.h>
#include <hermes/common/hash_table.h>
#include <hermes/common/errors.h>
#include <assert.h>

struct hm_rpc_server_type{
  hm_rpc_transport_t* transport;
  hm_hash_table_t* func_table;
};

hm_rpc_server_t* hm_rpc_server_create(hm_rpc_transport_t* transport){
  if(!transport){
    return NULL;
  }
  hm_rpc_server_t* s=calloc(sizeof(hm_rpc_server_t),1);
  assert(s);
  s->transport=transport;
  s->func_table=hm_hash_table_create();
  if(!(s->func_table)){
    hm_rpc_server_destroy(&s);
    return NULL;
  }
  return s;
}

uint32_t hm_rpc_server_destroy(hm_rpc_server_t** s){
  if(!s || !(*s)){
    return HM_INVALID_PARAMETER;
  }
  hm_hash_table_destroy(&((*s)->func_table));
  free(*s);
  *s=NULL;
  return HM_SUCCESS;
}

uint32_t hm_rpc_server_reg_func(hm_rpc_server_t* s, const uint8_t* func_signature, const size_t func_signature_length, hm_server_func_t func){
  if(!s || !func_signature || func_signature_length || !func){
    return HM_INVALID_PARAMETER;
  }
  return hm_hash_table_set(s->func_table, func_signature, func_signature_length, (const uint8_t*)func, sizeof(hm_server_func_t));
}

uint32_t hm_rpc_server_send_error(hm_rpc_server_t* s, uint32_t error){
  if(!s || HM_SUCCESS==error){
    return HM_INVALID_PARAMETER;
  }
  return hm_rpc_transport_send(s->transport, (uint8_t*)&error, sizeof(uint32_t));
}

uint32_t hm_rpc_server_send(hm_rpc_server_t* s, hm_param_pack_t* pack){
  if(!s || !pack){
    return HM_INVALID_PARAMETER;
  }
  size_t err=HM_SUCCESS;
  if(HM_SUCCESS!=hm_rpc_transport_send(s->transport, (uint8_t*)&err, sizeof(uint32_t))){
    return HM_FAIL;
  }
  return hm_param_pack_send(pack, s->transport);
}


uint32_t hm_rpc_server_call(hm_rpc_server_t* s, const uint8_t* func_signature, const size_t func_signature_length){
  if(!s || !func_signature || !func_signature_length){
    return HM_INVALID_PARAMETER;
  }
  hm_server_func_t func=NULL;
  size_t func_length=sizeof(hm_server_func_t);
  if(HM_SUCCESS!=hm_hash_table_get(s->func_table, func_signature, func_signature_length, (uint8_t**)&func, &func_length)){
    hm_rpc_server_send_error(s, HM_FAIL);
    return HM_FAIL;    
  }
  hm_param_pack_t* pack=hm_param_pack_receive(s->transport);
  if(!pack){
    hm_rpc_server_send_error(s, HM_FAIL);
    return HM_FAIL;
  }
  hm_param_pack_t* out_pack=NULL;
  uint32_t res=func(pack, &out_pack);
  return hm_rpc_server_send(s, out_pack);
}
