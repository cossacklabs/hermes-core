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

#include "py_midhermes.h"

typedef struct {
    PyObject_HEAD
    PyObject *credential_store_transport;
    PyObject *data_store_transport;
    PyObject *key_store_transport;
    mid_hermes_t *mid_hermes;
} pyhermes_MidHermesObject;

static void MidHermes_dealloc(pyhermes_MidHermesObject *self) {
    mid_hermes_destroy(&(self->mid_hermes));
    Py_DECREF(self->credential_store_transport);
    Py_DECREF(self->key_store_transport);
    Py_DECREF(self->data_store_transport);
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static int MidHermes_init(pyhermes_MidHermesObject *self, PyObject *args, PyObject *kwds) {
    PyObject *credential_store_transport = NULL, *data_store_transport = NULL, *key_store_transport = NULL;
    const char *id = NULL, *sk = NULL;
    Py_ssize_t id_length = 0, sk_length = 0;
    static char *kwlist[] = {"id", "private_key", "cs_transport", "ds_transport", "ks_transport", NULL};
    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "s#s#OOO", kwlist, &id, &id_length, &sk, &sk_length, &credential_store_transport,
            &data_store_transport, &key_store_transport)) {
        return -1;
    }

    PyObject *transport = PyObject_CallMethod(credential_store_transport, "get_hermes_transport", NULL);
    if (!HermesTransportWrapper_Check(transport)) {
        PyErr_SetString(PyExc_TypeError, "credential_store return transport with incorrect type");
        return -1;
    }
    hm_rpc_transport_t *credential_hermes_transport = ((pyhermes_HermesTransportWrapperObject_t *) transport)->hermes_transport;

    transport = PyObject_CallMethod(key_store_transport, "get_hermes_transport", NULL);
    if (HermesTransportWrapper_Check(transport) != 1) {
        PyErr_SetString(PyExc_TypeError, "credential_store return transport with incorrect type");
        return -1;
    }
    hm_rpc_transport_t *key_hermes_transport = ((pyhermes_HermesTransportWrapperObject_t *) transport)->hermes_transport;

    transport = PyObject_CallMethod(data_store_transport, "get_hermes_transport", NULL);
    if (HermesTransportWrapper_Check(transport) != 1) {
        PyErr_SetString(PyExc_TypeError, "credential_store return transport with incorrect type");
        return -1;
    }
    hm_rpc_transport_t *data_hermes_transport = ((pyhermes_HermesTransportWrapperObject_t *) transport)->hermes_transport;

    if (!(credential_store_transport)
        || !(data_store_transport)
        || !(key_store_transport)
        || !(self->mid_hermes = mid_hermes_create(
            (const uint8_t *) id, (size_t) id_length, (const uint8_t *) sk, (size_t) sk_length,
            key_hermes_transport, data_hermes_transport, credential_hermes_transport))) {
        return -1;
    }
    self->credential_store_transport = credential_store_transport;
    Py_INCREF(credential_store_transport);

    self->data_store_transport = data_store_transport;
    Py_INCREF(data_store_transport);

    self->key_store_transport = key_store_transport;
    Py_INCREF(key_store_transport);

    return 0;
}

static PyObject *MidHermes_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    pyhermes_MidHermesObject *self;
    self = (pyhermes_MidHermesObject *) type->tp_alloc(type, 0);
    if (!self) {
        PyErr_SetString(HermesError, "MidHermes memory allocation error");
        return NULL;
    }
    if (0 != MidHermes_init(self, args, kwds)) {
        PyErr_SetString(HermesError, "MidHermes object creation error");
        return NULL;
    }
    return (PyObject *) self;
}

static PyObject *MidHermes_addBlock(pyhermes_MidHermesObject *self, PyObject *args, PyObject *kwds) {
    const char *block_id = NULL, *block = NULL, *meta = NULL;
    size_t block_id_length = 0, block_length = 0, meta_length = 0;
    static char *kwlist[] = {"id", "data", "meta", NULL};
    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "s#s#s#", kwlist, &block_id, &block_id_length, &block, &block_length, &meta, &meta_length)) {
        PyErr_SetString(HermesError, "MidHermes.addBlock invalid parameters");
        return NULL;
    }
    if (0 != mid_hermes_create_block(
            self->mid_hermes, (uint8_t **) (&block_id), &block_id_length, (const uint8_t *) block, block_length,
            (const uint8_t *) meta, meta_length)) {
        PyErr_SetString(HermesError, "MidHermes.addBlock error");
        return NULL;
    }
    Py_RETURN_NONE;
}

