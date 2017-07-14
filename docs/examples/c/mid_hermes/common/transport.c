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

#include "transport.h"

#include <hermes/common/errors.h>
#include <assert.h>

typedef struct transport_type{
  int socket;
}transport_t;

uint32_t transport_send(void* t, const uint8_t* buf, const size_t buf_length){
  if(!t || !buf || !buf_length){
    return HM_FAIL;
  }
  if(buf_length!=write(((transport_t*)t)->socket, buf, buf_length)){
    return HM_FAIL;
  }
  return HM_SUCCESS;
}

uint32_t transport_recv(void* t, uint8_t* buf, size_t buf_length){
  if(!t || !buf || !buf_length){
    return HM_FAIL;
  }
  size_t readed_bytes, total_read=0;
  while(total_read<buf_length){
    if(!(readed_bytes=read(((transport_t*)t)->socket, buf+total_read, buf_length-total_read))){
	return HM_FAIL;
    }
    total_read+=readed_bytes;
  }
  return HM_SUCCESS;
}

hm_rpc_transport_t* transport_create(int socket){
  if(socket<0){
    return NULL;
  }
  hm_rpc_transport_t* res=calloc(1, sizeof(hm_rpc_transport_t));
  assert(res);
  res->user_data=calloc(1, sizeof(transport_t));
  assert(res->user_data);
  ((transport_t*)(res->user_data))->socket=socket;
  res->send=transport_send;
  res->recv=transport_recv;
  return res;
}

uint32_t transport_destroy(hm_rpc_transport_t** t){
  if(!t || !(*t) || !((*t)->user_data)){
    return HM_FAIL;
  }
  close(((transport_t*)((*t)->user_data))->socket);
  free((*t)->user_data);
  free(*t);
  *t=NULL;
  return HM_SUCCESS;
}
