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

#include <Python.h>
#include <hermes/rpc/transport.h>
#include "transport.h"
#include "py_transport_wrapper.h"

PyObject *HermesTransportWrapper_FromHmRpcTransport(hm_rpc_transport_t *transport) {
    pyhermes_HermesTransportWrapperObject_t *object = (pyhermes_HermesTransportWrapperObject_t *) PyObject_New(
            pyhermes_HermesTransportWrapperObject_t, &pyhermes_HermesTransportWrapperType);
    if (!object) {
        return PyErr_NoMemory();
    }
    object->hermes_transport = transport;
    Py_INCREF(object);
    return (PyObject *) object;
}

static PyObject *HermesTransportWrapper_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    pyhermes_HermesTransportWrapperObject_t *self;
    self = (pyhermes_HermesTransportWrapperObject_t *) type->tp_alloc(type, 0);
    if (!self) {
        PyErr_SetString(HermesTransportError, "Hermes transport memory allocation error");
        return NULL;
    }
    return (PyObject *) self;
}

int HermesTransportWrapper_Check(PyObject *transport) {
    return PyObject_TypeCheck(transport, &pyhermes_HermesTransportWrapperType);
}


static void HermesTransportWrapper_dealloc(pyhermes_HermesTransportWrapperObject_t *self) {
    Py_TYPE(self)->tp_free((PyObject *) self);
}

PyTypeObject pyhermes_HermesTransportWrapperType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "pyhermes.HermesTransportWrapper",             /* tp_name */
        sizeof(pyhermes_HermesTransportWrapperObject_t), /* tp_basicsize */
        0,                         /* tp_itemsize */
        (destructor) HermesTransportWrapper_dealloc,         /* tp_dealloc */
        0,                         /* tp_print */
        0,                         /* tp_getattr */
        0,                         /* tp_setattr */
        0,                         /* tp_reserved */
        0,                         /* tp_repr */
        0,                         /* tp_as_number */
        0,                         /* tp_as_sequence */
        0,                         /* tp_as_mapping */
        0,                         /* tp_hash  */
        0,                         /* tp_call */
        0,                         /* tp_str */
        0,                         /* tp_getattro */
        0,                         /* tp_setattro */
        0,                         /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,        /* tp_flags */
        "Hermes transport wrapper",           /* tp_doc */
        0,                         /* tp_traverse */
        0,                         /* tp_clear */
        0,                         /* tp_richcompare */
        0,                         /* tp_weaklistoffset */
        0,                         /* tp_iter */
        0,                         /* tp_iternext */
        NULL,             /* tp_methods */
        NULL,             /* tp_members */
        0,                         /* tp_getset */
        0,                         /* tp_base */
        0,                         /* tp_dict */
        0,                         /* tp_descr_get */
        0,                         /* tp_descr_set */
        0,                         /* tp_dictoffset */
        NULL,//(initproc)MidHermes_init,      /* tp_init */
        0,                         /* tp_alloc */
        HermesTransportWrapper_new,                 /* tp_new */
};