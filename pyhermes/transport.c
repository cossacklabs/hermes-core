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
  PyObject* t;
}transport_t;

uint32_t transport_send(void* t, const uint8_t* buf, const size_t buf_length){
  if(!t || !buf || !buf_length){
    return HM_FAIL;
  }
  PyObject_CallMethod(((transport_t*)t)->t, "send", "y#", (const char*)buf, buf_length);
  return HM_SUCCESS;
}

uint32_t transport_recv(void* t, uint8_t* buf, size_t buf_length){
  if(!t || !buf || !buf_length){
    return HM_FAIL;
  }
  PyObject* res=PyObject_CallMethod(((transport_t*)t)->t, "receive", "I", buf_length);
  if(!res){
    return HM_FAIL;
  }
  if(!(PyBytes_Check(res) || (PyBytes_Size(res)!=buf_length))){
    Py_XDECREF(res);
    return HM_FAIL;
  }
  memcpy(buf, PyBytes_AsString(res), buf_length);
  return HM_SUCCESS;
}

hm_rpc_transport_t* transport_create(PyObject* transport){
  hm_rpc_transport_t* res=calloc(1, sizeof(hm_rpc_transport_t));
  if(!res){
    return NULL;
  }
  res->user_data=calloc(1, sizeof(transport_t));
  assert(res->user_data);
  Py_INCREF(transport);
  ((transport_t*)(res->user_data))->t=transport;
  res->send=transport_send;
  res->recv=transport_recv;
  return res;
}

uint32_t transport_destroy(hm_rpc_transport_t** t){
  if(!t || !(*t) || !((*t)->user_data)){
    return HM_FAIL;
  }
  Py_XDECREF(((transport_t*)((*t)->user_data))->t);
  free((*t)->user_data);
  free(*t);
  *t=NULL;
  return HM_SUCCESS;
}
