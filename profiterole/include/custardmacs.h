#ifndef CUSTARDMACS_H
#define CUSTARDMACS_H

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif

#define QWORD_MAX 0xffffffffffffffff

#ifndef QWORD
#define QWORD unsigned long
#endif
#ifndef DWORD
#define DWORD int
#endif
#ifndef UDWORD
#define UDWORD unsigned int
#endif

#ifdef MUST_SET_MQD_T
typedef int mqd_t
#endif

#define RETURN_SUCCESS 1
#define RETURN_FAILURE 0

#define BINFILE_SENTINEL 0x150c55b5052464cf

#define MESSAGE_RECEIVED                                                       \
  (rcvlen = RECEIVE_MESSAGE(prof->relegatemq, &pinfo)) == sizeof(profinfo_t)
#define TERM_REQUESTED (pinfo.id == prof->sentinel)

#if WANT_ASM_FUNCS == 0 && WANT_MEMCPY == 1
#define MEMCOPYFN memcpy
#elif CUST_MEMCPYFN
#define MEMCOPYFN CUST_MEMCPYFN
#else
#define MEMCOPYFN memmove
#endif

#define SEND_MESSAGE(CHANNEL, BUFFER)                                          \
  mq_send(CHANNEL, (char *)BUFFER, sizeof(profinfo_t), priority)
#define RECEIVE_MESSAGE(CHANNEL, BUFFER)                                       \
  mq_receive(CHANNEL, (char *)BUFFER, sizeof(profinfo_t), &priority)
#define WRITE_BINMAGIC(FD)                                                     \
  do {                                                                         \
    QWORD sntnl = BINFILE_SENTINEL;                                            \
    write(FD, &sntnl, sizeof(QWORD));                                          \
  } while (0)
#define YIELD_IF_ERR(...)                                                      \
  if ((yield = __VA_ARGS__) < 0)                                               \
  return yield

#if defined(__amd64__) && WANT_ASM_FUNCS == 1
#define ASM_ELAPSE "rdtsc; movq %%rax, %0; shlq $32, %0; orq %%rdx, %0;"
#define ASM_BUF2QW "movq (%1), %%r11; movq %%r11, %0"
#define ASM_QW2BUF "movq %1, (%0);"
#define CLB_ELAPSE "rax", "rdx"
#define CLB_IDQWRD "r11"
#elif defined(__aarch64__) && WANT_ASM_FUNCS == 1
#define ASM_ELAPSE "mrs %0, cntvct_el0;"
#define ASM_BUF2QW "ldr %0, [%1];"
#define ADM_QW2BUF "str %0, [%1];"
#define CLB_ELAPSE "xzr"
#define CLB_IDQWRD "xzr"
#elif (!defined(__amd64__) && !defined(__aarch64__)) && WANT_ASM_FUNCS == 1
#error "Neither an Intel/AMD or ARM64 CPU macro detected, please disable ENABLE_ASM_FUNCS"
#endif