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
#include <hermes/common/errors.h>

uint32_t transport_send(void* transport, const uint8_t* buf, const size_t buf_length){
  if(!transport || !buf || !buf_length){
    return HM_FAIL;
  }
  PyObject_CallMethod((PyObject*)transport, "send", "y#", (const char*)buf, buf_length);
  return HM_SUCCESS;
}

uint32_t transport_recv(void* transport, uint8_t* buf, size_t buf_length){
  if(!transport || !buf || !buf_length){
    return HM_FAIL;
  }
  PyObject* result=PyObject_CallMethod((PyObject*)transport, "receive", "I", buf_length);
  if(!result){
    return HM_FAIL;
  }
  if(!(PyBytes_Check(result) || ((size_t)PyBytes_Size(result)!=buf_length))){
    Py_XDECREF(result);
    return HM_FAIL;
  }
  memcpy(buf, PyBytes_AsString(result), buf_length);
  return HM_SUCCESS;
}

hm_rpc_transport_t* transport_create(PyObject* transport){
  hm_rpc_transport_t* result=calloc(1, sizeof(hm_rpc_transport_t));
  if(!result){
    return NULL;
  }
  // not assign transport itself to avoid free hm_rpc_transport_t->user_data without PY_DECREF
  result->user_data=transport;
  Py_INCREF(transport);
  result->send=transport_send;
  result->recv=transport_recv;
  return result;
}

uint32_t transport_destroy(hm_rpc_transport_t** transport){
  if(!transport || !(*transport) || !((*transport)->user_data)){
    return HM_FAIL;
  }
  Py_XDECREF((PyObject*)(*transport)->user_data);
  free(*transport);
  *transport=NULL;
  return HM_SUCCESS;
}
