#define PY_SSIZE_T_CLEAN
#include "profiler.h"
#include <python3.10/Python.h>
#include <stdlib.h>

#ifndef DOC_TYPE
#define DOC_TYPE "Profiler for PyProfiterole"
#endif

#ifndef DOC_MODL
#define DOC_MODL "PyProfiterole is an overhead-free profiler for Python"
#endif

#define QWORD_u unsigned long
#define DWORD_u unsigned int
#define WORD_u unsigned short
#define BYTE_u unsigned char
#define QWORD_s long
#define DWORD_s int
#define WORD_s short
#define BYTE_s char
#define MEMADDR void *
#define CCNONE void

#define NIL

#define PYOREF PyObject *
#define EXTOBJ CCProfiterole
#define EXTPTR EXTOBJ *
#define PRFPTR MemoryAddress
#define PRFLEN sizeof(profiler_t)
#define PRFREF profiler_t *
#define PYSELF self
#define CASTPY (PyObject *)PYSELF
#define SNTLTY Sentinel_Type
#define SNTLAR Sentinel_Array
#define SNTLLN 8
#define MARKER marker
#define STRARG const char
#define XPTOBJ ProfilerException

#define STRUCTNAME_CCPROFILER EXTOBJ

#define FUNCNAME_NEWFN create
#define FUNCNAME_INITFN initialize
#define FUNCNAME_CALLFN callproto
#define FUNCNAME_DEALLOCFN freeobjmem
#define FUNCNAME_GETATTRO getattributes

#define ATTRNAME_PROFADDR profaddr
#define ATTRNAME_OUTPATH outpath
#define ATTRNAME_NAME name
#define ATTRNAME_SENTINEL sentinel

#define OBJNAME_MAINTYPE Profiler
#define OBJNAME_MAINMODULE PyProfiterole

#define CAST_NEW newfunc
#define CAST_INIT initproc
#define CAST_TERNARY ternaryfunc
#define CAST_DESTROY destructor

#define STR__(TOKEN) #TOKEN
#define PASTE__(TOKEN1, TOKEN2, TOKEN3) TOKEN1##TOKEN2##TOKEN3
#define STR(TOKEN) STR__(TOKEN)
#define PASTE(TOKEN1, TOKEN2, TOKEN3) PASTE__(TOKEN1, TOKEN2, TOKEN3)
#define CONCATNS__(TOKEN1, TOKEN2) STR__(TOKEN1.TOKEN2)
#define CONCATNS(TOKEN1, TOKEN2) CONCATNS__(TOKEN1, TOKEN2)

#define PYTYY_DEF(NAME) PyTypeObject *NAME
#define PYOBJ_DEF(NAME) PyObject *NAME
#define CCTYY_DEF(TYY, NAME) TYY NAME
#define CCPTR_DEF(TYY, NAME) TYY *NAME

#define GET_FUNCNAME__(PREFIX, FUNCNAME) PASTE(PREFIX, _, FUNCNAME)
#define GET_FUNCNAME(FUNCNAME) GET_FUNCNAME__(EXTOBJ, FUNCNAME)
#define GET_FUNCNAME_CASTED(CAST, FUNCNAME) (CAST) GET_FUNCNAME(FUNCNAME)

#define DECL_MEMSTRUCT(STRUCTNAME, ...)                                        \
  typedef struct {                                                             \
    __VA_ARGS__                                                                \
  } STRUCTNAME

#define DECL_FUNCTION(RETTYPE, FUNCNAME, ...)                                  \
  static RETTYPE GET_FUNCNAME(FUNCNAME)(__VA_ARGS__)

#define FUNCARGS_NEWSIG PYTYY_DEF(type), PYOBJ_DEF(args), PYOBJ_DEF(kwargs)
#define FUNCARGS_INITSIG                                                       \
  CCPTR_DEF(EXTOBJ, PYSELF), PYOBJ_DEF(args), PYOBJ_DEF(kwargs)
#define FUNCARGS_TERNARYSIG                                                    \
  CCPTR_DEF(EXTOBJ, PYSELF), PYOBJ_DEF(args), PYOBJ_DEF(kwargs)
