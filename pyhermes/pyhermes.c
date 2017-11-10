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

#if PY_MAJOR_VERSION >= 3

static PyModuleDef pyhermesmodule = {
        PyModuleDef_HEAD_INIT,
        "pyhermes",
        "PyHermes",
        -1,
        NULL, NULL, NULL, NULL, NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC PyInit_pyhermes(void)
#else
#define INITERROR return

void initpyhermes(void)
#endif
{
    if (PyType_Ready(&pyhermes_MidHermesType) < 0){
        INITERROR;
    }

    if (PyType_Ready(&pyhermes_SecureHermesTransportType) < 0){
        INITERROR;
    }

    if (PyType_Ready(&pyhermes_HermesTransportType) < 0){
        INITERROR;
    }

    PyObject *m;
#if PY_MAJOR_VERSION >= 3
    m = PyModule_Create(&pyhermesmodule);
#else
    m =  Py_InitModule("pyhermes", NULL);
#endif
    if (m == NULL)
        INITERROR;

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

#if PY_MAJOR_VERSION >= 3
    return m;
#endif
}

int main(int argc, char *argv[]) {
#if PY_MAJOR_VERSION >= 3
    PyImport_AppendInittab("pyhermes", PyInit_pyhermes);
#endif
    //  Py_SetProgramName(argv[0]);
    Py_Initialize();
    PyImport_ImportModule("pyhermes");
    return 0;
}


