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



#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <Python.h>
#include "bytesobject.h"
#include <hermes/rpc/transport.h>
#include <hermes/common/errors.h>

extern PyObject *HermesTransportError;

hm_rpc_transport_t *transport_create(PyObject *transport);

uint32_t transport_destroy(hm_rpc_transport_t **t);

#endif //TRANSPORT_H
