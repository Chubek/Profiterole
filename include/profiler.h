#ifndef PROFILER_H
#define PROFILER_H

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif

#define QWORD_MAX 0xffffffffffffffff

#define QWORD unsigned long
#define DWORD int
#define UDWORD unsigned int

#define RETURN_SUCCESS 1
#define RETURN_FAILURE 0

#define BINFILE_SENTINEL 0x150c55b5052464cf

#define MESSAGE_RECEIVED                                                       \
  (rcvlen = RECEIVE_MESSAGE(prof->relegatemq, &pinfo)) == sizeof(profinfo_t)
#define TERM_REQUESTED (pinfo.id == prof->sentinel)

#if defined(__amd64__) && !defined(NO_ASM_FUNC)
#define ASM_ELAPSE "rdtsc; movq %%rax, %0; shlq $32, %0; orq %%rdx, %0;"
#define ASM_BUF2QW "movq (%1), %%r11; movq %%r11, %0"
#define ASM_QW2BUF "movq %1, (%0);"
#define CLB_ELAPSE "rax", "rdx"
#define CLB_IDQWRD "r11"
#elif defined(__aarch64__) && !defined(NO_ASM_FUNC)
#define ASM_ELAPSE "mrs %0, cntvct_el0;"
#define ASM_BUF2QW "ldr %0, [%1];"
#define ADM_QW2BUF "str %0, [%1];"
#define CLB_ELAPSE "xzr"
#define CLB_IDQWRD "xzr"
#elif !defined(NO_ASM_FUNC)
#error                                                                         \
    "Assembly function capabilities are enabled. However, either your machine is not an Intel or ARM one, or the macros are not defined."
#error                                                                         \
    "If you are on an ARM machine, such as Apple M1, please pass __aarch64__."
#error "If you are on an Intel or AMD machine, please pass __amd64__."
#error "In neither case, please pass NO_ASM_FUNC"
#endif

#if defined(NO_ASM_FUNC) && defined(USE_MEMCPY)
#define MEMCOPYFN memcpy
#elif defined(NO_ASM_FUNC) && (defined(USE_MEMMOVE) || !defined(USE_MEMCPY)) && !defined(CUSTOM_MEMCOPYFN)
#define MEMCOPYFN memmove
#elif defined(CUSTOM_MEMCOPYFN)
#ifndef NO_WARNING
#warning "The prototype for MEMCOPYFN must be (ADDR dst, ADDR src, INT len)"
#endif  
#define MEMCOPYFN CUSTOM_MEMCOPYFN
#endif

typedef QWORD sentinel_t;
typedef QWORD markerid_t;
typedef QWORD elapsed_t;
typedef QWORD snl_t;
typedef DWORD fld_t;
typedef DWORD yield_t;
typedef void nonyield_t;

typedef struct Profiler {
  mqd_t relegatemq;
  fld_t serialoutfd;
  pid_t pidserializer;
  pid_t pidprofiler;
  snl_t sentinel;
} profiler_t;

typedef struct ProfileInfo {
  markerid_t id;
  elapsed_t elapsed;
} profinfo_t;

elapsed_t get_elapsed_since(void);
markerid_t buffer_to_qword(const char *src);
nonyield_t qword_to_buffer(char *dst, const QWORD src);

nonyield_t hash_sentinel(const char *profname, QWORD *sentinelqword, char *sentinelbytes);

nonyield_t poll_for_profile_and_serialize(profiler_t *prof);
nonyield_t poll_for_info_and_profile(profiler_t *prof);

yield_t init_profiler(profiler_t *prof, const char *profname, const char *outpath, char * sentinelbytes);
yield_t queue_message_to_profiler(profiler_t *prof, const char *markerid);

#define SEND_MESSAGE(CHANNEL, BUFFER)                                          \
  mq_send(CHANNEL, (char *)BUFFER, sizeof(profinfo_t), priority)
#define RECEIVE_MESSAGE(CHANNEL, BUFFER)                                       \
  mq_receive(CHANNEL, (char *)BUFFER, sizeof(profinfo_t), &priority)
#define WRITE_BINMAGIC(FD)                                                      \
  do {                                                                         \
    QWORD sntnl = BINFILE_SENTINEL;                                            \
    write(FD, &sntnl, sizeof(QWORD));                                          \
  } while (0)
#define YIELD_IF_ERR(...)                                                      \
  if ((yield = __VA_ARGS__) < 0)                                               \
  return yield



#endif