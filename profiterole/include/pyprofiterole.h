#ifndef PROFITEROLE_H
#define PROFITEROLE_H

#define PyObjIdent PyObject
#define PyObjRef PyObjIdent *
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
#define BreakMarker marker
#define StrArgumentType const SIGNED_Byte
#define ExceptionObject ProfilerException
#define SuffixArray array
#define SuffixPtr ptr
#define SuffixTemp tmp
#define MainClassDoc                                                           \
  "Profiler for PyProfiterole, please see github/Chubek/Profiterole for more " \
  "info"
#define MainModuleDoc "PyProfiterole is an overhead-free profiler for Python"

#define STRUCTNAME_ProfilerObj ExtensionObj

#define ATTRNAME_ProfileAddress profiler_address
#define ATTRNAME_OutPath outpath
#define ATTRNAME_Name name
#define ATTRNAME_Sentinel sentinel

#define OBJNAME_MainType Profiler
#define OBJNAME_MainModule PyProfiterole

#define FUNCNAME_NewFunc create
#define FUNCNAME_InitProc initialize
#define FUNCNAME_CallTernary callproto
#define FUNCNAME_Destructor freeobjmem

#define FNCAST_NewFunc newfunc
#define FNCAST_InitProc initproc
#define FNCAST_TernaryFunc ternaryfunc
#define FNCAST_Destructor destructor

#define FUNCPARAMS_NewFunc                                                     \
  DEFINE_PyType(type), DEFINE_PyObject(args), DEFINE_PyObject(kwargs)
#define FUNCPARAMS_InitProc                                                    \
  DEFINE_NativePtr(ExtensionObj, PySelfObject), DEFINE_PyObject(args),         \
      DEFINE_PyObject(kwargs)
#define FUNCPARAMS_TernaryFunc                                                 \
  DEFINE_NativePtr(ExtensionObj, PySelfObject), DEFINE_PyObject(args),         \
      DEFINE_PyObject(kwargs)
#define FUNCPARAMS_Destructor DEFINE_NativePtr(ExtensionObj, PySelfObject)

#define FUNCYIELD_NewFunc DEFINE_PyObject(NIL)
#define FUNCYIELD_InitProc DEFINE_NativeType(SIGNED_Double, NIL)
#define FUNCYIELD_TernaryFunc DEFINE_PyObject(NIL)
#define FUNCYIELD_Destructor DEFINE_NativeType(SIGNIFY_None, NIL)

#endif