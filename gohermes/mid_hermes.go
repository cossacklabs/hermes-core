//
// Copyright (c) 2017 Cossack Labs Limited
//
// This file is part of Hermes.
//
// Hermes is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Hermes is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with Hermes.  If not, see <http://www.gnu.org/licenses/>.
//
//

package gohermes

/*
#include <hermes/rpc/transport.h>
#include <stdint.h>
#include <assert.h>

extern int go_transport_send(void* transport, const uint8_t* buf, const size_t buf_length);
extern int go_transport_recv(void* transport, uint8_t* buf, const size_t buf_length);

typedef struct transport_type{
  void* t;
}transport_t;

uint32_t transport_send(void* transport, const uint8_t* buf, const size_t buf_length){
   return go_transport_send(transport, buf, buf_length);
}

uint32_t transport_recv(void* transport, uint8_t* buf, const size_t buf_length){
   return go_transport_recv(transport, buf, buf_length);
}

transport_t* create(void* transport){
  hm_rpc_transport_t* result=calloc(1, sizeof(hm_rpc_transport_t));
  if(!result){
    return NULL;
  }
  result->user_data=calloc(1, sizeof(transport_t));
  assert(result->user_data);
  Py_INCREF(transport);
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
*/

import "C"

import (
	"unsafe"
)

type transport_t interface{
	send(buf []byte) error
	recv(buf []byte) error
}

//export go_transport_send
func go_transport_send(transport unsafe.Pointer, buf unsafe.Pointer, buf_length C.size_t) C.int {
	p := (*transport_t)(transport)
	b := (*[1<<30]byte)(buf)[0:buf_length]
	err := (*p).send(b)
	if nil != err {
		return 1;
	}
	return 0;
}

//export go_transport_recv
func go_transport_recv(transport unsafe.Pointer, buf unsafe.Pointer, buf_length C.size_t) C.int{
	p := (*transport_t)(transport)
	b := (*[1<<30]byte)(buf)[0:buf_length]
	err := (*p).recv(b)
	if nil != err {
		return 1;
	}
	return 0;	
}

