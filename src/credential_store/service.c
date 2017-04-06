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

#include <hermes/credential_store/service.h>
#include <hermes/credential_store/server.h>
#include <hermes/common/errors.h>
#include <assert.h>
#include <stdbool.h>

struct hm_credential_server_service_type{
  hm_credential_store_server_t* s;
  bool finish;
};

hm_credential_store_service_t* hm_credential_store_service_create(hm_rpc_transport_t* transport, hm_cs_db_t* db){
  if(!transport || !db){
    return NULL;
  }
  hm_credential_store_service_t* s=calloc(sizeof(hm_credential_store_service_t), 1);
  assert(s);
  if(!(s->s=hm_credential_store_server_create(transport, db))){
    free(s);
    return NULL;
  }
  return s;
}

uint32_t hm_credential_store_service_destroy(hm_credential_store_service_t** s){
  if(!s || !(*s)){
    return HM_INVALID_PARAMETER;
  }
  hm_credential_store_server_destroy(&((*s)->s));
  free(*s);
  *s=NULL;
  return HM_SUCCESS;
}

uint32_t hm_credential_store_service_start(hm_credential_store_service_t* s){
  if(!s){
    return HM_INVALID_PARAMETER;
  }
  while(true){
    if(s->finish){
      return HM_SUCCESS;
    }
    //TODO logging
    if(HM_SUCCESS!=hm_credential_store_server_call(s->s)){
      return HM_FAIL;
    }
  }
  return HM_FAIL;
}

uint32_t hm_credential_store_service_stop(hm_credential_store_service_t* s){
  if(!s){
    return HM_INVALID_PARAMETER;
  }
  s->finish=true;
  return HM_SUCCESS;
}