static PyObject *MidHermes_updBlock(pyhermes_MidHermesObject *self, PyObject *args, PyObject *kwds) {
    const char *block_id = NULL, *block = NULL, *meta = NULL;
    size_t block_id_length = 0, block_length = 0, meta_length = 0;
    static char *kwlist[] = {"id", "data", "meta", NULL};
    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "s#s#s#", kwlist, &block_id, &block_id_length, &block, &block_length, &meta, &meta_length)) {
        PyErr_SetString(HermesError, "MidHermes.updBlock invalid parameters");
        return NULL;
    }
    if (0 != mid_hermes_update_block(
            self->mid_hermes, (const uint8_t *) block_id, block_id_length, (const uint8_t *) block, block_length,
            (const uint8_t *) meta, meta_length)) {
        PyErr_SetString(HermesError, "MidHermes.updBlock error");
        return NULL;
    }
    Py_RETURN_NONE;
}

static PyObject *MidHermes_delBlock(pyhermes_MidHermesObject *self, PyObject *args, PyObject *kwds) {
    const char *block_id = NULL;
    size_t block_id_length = 0;
    static char *kwlist[] = {"id", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s#", kwlist, &block_id, &block_id_length)) {
        PyErr_SetString(HermesError, "MidHermes.delBlock invalid parameters");
        return NULL;
    }
    if (0 != mid_hermes_delete_block(self->mid_hermes, (const uint8_t *) block_id, block_id_length)) {
        PyErr_SetString(HermesError, "MidHermes.delBlock error");
        return NULL;
    }
    Py_RETURN_NONE;
}

static PyObject *MidHermes_getBlock(pyhermes_MidHermesObject *self, PyObject *args, PyObject *kwds) {
    const char *block_id = NULL, *block = NULL, *meta = NULL;
    size_t block_id_length = 0, block_length = 0, meta_length = 0;
    static char *kwlist[] = {"id", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s#", kwlist, &block_id, &block_id_length)) {
        PyErr_SetString(HermesError, "MidHermes.getBlock invalid parameters");
        return NULL;
    }
    if (0 != mid_hermes_read_block(
            self->mid_hermes, (const uint8_t *) block_id, block_id_length, (uint8_t **) (&block), &block_length,
            (uint8_t **) (&meta), &meta_length)) {
        PyErr_SetString(HermesError, "MidHermes.getBlock error");
        return NULL;
    }
    return Py_BuildValue("OO", PyBytes_FromStringAndSize(block, block_length), PyBytes_FromStringAndSize(meta, meta_length));
}

static PyObject *MidHermes_rotateBlock(pyhermes_MidHermesObject *self, PyObject *args, PyObject *kwds) {
    const char *block_id = NULL;
    size_t block_id_length = 0;
    static char *kwlist[] = {"id", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s#", kwlist, &block_id, &block_id_length)) {
        PyErr_SetString(HermesError, "MidHermes.rotateBlock invalid parameters");
        return NULL;
    }
    if (0 != mid_hermes_rotate_block(
            self->mid_hermes, (const uint8_t *) block_id, block_id_length)) {
        PyErr_SetString(HermesError, "MidHermes.rotateBlock error");
        return NULL;
    }
    Py_RETURN_NONE;
}

static PyObject *MidHermes_grantReadAccess(pyhermes_MidHermesObject *self, PyObject *args, PyObject *kwds) {
    const char *block_id = NULL, *user_id = NULL;
    size_t block_id_length = 0, user_id_length = 0;
    static char *kwlist[] = {"id", "user", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s#s#", kwlist, &block_id, &block_id_length, &user_id,
                                     &user_id_length)) {
        PyErr_SetString(HermesError, "MidHermes.grantReadAccess invalid parameters");
        return NULL;
    }
    if (0 != mid_hermes_grant_read_access(
            self->mid_hermes, (const uint8_t *) block_id, block_id_length, (const uint8_t *) user_id, user_id_length)) {
        PyErr_SetString(HermesError, "MidHermes.grantReadAccess error");
        return NULL;
    }
    Py_RETURN_NONE;
}

static PyObject *MidHermes_grantUpdateAccess(pyhermes_MidHermesObject *self, PyObject *args, PyObject *kwds) {
    const char *block_id = NULL, *user_id = NULL;
    size_t block_id_length = 0, user_id_length = 0;
    static char *kwlist[] = {"id", "user", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s#s#", kwlist, &block_id, &block_id_length, &user_id,
                                     &user_id_length)) {
        PyErr_SetString(HermesError, "MidHermes.grantUpdateAccess invalid parameters");
        return NULL;
    }
    if (0 != mid_hermes_grant_update_access(
            self->mid_hermes, (const uint8_t *) block_id, block_id_length, (const uint8_t *) user_id, user_id_length)) {
        PyErr_SetString(HermesError, "MidHermes.grantUpdateAccess error");
        return NULL;
    }
    Py_RETURN_NONE;
}

