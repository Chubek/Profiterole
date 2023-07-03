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
#define SIGNIFY_None void
#define NIL

#define PyObjRef PyObject *
#define ExtensionObj CCProfiterole
#define ExtensionObjRef ExtensionObj *
#define ProfilerAddrName ProfMemoryAddr
#define ProfilerAddrType MEMORY_Address
#define ProfilerLength sizeof(profiler_t)
#define ProfilerReference profiler_t *
#define PySelfObject self
#define SelfObjectCasted (PyObject *)PySelfObject
#define SentinelBufferName Sentinel_Type
#define SentinelBufferSize 8
#define SentinelBufferType const SIGNED_Byte
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

#define __PRF_STR(TOKEN) #TOKEN
#define __PRF_PASTE(TOKEN1, TOKEN2, TOKEN3) TOKEN1##TOKEN2##TOKEN3
#define __PRF_CONCAT(TOKEN1, TOKEN2) TOKEN1.TOKEN2
#define __PRF_EVAL(MAC, ...) MAC(__VA_ARGS__)

#define TOKEN_String(TOKEN) __PRF_EVAL(__PRF_STR, TOKEN)
#define TOKEN_Paste(TOKEN1, TOKEN2, TOKEN3) __PRF_EVAL(__PRF_PASTE, TOKEN1, TOKEN2, TOKEN3)
#define TOKEN_Concat(TOKEN1, TOKEN2) __PRF_EVAL(__PRF_CONCAT, TOKEN1, TOKEN2)
#define TOKEN_Namespace(TOKEN1, TOKEN2) TOKEN_String(TOKEN_Concat(TOKEN1, TOKEN2))

#define DEFINE_PyType(NAME) PyTypeObject *NAME
#define DEFINE_PyObject(NAME) PyObject *NAME
#define DEFINE_NativeType(TYY, NAME) TYY NAME
#define DEFINE_NativePtr(TYY, NAME) TYY *NAME

#define DEFINE_FuncName(FUNCNAME) TOKEN_Paste(ExtensionObj, _, FUNCNAME)
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
#define FUNCYIELD_Destructor DEFINE_NativeType(SIGNIFY_None, NIL)

#define DEFINE_StaticObj(OBJTYY, OBJNAME, ...)                                   \
  static OBJTYY OBJNAME = {__VA_ARGS__}


#define PARSE_FnArgs(FMT, ...)                                                   \
  if (!(PyArg_ParseTupleAndKeywords(args, kwargs, FMT, &kwargslist[0],         \
                                    __VA_ARGS__)))                             \
  PyErr_BadArgument()

#define DEFINE_Exception(NAME, NS) PyObjRef NAME = PyErr_NewException(NS, NULL, NULL)

#define KWLIST_Create(...) static char *kwargslist[] = {__VA_ARGS__, NULL}

#define REFLIST_Create(NAME, ...)   \
  PyObject *TOKEN_Paste(NAME, _, array)[] =                                                      \
      {                                                                        \
          __VA_ARGS__,                                                         \
          NULL,                                                                \
  },                                                                           \
           **TOKEN_Paste(NAME, _, ptr) = &NAME_array[0], *TOKEN_Paste(NAME, _, tmp)

#define REFLIST_Apply(NAME, APPLYFN)                                                  \
  while ((TOKEN_Paste(NAME, _, tmp) = *TOKEN_Paste(NAME, _, ptr)++)) {                                              \
    APPLYFN(NAME);                                                          \
  }

DEFINE_TypeAlias(ProfilerAddrType, ProfilerAddrName);
DEFINE_ArrayAlias(SentinelBufferType, SentinelBufferName, SentinelBufferSize);

DEFINE_MemStruct(STRUCTNAME_ProfilerObj,
               PyObject_HEAD DEFINE_PyObject(ATTRNAME_ProfileAddress);
               DEFINE_PyObject(ATTRNAME_Name); DEFINE_PyObject(ATTRNAME_OutPath);
               DEFINE_PyObject(ATTRNAME_Sentinel););

DEFINE_FuncProto(FUNCYIELD_NewFunc, FUNCNAME_NewFunc, FUNCPARAMS_NewFunc) {
  ProfilerAddrName ATTRNAME_ProfileAddress;
  ExtensionObjRef PySelfObject;
  ATTRNAME_ProfileAddress = PyMem_RawCalloc(1, sizeof(ProfilerLength));
  PySelfObject = (ExtensionObjRef)type->tp_alloc(type, 0);
  PySelfObject->ATTRNAME_ProfileAddress = PyLong_FromVoidPtr(ATTRNAME_ProfileAddress);
  Py_DECREF(PySelfObject->ATTRNAME_ProfileAddress);
  return SelfObjectCasted;
}

