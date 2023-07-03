#define PY_SSIZE_T_CLEAN
#include "profiler.h"
#include <python3.10/Python.h>
#include <stdlib.h>

#define UNSIGNED_QUAD unsigned long
#define UNSIGNED_DOUBLE unsigned int
#define UNSIGNED_HALF unsigned short
#define UNSIGNED_BYTE unsigned char
#define SIGNED_Quad long
#define SIGNED_Double int
#define SIGNED_Half short
#define SIGNED_Byte char
#define MEMORY_Address void *
#define YIELD_None void
#define NIL

#define PyObjRef PyObject *
#define ExtensionObj CCProfiterole
#define ExtensionObjRef ExtensionObj *
#define ProfilerMemoryAddr ProfMemoryAddr
#define ProfilerLength sizeof(profiler_t)
#define ProfilerReference profiler_t *
#define PySelfObject self
#define SelfObjectCasted (PyObject *)PySelfObject
#define SentinelStrArrType Sentinel_Type
#define SentinelNumChars 8
#define MarkerObj marker
#define StringArgument const char
#define ExceptionObject ProfilerException
#define MainClassDoc "Profiler for PyProfiterole"
#define MainModuleDoc "PyProfiterole is an overhead-free profiler for Python"


#define STRUCTNAME_ProfilerObj ExtensionObj

#define FUNCNAME_NewFunc create
#define FUNCNAME_InitProc initialize
#define FUNCNAME_CallTernary callproto
#define FUNCNAME_Destructor freeobjmem

#define FNCAST_NewFunc newfunc
#define FNCAST_InitProc initproc
#define FNCAST_TernaryFunc ternaryfunc
#define FNCAST_Destructor destructor

#define ATTRNAME_ProfileAddress profiler_address
#define ATTRNAME_OutPath outpath
#define ATTRNAME_Name name
#define ATTRNAME_Sentinel sentinel

#define OBJNAME_MainType Profiler
#define OBJNAME_MainModule PyProfiterole

#define __STR(TOKEN) #TOKEN
#define __PASTE(TOKEN1, TOKEN2, TOKEN3) TOKEN1##TOKEN2##TOKEN3
#define __CONCAT(TOKEN1, TOKEN2) TOKEN1.TOKEN2
#define __EVAL(MAC, ...) MAC(__VA_ARGS__)

#define TOKEN_string(TOKEN) __EVAL(__STR, TOKEN)
#define TOKEN_paste(TOKEN1, TOKEN2, TOKEN3) __EVAL(__PASTE, TOKEN1, TOKEN2, TOKEN3)
#define TOKEN_concat(TOKEN1, TOKEN2) __EVAL(__CONCAT, TOKEN1, TOKEN2)

#define DEFINE_PyType(NAME) PyTypeObject *NAME
#define DEFINE_PyObject(NAME) PyObject *NAME
#define DEFINE_NativeType(TYY, NAME) TYY NAME
#define DEFINE_NativePtr(TYY, NAME) TYY *NAME

#define DEFINE_FuncName(FUNCNAME) TOKEN_paste(ExtensionObj, FUNCNAME)
#define DEFINE_CastFunc(CAST, FUNCNAME) (CAST) DEFINE_FuncName(FUNCNAME)

#define DEFINE_TypeAlias(MAIN, ALIAS) typedef MAIN ALIAS
#define DEFINE_ArrayAlias(MAIN, ALIAS, SIZE) typedef MAIN ALIAS[SIZE]

#define DEFINE_MemStruct(STRUCTNAME, ...)                                        \
  typedef struct {                                                             \
    __VA_ARGS__                                                                \
  } STRUCTNAME

#define DEFINE_FuncProto(RETTYPE, FUNCNAME, ...)                                  \
  static RETTYPE DEFINE_FuncName(FUNCNAME)(__VA_ARGS__)

#define FUNCPARAMS_NewFunc DEFINE_PyType(type), DEFINE_PyObject(args), DEFINE_PyObject(kwargs)
#define FUNCPARAMS_InitProc                                                       \
  DEFINE_NativePtr(ExtensionObj, PySelfObject), DEFINE_PyObject(args), DEFINE_PyObject(kwargs)