#define FUNCARGS_DESTROYSIG CCPTR_DEF(EXTOBJ, PYSELF)

#define FUNCRET_NEWSIG PYOBJ_DEF(NIL)
#define FUNCRET_INITSIG CCTYY_DEF(DWORD_s, NIL)
#define FUNCRET_TERNARYSIG PYOBJ_DEF(NIL)
#define FUNCRET_DESTROYSIG CCTYY_DEF(CCNONE, NIL)

#define DECL_STATICOBJ(OBJTYY, OBJNAME, ...)                                   \
  static OBJTYY OBJNAME = {__VA_ARGS__}

#define KWARGS_NAMES(...) static char *kwargslist[] = {__VA_ARGS__, NULL}

#define PARSE_ARGS(FMT, ...)                                                   \
  if (!(PyArg_ParseTupleAndKeywords(args, kwargs, FMT, &kwargslist[0],         \
                                    __VA_ARGS__)))                             \
  PyErr_BadArgument()

#define NEW_EXCEPTION(NAME, NS) PYOREF NAME = PyErr_NewException(NS, NULL, NULL)

#define CREATE_REFARRAY(NAME, ...)                                             \
  PyObject *NAME##arr[] =                                                      \
      {                                                                        \
          __VA_ARGS__,                                                         \
          NULL,                                                                \
  },                                                                           \
           **NAME##ptr = &NAME##arr[0], *NAME
