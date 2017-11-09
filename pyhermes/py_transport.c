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

#include "py_transport.h"

static void HermesTransport_dealloc(pyhermes_HermesTransportObject_t *self) {
    transport_destroy(&(self->hermes_transport));
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static int HermesTransport_init(pyhermes_HermesTransportObject_t *self, PyObject *args, PyObject *kwds) {
    PyObject *transport = NULL;

    static char *kwlist[] = {"transport", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, &transport)) {
        return -1;
    }

    hm_rpc_transport_t* hermes_transport = transport_create(transport);
    if (!hermes_transport){
        return -1;
    }

    self->hermes_transport = hermes_transport;
    return 0;
}

static PyObject *HermesTransport_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    pyhermes_HermesTransportObject_t *self;
    self = (pyhermes_HermesTransportObject_t *) type->tp_alloc(type, 0);
    if (!self) {
        PyErr_SetString(HermesTransportError, "Hermes transport memory allocation error");
        return NULL;
    }
    if (0 != HermesTransport_init(self, args, kwds)) {
        PyErr_SetString(HermesTransportError, "Hermes transport object creation error");
        return NULL;
    }
    return (PyObject *) self;
}

static PyObject* HermesTransport_get_hermes_transport(pyhermes_HermesTransportObject_t* self, PyObject *unused){
    return HermesTransportWrapper_FromHmRpcTransport(self->hermes_transport);
}

static PyMethodDef HermesTransport_methods[] = {
        {"get_hermes_transport", (PyCFunction)HermesTransport_get_hermes_transport, METH_NOARGS,
                PyDoc_STR("return hermes transport object")},
        {NULL,	NULL},
};

PyTypeObject pyhermes_HermesTransportType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "pyhermes.HermesTransport",             /* tp_name */
        sizeof(pyhermes_HermesTransportObject_t), /* tp_basicsize */
        0,                         /* tp_itemsize */
        (destructor) HermesTransport_dealloc,         /* tp_dealloc */
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
        "Simple hermes transport",           /* tp_doc */
        0,                         /* tp_traverse */
        0,                         /* tp_clear */
        0,                         /* tp_richcompare */
        0,                         /* tp_weaklistoffset */
        0,                         /* tp_iter */
        0,                         /* tp_iternext */
        HermesTransport_methods,             /* tp_methods */
        NULL,             /* tp_members */
        0,                         /* tp_getset */
        0,                         /* tp_base */
        0,                         /* tp_dict */
        0,                         /* tp_descr_get */
        0,                         /* tp_descr_set */
        0,                         /* tp_dictoffset */
        NULL,                     /* tp_init */
        0,                         /* tp_alloc */
        HermesTransport_new,                 /* tp_new */
};