#define FUNCPARAMS_TernaryFunc                                                    \
  DEFINE_NativePtr(ExtensionObj, PySelfObject), DEFINE_PyObject(args), DEFINE_PyObject(kwargs)
#define FUNCPARAMS_Destructor DEFINE_NativePtr(ExtensionObj, PySelfObject)

#define FUNCYIELD_NewFunc DEFINE_PyObject(NIL)
#define FUNCYIELD_InitProc DEFINE_NativeType(SIGNED_Double, NIL)
#define FUNCYIELD_TernaryFunc DEFINE_PyObject(NIL)
#define FUNCYIELD_Destructor DEFINE_NativeType(YIELD_None, NIL)

#define DEFINE_StaticObj(OBJTYY, OBJNAME, ...)                                   \
  static OBJTYY OBJNAME = {__VA_ARGS__}

#define DEFINE_KWLIST(...) static char *kwargslist[] = {__VA_ARGS__, NULL}

#define PARSE_FnArgs(FMT, ...)                                                   \
  if (!(PyArg_ParseTupleAndKeywords(args, kwargs, FMT, &kwargslist[0],         \
                                    __VA_ARGS__)))                             \
  PyErr_BadArgument()

#define DEFINE_Exception(NAME, NS) PyObjRef NAME = PyErr_NewException(NS, NULL, NULL)

#define REFARRAY_Create(NAME, ...)   \
  PyObject *TOKEN_paste(NAME, array)[] =                                                      \
      {                                                                        \
          __VA_ARGS__,                                                         \
          NULL,                                                                \
  },                                                                           \
           **TOKEN_paste(NAME, ptr) = &NAME_array[0], *TOKEN_paste(NAME, tmp)

#define REFARRAY_Apply(NAME, APPLYFN)                                                  \
  while ((TOKEN_paste(NAME, tmp) = *TOKEN_paste(NAME, ptr)++)) {                                              \
    APPLYFN(NAME);                                                          \
  }

DEFINE_TypeAlias(MEMORY_Address, ProfilerMemoryAddr);
DEFINE_ArryAlias(SIGNED_Byte, SentinelStrArrType, SentinelNumChars);

DEFINE_MemStruct(STRUCTNAME_ProfilerObj,
               PyObject_HEAD DEFINE_PyObject(ATTRNAME_ProfileAddress);
               DEFINE_PyObject(ATTRNAME_Name); DEFINE_PyObject(ATTRNAME_OutPath);
               DEFINE_PyObject(ATTRNAME_Sentinel););

DEFINE_FuncProto(FUNCYIELD_NewFunc, FUNCNAME_NewFunc, FUNCPARAMS_NewFunc) {
  ProfilerMemoryAddr ATTRNAME_ProfileAddress;
  ExtensionObjRef PySelfObject;
  ATTRNAME_ProfileAddress = PyMem_RawCalloc(1, sizeof(ProfilerLength));
  PySelfObject = (ExtensionObjRef)type->tp_alloc(type, 0);
  PySelfObject->ATTRNAME_ProfileAddress = PyLong_FromVoidPtr(ATTRNAME_ProfileAddress);
  Py_DECREF(PySelfObject->ATTRNAME_ProfileAddress);
  return SelfObjectCasted;
}

DEFINE_FuncProto(FUNCYIELD_InitProc, FUNCNAME_InitProc, FUNCPARAMS_InitProc) {
  DEFINE_NativeType(SentinelStrArrType, ATTRNAME_Sentinel);
  DEFINE_Exception(profxcpt, TOKEN_concat(ExtensionObj, ExceptionObject));
  DEFINE_KWLIST(TOKEN_string(ATTRNAME_Name), TOKEN_string(ATTRNAME_OutPath));
  StringArgument *ATTRNAME_Name = NULL, *ATTRNAME_OutPath = NULL;
  PARSE_FnArgs("ss", ATTRNAME_Name, ATTRNAME_OutPath);
  Py_XINCREF(PySelfObject->ATTRNAME_ProfileAddress);
  ProfilerMemoryAddr ATTRNAME_ProfileAddress = PyLong_AsVoidPtr(PySelfObject->ATTRNAME_ProfileAddress);
  if ((init_profiler((ProfilerReference)ATTRNAME_ProfileAddress, ATTRNAME_Name, ATTRNAME_OutPath,
                     &ATTRNAME_Sentinel[0])) < 0)
    PyErr_SetFromErrno(profxcpt);
  Py_XDECREF(PySelfObject->ATTRNAME_ProfileAddress);
  PySelfObject->ATTRNAME_Sentinel = PyLong_FromUNSIGNEDLong(ATTRNAME_Sentinel);
  PySelfObject->ATTRNAME_Name = PyUnicode_FromString(ATTRNAME_Name);
  PySelfObject->ATTRNAME_OutPath = PyUnicode_FromString(ATTRNAME_OutPath);
  return RETURN_SUCCESS;
}

