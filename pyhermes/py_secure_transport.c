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

#include <hermes/mid_hermes/mid_hermes.h>
#include <hermes/secure_transport/transport.h>
#include "py_secure_transport.h"
#include "transport.h"
#include "py_transport_wrapper.h"

extern PyObject *HermesTransportError;

static void SecureHermesTransport_dealloc(pyhermes_SecureHermesTransportObject_t *self) {
    transport_destroy(&(self->raw_transport));
    destroy_rpc_secure_transport(&self->hermes_transport);
    Py_TYPE(self)->tp_free((PyObject *) self);
}

// SecureHermesTransport(id, private, publicId, public, transport)
static int SecureHermesTransport_init(pyhermes_SecureHermesTransportObject_t *self, PyObject *args, PyObject *kwds) {
    PyObject *transport = NULL;
    const char *id = NULL;
    const char *private_key = NULL;
    const char *public_key_id = NULL;
    const char *public_key = NULL;
    Py_ssize_t id_length = 0;
    Py_ssize_t private_key_length = 0;
    Py_ssize_t public_key_id_length = 0;
    Py_ssize_t public_key_length = 0;
    unsigned char is_server = 0;

    static char *kwlist[] = {"user_id", "private_key", "public_key_id", "public_key", "transport", "is_server", NULL};
    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "s#s#s#s#OB", kwlist,
            &id, &id_length,
            &private_key, &private_key_length,
            &public_key_id, &public_key_id_length,
            &public_key, &public_key_length,
            &transport, &is_server)) {
        return -1;
    }
    if (id_length == 0 || private_key_length == 0 || public_key_id_length == 0 || public_key_length == 0) {
        return -1;
    }

    hm_rpc_transport_t *raw_transport = transport_create(transport);
    if (!raw_transport) {
        return -1;
    }

    hm_rpc_transport_t *secure_transport = create_secure_transport(
            (uint8_t *) id, (size_t) id_length,
            (uint8_t *) private_key, (size_t) private_key_length,
            (uint8_t *) public_key, (size_t) public_key_length,
            (uint8_t *) public_key_id, (size_t) public_key_id_length,
            raw_transport,
            is_server == 1);
    if (!secure_transport) {
        transport_destroy(&raw_transport);
        return -1;
    }
    self->hermes_transport = secure_transport;
    self->raw_transport = raw_transport;
    return 0;
}

static PyObject *SecureHermesTransport_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    pyhermes_SecureHermesTransportObject_t *self;
    self = (pyhermes_SecureHermesTransportObject_t *) type->tp_alloc(type, 0);
    if (!self) {
        PyErr_SetString(HermesTransportError, "Hermes transport memory allocation error");
        return NULL;
    }
    if (0 != SecureHermesTransport_init(self, args, kwds)) {
        PyErr_SetString(HermesTransportError, "Hermes transport object creation error");
        return NULL;
    }
    return (PyObject *) self;
}

static PyObject *
SecureHermesTransport_get_hermes_transport(pyhermes_SecureHermesTransportObject_t *self, PyObject *unused) {
    return HermesTransportWrapper_FromHmRpcTransport(self->hermes_transport);
}

static PyMethodDef SecureHermesTransport_methods[] = {
        {"get_hermes_transport", (PyCFunction) SecureHermesTransport_get_hermes_transport, METH_NOARGS,
                PyDoc_STR("return hermes transport object")},
        {NULL, NULL},
};

PyTypeObject pyhermes_SecureHermesTransportType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "pyhermes.SecureHermesTransport",             /* tp_name */
        sizeof(pyhermes_SecureHermesTransportObject_t), /* tp_basicsize */
        0,                         /* tp_itemsize */
        (destructor) SecureHermesTransport_dealloc,         /* tp_dealloc */
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
        "Hermes transport wrapper that uses secure session",           /* tp_doc */
        0,                         /* tp_traverse */
        0,                         /* tp_clear */
        0,                         /* tp_richcompare */
        0,                         /* tp_weaklistoffset */
        0,                         /* tp_iter */
        0,                         /* tp_iternext */
        SecureHermesTransport_methods,             /* tp_methods */
        NULL,             /* tp_members */
        0,                         /* tp_getset */
        0,                         /* tp_base */
        0,                         /* tp_dict */
        0,                         /* tp_descr_get */
        0,                         /* tp_descr_set */
        0,                         /* tp_dictoffset */
        NULL,//(initproc)MidHermes_init,      /* tp_init */
        0,                         /* tp_alloc */
        SecureHermesTransport_new,                 /* tp_new */
};

