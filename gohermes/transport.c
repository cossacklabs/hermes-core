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


#include "transport.h"

#include <hermes/mid_hermes/mid_hermes.h>
#include "transport.h"
#include <stdio.h>

extern int go_transport_send(uint64_t transport, void* buf, const size_t buf_length);
extern int go_transport_recv(uint64_t transport, void* buf, const size_t buf_length);

typedef struct transport_type{
  uint64_t t;
}transport_t;

uint32_t transport_send(void* transport, const uint8_t* buf, const size_t buf_length){
  return go_transport_send(((transport_t*)transport)->t, (void*)buf, buf_length);
}

uint32_t transport_recv(void* transport, uint8_t* buf, const size_t buf_length){
  return go_transport_recv(((transport_t*)transport)->t, buf, buf_length);
}

hm_rpc_transport_t* transport_create(uint64_t transport){
  hm_rpc_transport_t* result=calloc(1, sizeof(hm_rpc_transport_t));
  if(!result){
    return NULL;
  }
  result->user_data=calloc(1, sizeof(transport_t));
  if(!result->user_data){
    return NULL;
  }
  ((transport_t*)(result->user_data))->t=transport;
  result->send=transport_send;
  result->recv=transport_recv;
  return result;
}

uint32_t transport_destroy(hm_rpc_transport_t** transport){
  if(!transport || !(*transport) || !((*transport)->user_data)){
    return HM_FAIL;
  }
  free((*transport)->user_data);
  free(*transport);
  *transport=NULL;
  return HM_SUCCESS;
}