DEFINE_FuncProto(FUNCYIELD_TernaryFunc, FUNCNAME_CallTernary, FUNCPARAMS_TernaryFunc) {
  DEFINE_NativeType(ProfilerMemoryAddr, ATTRNAME_ProfileAddress);
  DEFINE_Exception(profxcpt, TOKEN_concat(ExtensionObj, ExceptionObject));
  DEFINE_KWLIST(TOKEN_string(MarkerObj));
  StringArgument *MarkerObj = NULL;
  PARSE_FnArgs("s", MarkerObj);
  Py_XINCREF(PySelfObject->ATTRNAME_ProfileAddress);
  ATTRNAME_ProfileAddress = PyLong_AsVoidPtr(PySelfObject->ATTRNAME_ProfileAddress);
  if (!(queue_message_to_profiler((ProfilerReference)ATTRNAME_ProfileAddress, MarkerObj)))
    PyErr_SetFromErrno(profxcpt);
  Py_XDECREF(PySelfObject->ATTRNAME_ProfileAddress);
  Py_RETURN_NONE;
}

DEFINE_FuncProto(YIELD_None, FUNCNAME_Destructor, FUNCPARAMS_Destructor) {
  REFARRAY_Create(deallocrefs, PySelfObject->ATTRNAME_ProfileAddress,
                  PySelfObject->ATTRNAME_Sentinel, PySelfObject->ATTRNAME_OutPath,
                  PySelfObject->ATTRNAME_Name);
  ProfilerMemoryAddr ATTRNAME_ProfileAddress = PyLong_AsVoidPtr(PySelfObject->ATTRNAME_ProfileAddress);
  PyMem_RawFree(ATTRNAME_ProfileAddress);
  REFARRAY_Apply(deallocrefs, Py_XDECREF);
  Py_TYPE(PySelfObject)->tp_free(SelfObjectCasted);
}

DEFINE_StaticObj(
    PyTypeObject, OBJNAME_MainType,
    PyVarObject_HEAD_INIT(NULL, 0).tp_name = TOKEN_concat(OBJNAME_MainModule,
                                                      OBJNAME_MainType),
    .tp_doc = PyDoc_TOKEN_string(MainClassDoc), .tp_basicsize = sizeof(ExtensionObj),
    .tp_itemsize = 0, .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = DEFINE_CastFunc(FNCAST_NewFunc, FUNCNAME_NewFunc),
    .tp_init = DEFINE_CastFunc(FNCAST_InitProc, FUNCNAME_InitProc),
    .tp_call = DEFINE_CastFunc(FNCAST_TernaryFunc, FUNCNAME_CallTernary),
    .tp_dealloc = DEFINE_CastFunc(FNCAST_Destructor, FUNCNAME_Destructor));

DEFINE_StaticObj(PyModuleDef, OBJNAME_MainModule, PyModuleDef_HEAD_INIT,
               .m_name = TOKEN_string(OBJNAME_MainModule), .m_doc = PyDoc_TOKEN_string(MainModuleDoc),
               .m_size = -1);

PyMODINIT_FUNC PyInit_profiterole(YIELD_None) {
  DEFINE_PyObject(module);
  if ((PyType_Ready(&OBJNAME_MainType)) < 0)
    return NULL;

  if (!(module = PyModule_Create(&OBJNAME_MainModule)))
    return NULL;

  Py_INCREF(&OBJNAME_MainType);

  if ((PyModule_AddObject(module, TOKEN_string(OBJNAME_MainModule),
                          (PyObjRef)&OBJNAME_MainModule)) < 0) {
    Py_DECREF(&OBJNAME_MainType);
    Py_DECREF(module);
    return NULL;
  }

  return module;
}