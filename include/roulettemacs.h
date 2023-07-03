#ifndef FRAISEMACS_H
#define FRAISEMACS_H

#define UNSIGNED_Quad unsigned long
#define UNSIGNED_Double unsigned int
#define UNSIGNED_Half unsigned short
#define UNSIGNED_Byte unsigned char
#define SIGNED_Quad long
#define SIGNED_Double int
#define SIGNED_Half short
#define SIGNED_Byte char
#define MEMORY_Address void*
#define SIGNIFY_None void
#define NIL

#define __PRF_STR(TOKEN) #TOKEN
#define __PRF_WSTR(TOKEN) L##TOKEN
#define __PRF_PASTE(TOKEN1, TOKEN2, TOKEN3) TOKEN1##TOKEN2##TOKEN3
#define __PRF_CONCAT(TOKEN1, TOKEN2) TOKEN1.TOKEN2
#define __PRF_EVAL(MAC, ...) MAC(__VA_ARGS__)

#define TOKEN_AsciiStr(TOKEN) __PRF_EVAL(__PRF_STR, TOKEN)
#define TOKEN_WideStr(TOKEN) __PRF_EVAL(__PRF_WSTR, TOKEN_AsciiStr(TOKEN))
#define TOKEN_ConcatPeriod(TOKEN1, TOKEN2) __PRF_EVAL(__PRF_CONCAT, TOKEN1, TOKEN2)
#define TOKEN_ConcatLowdash(TOKEN1, TOKEN2) __PRF_EVAL(__PRF_PASTE, TOKEN1, _, TOKEN2)


#define DEFINE_Namespace(PARENT, CHILD) TOKEN_AsciiStr(TOKEN_ConcatPeriod(PARENT, CHILD))
#define DEFINE_PyType(NAME) PyTypeObject *NAME
#define DEFINE_PyObject(NAME) PyObject *NAME
#define DEFINE_NativeType(TYY, NAME) TYY NAME
#define DEFINE_NativePtr(TYY, NAME) TYY *NAME
#define DEFINE_SerialVar(TYY, ...) TYY __VA_ARGS__

#define DEFINE_FuncName(FUNCNAME) TOKEN_ConcatLowdash(ExtensionObj, FUNCNAME)
#define DEFINE_CastFunc(CAST, FUNCNAME) (CAST) DEFINE_FuncName(FUNCNAME)

#define DEFINE_TypeAlias(MAIN, ALIAS) typedef MAIN ALIAS
#define DEFINE_ArrayAlias(MAIN, ALIAS, SIZE) typedef MAIN ALIAS[SIZE]

#define DEFINE_MemStruct(STRUCTNAME, ...)                                        \
  typedef struct {                                                             \
    __VA_ARGS__                                                                \
  } STRUCTNAME

#define DEFINE_FuncProto(RETTYPE, FUNCNAME, ...)                                  \
  static RETTYPE DEFINE_FuncName(FUNCNAME)(__VA_ARGS__)



#define DEFINE_StaticObj(OBJTYY, OBJNAME, ...)                                   \
  static OBJTYY OBJNAME = {__VA_ARGS__}


#define PARSE_FnArgs(FMT, ...)                                                   \
  if (!(PyArg_ParseTupleAndKeywords(args, kwargs, FMT, &kwargslist[0],         \
                                    __VA_ARGS__)))                             \
  PyErr_BadArgument()

#define DEFINE_Exception(NAME, NS) PyObjRef NAME = PyErr_NewException(NS, NULL, NULL)

#define KWLIST_Create(...) static char *kwargslist[] = {__VA_ARGS__, NULL}

#define REFLIST_Create(TYY, NAME, ...)   \
  TYY *TOKEN_ConcatLowdash(NAME, SuffixArray)[] =                                                      \
      {                                                                        \
          __VA_ARGS__,                                                         \
          NULL,                                                                \
  },                                                                           \
           **TOKEN_ConcatLowdash(NAME, SuffixPtr) = &TOKEN_ConcatLowdash(NAME, SuffixArray)[0], *TOKEN_ConcatLowdash(NAME, SuffixTemp)

#define REFLIST_ApplyXrs(NAME, APPLYFN)                                                  \
  do { TOKEN_ConcatLowdash(NAME, SuffixTemp) = *TOKEN_ConcatLowdash(NAME, SuffixPtr)++; APPLYFN(TOKEN_ConcatLowdash(NAME, SuffixTemp));  } while (TOKEN_ConcatLowdash(NAME, SuffixTemp))
  

#define REFLIST_AssignXrs(NAME, ASSGNVAL) \
  do {  TOKEN_ConcatLowdash(NAME, SuffixTemp) = *TOKEN_ConcatLowdash(NAME, SuffixPtr)++; *TOKEN_ConcatLowdash(NAME, SuffixPtr) = ASSGNVAL;  } while (TOKEN_ConcatLowdash(NAME, SuffixTemp)) 
  

#define BUFFER_ReferArray(ARRAY) &ARRAY[0]


#define SEND_MESSAGE(CHANNEL, BUFFER)                                          \
  mq_send(CHANNEL, (char *)BUFFER, sizeof(profinfo_t), NULL)
#define RECEIVE_MESSAGE(CHANNEL, BUFFER)                                       \
  mq_receive(CHANNEL, (char *)BUFFER, sizeof(profinfo_t), NULL)
#define EMIT_SENTINEL(FD)                                                      \
  do {                                                                         \
    QWORD sntnl = BINFILE_SENTINEL;                                            \
    write(FD, &sntnl, sizeof(QWORD));                                          \
  } while (0)
#define YIELD_IF_ERR(...)                                                      \
  if ((yield = __VA_ARGS__) < 0)                                               \
  return yield


#endif