DEFINE_FuncProto(FUNCYIELD_InitProc, FUNCNAME_InitProc, FUNCPARAMS_InitProc) {
  DEFINE_NativeType(SentinelBufferName, ATTRNAME_Sentinel);
  DEFINE_Exception(profxcpt, TOKEN_Namespace(ExtensionObj, ExceptionObject));
  KWLIST_Create(TOKEN_String(ATTRNAME_Name), TOKEN_String(ATTRNAME_OutPath));
  StringArgument *ATTRNAME_Name = NULL, *ATTRNAME_OutPath = NULL;
  PARSE_FnArgs("ss", ATTRNAME_Name, ATTRNAME_OutPath);
  Py_XINCREF(PySelfObject->ATTRNAME_ProfileAddress);
  ProfilerAddrName ATTRNAME_ProfileAddress = PyLong_AsVoidPtr(PySelfObject->ATTRNAME_ProfileAddress);
  if ((init_profiler((ProfilerReference)ATTRNAME_ProfileAddress, ATTRNAME_Name, ATTRNAME_OutPath,
                     &ATTRNAME_Sentinel[0])) < 0)
    PyErr_SetFromErrno(profxcpt);
  Py_XDECREF(PySelfObject->ATTRNAME_ProfileAddress);
  PySelfObject->ATTRNAME_Sentinel = PyUnicode_FromString(ATTRNAME_Sentinel);
  PySelfObject->ATTRNAME_Name = PyUnicode_FromString(ATTRNAME_Name);
  PySelfObject->ATTRNAME_OutPath = PyUnicode_FromString(ATTRNAME_OutPath);
  return RETURN_SUCCESS;
}

DEFINE_FuncProto(FUNCYIELD_TernaryFunc, FUNCNAME_CallTernary, FUNCPARAMS_TernaryFunc) {
  DEFINE_NativeType(ProfilerAddrName, ATTRNAME_ProfileAddress);
  DEFINE_Exception(profxcpt, TOKEN_Namespace(ExtensionObj, ExceptionObject));
  KWLIST_Create(TOKEN_String(MarkerObj));
  StringArgument *MarkerObj = NULL;
  PARSE_FnArgs("s", MarkerObj);
  Py_XINCREF(PySelfObject->ATTRNAME_ProfileAddress);
  ATTRNAME_ProfileAddress = PyLong_AsVoidPtr(PySelfObject->ATTRNAME_ProfileAddress);
  if (!(queue_message_to_profiler((ProfilerReference)ATTRNAME_ProfileAddress, MarkerObj)))
    PyErr_SetFromErrno(profxcpt);
  Py_XDECREF(PySelfObject->ATTRNAME_ProfileAddress);
  Py_RETURN_NONE;
}

DEFINE_FuncProto(SIGNIFY_None, FUNCNAME_Destructor, FUNCPARAMS_Destructor) {
  REFLIST_Create(deallocrefs, PySelfObject->ATTRNAME_ProfileAddress,
                  PySelfObject->ATTRNAME_Sentinel, PySelfObject->ATTRNAME_OutPath,
                  PySelfObject->ATTRNAME_Name);
  ProfilerAddrName ATTRNAME_ProfileAddress = PyLong_AsVoidPtr(PySelfObject->ATTRNAME_ProfileAddress);
  PyMem_RawFree(ATTRNAME_ProfileAddress);
  REFLIST_Apply(deallocrefs, Py_XDECREF);
  Py_TYPE(PySelfObject)->tp_free(SelfObjectCasted);
}

DEFINE_StaticObj(
    PyTypeObject, OBJNAME_MainType,
    PyVarObject_HEAD_INIT(NULL, 0).tp_name = TOKEN_Namespace(OBJNAME_MainModule,
                                                      OBJNAME_MainType),
    .tp_doc = PyDoc_TOKEN_String(MainClassDoc), .tp_basicsize = sizeof(ExtensionObj),
    .tp_itemsize = 0, .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = DEFINE_CastFunc(FNCAST_NewFunc, FUNCNAME_NewFunc),
    .tp_init = DEFINE_CastFunc(FNCAST_InitProc, FUNCNAME_InitProc),
    .tp_call = DEFINE_CastFunc(FNCAST_TernaryFunc, FUNCNAME_CallTernary),
    .tp_dealloc = DEFINE_CastFunc(FNCAST_Destructor, FUNCNAME_Destructor));

DEFINE_StaticObj(PyModuleDef, OBJNAME_MainModule, PyModuleDef_HEAD_INIT,
               .m_name = TOKEN_String(OBJNAME_MainModule), .m_doc = PyDoc_TOKEN_String(MainModuleDoc),
               .m_size = -1);

PyMODINIT_FUNC PyInit_profiterole(SIGNIFY_None) {
  DEFINE_PyObject(module);
  if ((PyType_Ready(&OBJNAME_MainType)) < 0)
    return NULL;

  if (!(module = PyModule_Create(&OBJNAME_MainModule)))
    return NULL;

  Py_INCREF(&OBJNAME_MainType);

  if ((PyModule_AddObject(module, TOKEN_String(OBJNAME_MainModule),
                          (PyObjRef)&OBJNAME_MainModule)) < 0) {
    Py_DECREF(&OBJNAME_MainType);
    Py_DECREF(module);
    return NULL;
  }

  return module;
}