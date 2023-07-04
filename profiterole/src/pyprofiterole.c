#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <mqueue.h>
#include "profiterole.h"
#include "pyprofiterole.h"
#include "roulettemacs.h"

DEFINE_TypeAlias(ProfilerAddrType, ProfilerAddrName);
DEFINE_ArrayAlias(SentinelBufferType, SentinelBufferName, SentinelBufferSize);

DEFINE_MemStruct(STRUCTNAME_ProfilerObj,
                 PyObject_HEAD DEFINE_PyObject(ATTRNAME_ProfileAddress);
                 DEFINE_PyObject(ATTRNAME_Name);
                 DEFINE_PyObject(ATTRNAME_OutPath);
                 DEFINE_PyObject(ATTRNAME_Sentinel););

DEFINE_FuncProto(FUNCYIELD_NewFunc, FUNCNAME_NewFunc, FUNCPARAMS_NewFunc) {
  ProfilerAddrName ATTRNAME_ProfileAddress;
  ExtensionObjRef PySelfObject;
  ATTRNAME_ProfileAddress = PyMem_RawCalloc(1, sizeof(ProfilerLength));
  PySelfObject = (ExtensionObjRef)type->tp_alloc(type, 0);
  PySelfObject->ATTRNAME_ProfileAddress =
      PyLong_FromVoidPtr(ATTRNAME_ProfileAddress);
  Py_DECREF(PySelfObject->ATTRNAME_ProfileAddress);
  return SelfObjectCasted;
}

DEFINE_FuncProto(FUNCYIELD_InitProc, FUNCNAME_InitProc, FUNCPARAMS_InitProc) {
  DEFINE_NativeType(SentinelBufferName, ATTRNAME_Sentinel);
  DEFINE_Exception(ExceptionObject,
                   DEFINE_Namespace(ExtensionObj, ExceptionObject));
  KWLIST_Create(TOKEN_AsciiStr(ATTRNAME_Name),
                TOKEN_AsciiStr(ATTRNAME_OutPath));
  DEFINE_SerialVar(StrArgumentType, *ATTRNAME_Name = NULL,
                   *ATTRNAME_OutPath = NULL);
  PARSE_FnArgs("ss", ATTRNAME_Name, ATTRNAME_OutPath);
  Py_XINCREF(PySelfObject->ATTRNAME_ProfileAddress);
  ProfilerAddrName ATTRNAME_ProfileAddress =
      PyLong_AsVoidPtr(PySelfObject->ATTRNAME_ProfileAddress);
  if ((init_profiler((ProfilerReference)ATTRNAME_ProfileAddress, ATTRNAME_Name,
                     ATTRNAME_OutPath, BUFFER_ReferArray(ATTRNAME_Sentinel))) <
      0)
    PyErr_SetFromErrno(ExceptionObject);
  Py_XDECREF(PySelfObject->ATTRNAME_ProfileAddress);
  PySelfObject->ATTRNAME_Sentinel =
      PyUnicode_FromString(BUFFER_ReferArray(ATTRNAME_Sentinel));
  PySelfObject->ATTRNAME_Name =
      PyUnicode_FromString(BUFFER_ReferArray(ATTRNAME_Name));
  PySelfObject->ATTRNAME_OutPath =
      PyUnicode_FromString(BUFFER_ReferArray(ATTRNAME_OutPath));
  return RETURN_SUCCESS;
}

DEFINE_FuncProto(FUNCYIELD_TernaryFunc, FUNCNAME_CallTernary,
                 FUNCPARAMS_TernaryFunc) {
  DEFINE_NativeType(ProfilerAddrName, ATTRNAME_ProfileAddress);
  DEFINE_Exception(ExceptionObject,
                   DEFINE_Namespace(ExtensionObj, ExceptionObject));
  KWLIST_Create(TOKEN_AsciiStr(BreakMarker));
  DEFINE_NativePtr(StrArgumentType, BreakMarker);
  PARSE_FnArgs("s", BreakMarker = NULL);
  Py_XINCREF(PySelfObject->ATTRNAME_ProfileAddress);
  ATTRNAME_ProfileAddress =
      PyLong_AsVoidPtr(PySelfObject->ATTRNAME_ProfileAddress);
  if (!(queue_message_to_profiler((ProfilerReference)ATTRNAME_ProfileAddress,
                                  BreakMarker)))
    PyErr_SetFromErrno(ExceptionObject);
  Py_XDECREF(PySelfObject->ATTRNAME_ProfileAddress);
  Py_RETURN_NONE;
}

DEFINE_FuncProto(FUNCYIELD_Destructor, FUNCNAME_Destructor,
                 FUNCPARAMS_Destructor) {
  REFLIST_Create(PyObjIdent, deallocrefs, PySelfObject->ATTRNAME_ProfileAddress,
                 PySelfObject->ATTRNAME_Sentinel,
                 PySelfObject->ATTRNAME_OutPath, PySelfObject->ATTRNAME_Name);
  ProfilerAddrName ATTRNAME_ProfileAddress =
      PyLong_AsVoidPtr(PySelfObject->ATTRNAME_ProfileAddress);
  PyMem_RawFree(ATTRNAME_ProfileAddress);
  REFLIST_ApplyXrs(deallocrefs, Py_XDECREF);
  Py_TYPE(PySelfObject)->tp_free(SelfObjectCasted);
}

DEFINE_StaticObj(
    PyTypeObject, OBJNAME_MainType,
    PyVarObject_HEAD_INIT(NULL, 0).tp_name =
        DEFINE_Namespace(OBJNAME_MainModule, OBJNAME_MainType),
    .tp_doc = PyDoc_STR(MainClassDoc), .tp_basicsize = sizeof(ExtensionObj),
    .tp_itemsize = 0, .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = DEFINE_CastFunc(FNCAST_NewFunc, FUNCNAME_NewFunc),
    .tp_init = DEFINE_CastFunc(FNCAST_InitProc, FUNCNAME_InitProc),
    .tp_call = DEFINE_CastFunc(FNCAST_TernaryFunc, FUNCNAME_CallTernary),
    .tp_dealloc = DEFINE_CastFunc(FNCAST_Destructor, FUNCNAME_Destructor));

DEFINE_StaticObj(PyModuleDef, OBJNAME_MainModule, PyModuleDef_HEAD_INIT,
                 .m_name = TOKEN_AsciiStr(OBJNAME_MainModule),
                 .m_doc = PyDoc_STR(MainModuleDoc), .m_size = -1);

PyMODINIT_FUNC PyInit_PyProfiterole(SIGNIFY_None) {
  DEFINE_PyObject(module);
  if ((PyType_Ready(&OBJNAME_MainType)) < 0)
    return NULL;

  if (!(module = PyModule_Create(&OBJNAME_MainModule)))
    return NULL;

  Py_INCREF(&OBJNAME_MainType);

  if ((PyModule_AddObject(module, TOKEN_AsciiStr(OBJNAME_MainModule),
                          (PyObjRef)&OBJNAME_MainModule)) < 0) {
    Py_DECREF(&OBJNAME_MainType);
    Py_DECREF(module);
    return NULL;
  }

  return module;
}