static PyObject *MidHermes_denyReadAccess(pyhermes_MidHermesObject *self, PyObject *args, PyObject *kwds) {
    const char *block_id = NULL, *user_id = NULL;
    size_t block_id_length = 0, user_id_length = 0;
    static char *kwlist[] = {"id", "user", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s#s#", kwlist, &block_id, &block_id_length, &user_id,
                                     &user_id_length)) {
        PyErr_SetString(HermesError, "MidHermes.denyReadAccess invalid parameters");
        return NULL;
    }
    if (0 != mid_hermes_deny_read_access(
            self->mid_hermes, (const uint8_t *) block_id, block_id_length, (const uint8_t *) user_id, user_id_length)) {
        PyErr_SetString(HermesError, "MidHermes.denyReadAccess error");
        return NULL;
    }
    Py_RETURN_NONE;
}

static PyObject *MidHermes_denyUpdateAccess(pyhermes_MidHermesObject *self, PyObject *args, PyObject *kwds) {
    const char *block_id = NULL, *user_id = NULL;
    size_t block_id_length = 0, user_id_length = 0;
    static char *kwlist[] = {"id", "user", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s#s#", kwlist, &block_id, &block_id_length, &user_id,
                                     &user_id_length)) {
        PyErr_SetString(HermesError, "MidHermes.denyUpdateAccess invalid parameters");
        return NULL;
    }
    if (0 != mid_hermes_deny_update_access(
            self->mid_hermes, (const uint8_t *) block_id, block_id_length, (const uint8_t *) user_id, user_id_length)) {
        PyErr_SetString(HermesError, "MidHermes.denyUpdateAccess error");
        return NULL;
    }
    Py_RETURN_NONE;
}


static PyMethodDef MidHermes_methods[] = {
        {"addBlock",          (PyCFunction) MidHermes_addBlock,
                                                                         METH_VARARGS |
                                                                         METH_KEYWORDS, "add (block, meta) to hermes"},
        {"getBlock",          (PyCFunction) MidHermes_getBlock,          METH_VARARGS |
                                                                         METH_KEYWORDS, "return (block, meta) from hermes"},
        {"updBlock",          (PyCFunction) MidHermes_updBlock,          METH_VARARGS |
                                                                         METH_KEYWORDS, "update (block, meta) in hermes"},
        {"delBlock",          (PyCFunction) MidHermes_delBlock,          METH_VARARGS |
                                                                         METH_KEYWORDS, "delete (block, meta) from hermes"},
        {"rotateBlock",       (PyCFunction) MidHermes_rotateBlock,       METH_VARARGS |
                                                                         METH_KEYWORDS, "rotete block in hermes"},
        {"grantReadAccess",   (PyCFunction) MidHermes_grantReadAccess,   METH_VARARGS |
                                                                         METH_KEYWORDS, "grant read access to user for block"},
        {"grantUpdateAccess", (PyCFunction) MidHermes_grantUpdateAccess, METH_VARARGS |
                                                                         METH_KEYWORDS, "grant update access to user for block"},
        {"denyReadAccess",    (PyCFunction) MidHermes_denyReadAccess,    METH_VARARGS |
                                                                         METH_KEYWORDS, "deny read access to user for block"},
        {"denyUpdateAccess",  (PyCFunction) MidHermes_denyUpdateAccess,  METH_VARARGS |
                                                                         METH_KEYWORDS, "deny update access to user for block"},
        {NULL}  /* Sentinel */
};

PyTypeObject pyhermes_MidHermesType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "hermes_core.MidHermes",             /* tp_name */
        sizeof(pyhermes_MidHermesObject), /* tp_basicsize */
        0,                         /* tp_itemsize */
        (destructor) MidHermes_dealloc,         /* tp_dealloc */
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
        "MidHermes objects",           /* tp_doc */
        0,                         /* tp_traverse */
        0,                         /* tp_clear */
        0,                         /* tp_richcompare */
        0,                         /* tp_weaklistoffset */
        0,                         /* tp_iter */
        0,                         /* tp_iternext */
        MidHermes_methods,             /* tp_methods */
        NULL,             /* tp_members */
        0,                         /* tp_getset */
        0,                         /* tp_base */
        0,                         /* tp_dict */
        0,                         /* tp_descr_get */
        0,                         /* tp_descr_set */
        0,                         /* tp_dictoffset */
        NULL,//(initproc)MidHermes_init,      /* tp_init */
        0,                         /* tp_alloc */
        MidHermes_new,                 /* tp_new */
};
