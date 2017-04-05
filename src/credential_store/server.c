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

#include <hermes/credential_store/server.h>

#include <hermes/common/errors.h>
#include <hermes/rpc/server.h>

#include <assert.h>

#include "functions.h"

struct hm_credential_store_server_type{
  hm_rpc_server_t* s;
  hm_cs_db_t* db;
};

hm_credential_store_server_t* hm_credential_store_server_create(hm_rpc_transport_t* transport, hm_cs_db_t* db){
  if(!transport || !db){
    return NULL;
  }
  hm_credential_store_server_t* s=calloc(sizeof(hm_credential_store_server_t),1);
  assert(s);
  s->s=hm_rpc_server_create(transport);
  if(!(s->s)){
    hm_credential_store_server_destroy(&s);
    return NULL;
  }
  uint32_t res=hm_rpc_server_reg_func(s->s, (const uint8_t*)hm_credential_store_get_pub_key_by_id_NAME, sizeof(hm_credential_store_get_pub_key_by_id_NAME), hm_credential_store_get_pub_key_by_id_stub);
  if(HM_SUCCESS!=res){
    hm_credential_store_server_destroy(&s);
    return NULL;
  }
  s->db=db;
  return s;
}

uint32_t hm_credential_store_server_destroy(hm_credential_store_server_t** s){
  if(!s || !(*s)){
    return HM_INVALID_PARAMETER;
  }
  hm_rpc_server_destroy(&((*s)->s));
  free(*s);
  *s=NULL;
  return HM_SUCCESS; 
}

uint32_t hm_credential_store_server_call(hm_credential_store_server_t* s){
  if(!s){
    return HM_INVALID_PARAMETER;
  }
  return hm_rpc_server_call(s->s, (void*)(s->db));
}



