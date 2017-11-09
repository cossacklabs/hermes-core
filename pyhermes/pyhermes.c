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

PyObject *HermesError;
extern PyObject *HermesTransportError;
extern PyTypeObject pyhermes_SecureHermesTransportType;
extern PyTypeObject pyhermes_MidHermesType;
extern PyTypeObject pyhermes_HermesTransportType;

static PyModuleDef pyhermesmodule = {
        PyModuleDef_HEAD_INIT,
        "pyhermes",
        "PyHermes",
        -1,
        NULL, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC PyInit_pyhermes(void) {
    PyObject *m;

    if (PyType_Ready(&pyhermes_MidHermesType) < 0){
        return NULL;
    }

    if (PyType_Ready(&pyhermes_SecureHermesTransportType) < 0){
        return NULL;
    }

    if (PyType_Ready(&pyhermes_HermesTransportType) < 0){
        return NULL;
    }

    m = PyModule_Create(&pyhermesmodule);
    if (m == NULL)
        return NULL;

    HermesError = PyErr_NewException("pyhermes.HermesError", NULL, NULL);
    Py_INCREF(HermesError);
    PyModule_AddObject(m, "HermesError", HermesError);

    HermesTransportError = PyErr_NewException("pyhermes.HermesTransportError", NULL, NULL);
    Py_INCREF(HermesTransportError);
    PyModule_AddObject(m, "HermesTransportError", HermesTransportError);

    Py_INCREF(&pyhermes_MidHermesType);
    PyModule_AddObject(m, "MidHermes", (PyObject *) &pyhermes_MidHermesType);

    Py_INCREF(&pyhermes_SecureHermesTransportType);
    PyModule_AddObject(m, "SecureHermesTransport", (PyObject*) &pyhermes_SecureHermesTransportType);

    Py_INCREF(&pyhermes_HermesTransportType);
    PyModule_AddObject(m, "HermesTransport", (PyObject*) &pyhermes_HermesTransportType);

    return m;
}

int main(int argc, char *argv[]) {
    PyImport_AppendInittab("pyhermes", PyInit_pyhermes);
    //  Py_SetProgramName(argv[0]);
    Py_Initialize();
    PyImport_ImportModule("pyhermes");
    return 0;
}


