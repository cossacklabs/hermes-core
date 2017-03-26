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

#include <hermes/rpc/transport.h>
#include <hermes/common/errors.h>

#include "test_transport.h"

#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

struct hm_rpc_transport_type{
  int read_pipe;
  int write_pipe;
  bool is_server;
};

hm_rpc_transport_t* hm_test_transport_create(bool is_server){
  hm_rpc_transport_t* t = calloc(sizeof(hm_rpc_transport_t), 1);
  assert(t);
  t->is_server=is_server;
  if(is_server){
    t->write_pipe=open(SC_PIPE_NAME, O_WRONLY);
    t->read_pipe=open(CS_PIPE_NAME, O_RDONLY); 
  } else {
    t->write_pipe=open(CS_PIPE_NAME, O_WRONLY);
    t->read_pipe=open(SC_PIPE_NAME, O_RDONLY); 
  }
  if(!(t->write_pipe) || !(t->read_pipe)){
    hm_test_transport_destroy(t);
    return NULL;
  }
  return t;
}

uint32_t hm_test_transport_destroy(hm_rpc_transport_t* t){
  if(!t){
    return HM_INVALID_PARAMETER;
  }
  if(t->write_pipe){
    close(t->write_pipe);
  }
  if(t->read_pipe){
    close(t->read_pipe);
  }
  free(t);
  return HM_SUCCESS;
}

uint32_t hm_rpc_transport_send(hm_rpc_transport_t* transport, const uint8_t* buffer, const size_t buffer_length){
  if(!transport || !buffer || !buffer_length){
    return HM_INVALID_PARAMETER;
  }
  ssize_t sended_bytes=write(transport->write_pipe, buffer, buffer_length);
  if(sended_bytes!=buffer_length){
    return HM_FAIL;
  }
  return HM_SUCCESS;
}
uint32_t hm_rpc_transport_recv(hm_rpc_transport_t* transport, uint8_t* buffer, size_t buffer_length){
  if(!transport || !buffer || !buffer_length){
    return HM_INVALID_PARAMETER;
  }
  ssize_t readed_bytes=read(transport->write_pipe, buffer, buffer_length);
  if(readed_bytes!=buffer_length){
    return HM_FAIL;
  }
  return HM_SUCCESS;
}
