#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdlib.h>
#include "profiler.h"

#define UQWORD unsigned long
#define UDWORD unsigned int
#define UWORD  unsigned short
#define UBYTE unsigned char
#define IQWORD long
#define IDWORD int
#define UWORD  short
#define IBYTE char
#define MEMADR void*
#define NORETURN void

#ifndef NAME_KWARG
#define NAME_KWARG "name"
#endif

#ifndef OUTPATH_KWARG
#define OUTPATH_KWARG "outpath"
#endif

#ifndef BREAKID_KWARG
#define BREAKID_KWARG "breakid"
#endif

#define STRARG const char

#define NIL

#define EXCEPT PyObject *
#define EXTOBJ ProfiteroleObj
#define PTROBJ ProfilerPtr_Type
#define PRFLEN sizeof(profiler_t);
#define PRFREF *profiler_t
#define XCPTNS "profiterole.ProfileException"

#define FUNCNAME_PREFIX Profiterole_
#define FUNCNAME_NEWFN create
#define FUNCNAME_INITFN initialize
#define FUNCNAME_CALLFN callproto
#define FUNCNAME_DEALLOCFN freeobjmem

#define PYTYY_DEF(NAME) PyTypeObject *NAME
#define PYOBJ_DEF(NAME) PyObject *NAME
#define CCTYY_DEF(TYY, NAME) TYY NAME
#define CCPTR_DEF(TYY, NAME) TYY *NAME

#define DECL_FUNCNAME(RETTYPE, FUNCNAME, FIRST_ARG) 
	static RETTYPE FUNCNAME_PREFIX ## FUNCNAME (FIRST_ARG, PYOBJ_DEF(args), PYOBJ_DEF(kwargs))

#define DECL_STATICOBJ(OBJTYY, OBJNAME)\
	static OBJTYY OBJNAME =

#define GET_FUNCNAME(FUNCNAME) FUNC_PREFX ## FUNCNAME

#define KWARGS_NAMES(...) \
	static char *kwargslist = {  __VA_ARGS__ , NULL  }

#define PARSE_ARGS(FMT, ...)	\
	if (!(PyArg_ParseTupleAndKeywords(args, kwargs, FMT, kwargslist, __VA_ARGS__)))	\
		return PyErr_BadArgument()

#define NEW_EXCEPTION(NAME, NS) \
	EXCEPT NAME = PyErr_NewException(NS, NULL, NULL)


typedef MEMADR PTROBJ;

typedef struct {
	PyObject_HEAD
	PYOBJ_DEF(profptr);
} EXTOBJ;


DECL_FUNCNAME(PYOBJ_DEF(NIL), FUNCNAME_NEWFN, PYTYY_DEF(type)) {
	PTROBJ profptr;
	EXTOBJ *selfobj;
	profptr = PyMem_RawCalloc(1, sizeof(PRFLEN));
	selfobj = (EXTOBJ *) type->tp_alloc(type, 0);
	selfobj->profptr = PyLong_FromVoidPtr(profptr);
	Py_DECREF(selfobj->profptr);
	return (PyObject *)selfobj;
}

DECL_FUNCNAME(CCTYY_DEF(IDWORD, NIL), FUNCNAME_INITFN, CCPTR_DEF(EXTOBJ, self)) {
	NEW_EXCEPTION(profxcpt, XCPTNS);
	KWARGS_NAMES(NAME_KWARG, OUTPATH_KWARG);
	STRARG *name = NULL, *outpath = NULL;
	PARSE_ARGS("ss", name, outpath);
	PTROBJ profptr = PyLong_AsVoidPtr(self->profptr);
	if ((init_profiler((PRFREF)profptr, name, outpath)) < 0)
		return PyErr_SetFromErrno(profxcpt);
	return RETURN_SUCCESS;
}

DECL_FUNCNAME(PYOBJ_DEF(NIL), FUNCNAME_CALLFN, CCPTR_DEF(EXTOBJ, self)) {
	NEW_EXCEPTION(profxcpt, XCPTNS);
	KWARGS_NAMES(BREAKID_KWARG);
	STRARG *breakid = NULL;
	PARSE_ARGS("s", breakid);
	PTROBJ profptr = PyLong_AsVoidPtr(self->profptr);
	if (!(queue_message_to_profiler((PROFCC_REFR)profptr, breakid)))
		return PyErr_SetFromErrno(profxcpt);
	return NULL;
}

DECL_FUNCNAME(NORETURN, FUNCNAME_DEALLOCFN, CCPTR_DEF(EXTOBJ, self)) {
	PTROBJ profptr = PyLong_AsVoidPtr(self->profptr);
	PyMem_RawFree(profptr);
	Py_TYPE(self)->tp_free(PYOBJ_DEF(NIL)self);
}

static PyTpeObject ProfiteroleProfiler = {
	PyObject_HEAD_INIT(NULL, 0)
};