#define INCREF_REFARRAY(NAME)                                                  \
  while ((NAME = *NAME##ptr++)) {                                              \
    Py_XINCREF(NAME);                                                          \
  }
#define DECREF_REFARRAY(NAME)                                                  \
  while ((NAME = *NAME##ptr++)) {                                              \
    Py_XDECREF(NAME);                                                          \
  }

typedef MEMADDR PRFPTR;
typedef QWORD SNTLTY;
typedef const char SNTLAR[SNTLLN];

DECL_MEMSTRUCT(STRUCTNAME_CCPROFILER,
               PyObject_HEAD PYOBJ_DEF(ATTRNAME_PROFADDR);
               PYOBJ_DEF(ATTRNAME_NAME); PYOBJ_DEF(ATTRNAME_OUTPATH);
               PYOBJ_DEF(ATTRNAME_SENTINEL););

DECL_FUNCTION(FUNCRET_NEWSIG, FUNCNAME_NEWFN, FUNCARGS_NEWSIG) {
  PRFPTR ATTRNAME_PROFADDR;
  EXTPTR PYSELF;
  ATTRNAME_PROFADDR = PyMem_RawCalloc(1, sizeof(PRFLEN));
  PYSELF = (EXTPTR)type->tp_alloc(type, 0);
  PYSELF->ATTRNAME_PROFADDR = PyLong_FromVoidPtr(ATTRNAME_PROFADDR);
  Py_DECREF(PYSELF->ATTRNAME_PROFADDR);
  return CASTPY;
}

DECL_FUNCTION(FUNCRET_INITSIG, FUNCNAME_INITFN, FUNCARGS_INITSIG) {
  CCTYY_DEF(SNTLTY, ATTRNAME_SENTINEL);
  CCTYY_DEF(SNTLAR, sentinelbytes);
  NEW_EXCEPTION(profxcpt, CONCATNS(EXTOBJ, XPTOBJ));
  KWARGS_NAMES(STR(ATTRNAME_NAME), STR(ATTRNAME_OUTPATH));
  STRARG *ATTRNAME_NAME = NULL, *ATTRNAME_OUTPATH = NULL;
  PARSE_ARGS("ss", ATTRNAME_NAME, ATTRNAME_OUTPATH);
  Py_XINCREF(PYSELF->ATTRNAME_PROFADDR);
  PRFPTR ATTRNAME_PROFADDR = PyLong_AsVoidPtr(PYSELF->ATTRNAME_PROFADDR);
  if ((init_profiler((PRFREF)ATTRNAME_PROFADDR, ATTRNAME_NAME, ATTRNAME_OUTPATH,
                     &sentinelbytes[0])) < 0)
    PyErr_SetFromErrno(profxcpt);
  Py_XDECREF(PYSELF->ATTRNAME_PROFADDR);
  PYSELF->ATTRNAME_SENTINEL = PyLong_FromUnsignedLong(ATTRNAME_SENTINEL);
  PYSELF->ATTRNAME_NAME = PyUnicode_FromString(ATTRNAME_NAME);
  PYSELF->ATTRNAME_OUTPATH = PyUnicode_FromString(ATTRNAME_OUTPATH);
  return RETURN_SUCCESS;
}

DECL_FUNCTION(FUNCRET_TERNARYSIG, FUNCNAME_CALLFN, FUNCARGS_TERNARYSIG) {
  CCTYY_DEF(PRFPTR, ATTRNAME_PROFADDR);
  NEW_EXCEPTION(profxcpt, CONCATNS(EXTOBJ, XPTOBJ));
  KWARGS_NAMES(STR(MARKER));
  STRARG *MARKER = NULL;
  PARSE_ARGS("s", MARKER);
  Py_XINCREF(PYSELF->ATTRNAME_PROFADDR);
  ATTRNAME_PROFADDR = PyLong_AsVoidPtr(PYSELF->ATTRNAME_PROFADDR);
  if (!(queue_message_to_profiler((PRFREF)ATTRNAME_PROFADDR, MARKER)))
    PyErr_SetFromErrno(profxcpt);
  Py_XDECREF(PYSELF->ATTRNAME_PROFADDR);
  Py_RETURN_NONE;
}

DECL_FUNCTION(CCNONE, FUNCNAME_DEALLOCFN, FUNCARGS_DESTROYSIG) {
  CREATE_REFARRAY(deallocrefs, PYSELF->ATTRNAME_PROFADDR,
                  PYSELF->ATTRNAME_SENTINEL, PYSELF->ATTRNAME_OUTPATH,
                  PYSELF->ATTRNAME_NAME);
  PRFPTR ATTRNAME_PROFADDR = PyLong_AsVoidPtr(PYSELF->ATTRNAME_PROFADDR);
  PyMem_RawFree(ATTRNAME_PROFADDR);
  DECREF_REFARRAY(deallocrefs);
  Py_TYPE(PYSELF)->tp_free(CASTPY);
}

DECL_STATICOBJ(
    PyTypeObject, OBJNAME_MAINTYPE,
    PyVarObject_HEAD_INIT(NULL, 0).tp_name = CONCATNS(OBJNAME_MAINMODULE,
                                                      OBJNAME_MAINTYPE),
    .tp_doc = PyDoc_STR(DOC_TYPE), .tp_basicsize = sizeof(EXTOBJ),
    .tp_itemsize = 0, .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = GET_FUNCNAME_CASTED(CAST_NEW, FUNCNAME_NEWFN),
    .tp_init = GET_FUNCNAME_CASTED(CAST_INIT, FUNCNAME_INITFN),
    .tp_call = GET_FUNCNAME_CASTED(CAST_TERNARY, FUNCNAME_CALLFN),
    .tp_dealloc = GET_FUNCNAME_CASTED(CAST_DESTROY, FUNCNAME_DEALLOCFN));

DECL_STATICOBJ(PyModuleDef, OBJNAME_MAINMODULE, PyModuleDef_HEAD_INIT,
               .m_name = STR(OBJNAME_MAINMODULE), .m_doc = PyDoc_STR(DOC_MODL),
               .m_size = -1);

PyMODINIT_FUNC PyInit_profiterole(CCNONE) {
  PYOBJ_DEF(module);
  if ((PyType_Ready(&OBJNAME_MAINTYPE)) < 0)
    return NULL;

  if (!(module = PyModule_Create(&OBJNAME_MAINMODULE)))
    return NULL;

  Py_INCREF(&OBJNAME_MAINTYPE);

  if ((PyModule_AddObject(module, STR(OBJNAME_MAINMODULE),
                          (PYOREF)&OBJNAME_MAINMODULE)) < 0) {
    Py_DECREF(&OBJNAME_MAINTYPE);
    Py_DECREF(module);
    return NULL;
  }

  return module;
}