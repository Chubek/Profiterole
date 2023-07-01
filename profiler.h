#define _POSIX_C_SOURCE 199309L
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <mqueue.h>
#include <sys/types.h>

#define QWORD unsigned long
#define DWORD int
#define NILTY void

#define RETURN_SUCCESS 0
#define RETURN_FAILURE -127

#ifndef BINFILE_MAGIC
#define BINFILE_MAGIC 0x50524f464954524c0a
#endif

#define TERM_REQUESTED(FLAG) ((signed long)FLAG == -1)
#define SEND_MESSAGE(CHANNEL, BUFFER) mq_send(CHANNEL, (char *)BUFFER, sizeof(profinfo_t), NULL)
#define RECEIVE_MESSAGE(CHANNEL, BUFFER) mq_receive(CHANNEL, (char *)BUFFER, sizeof(profinfo_t), NULL)
#define WRITE_OUT_MAGIC(FD)                                                       \
  do {                                                                              \
    const unsigned long BINFILE_MAGIC_VAR = BINFILE_MAGIC;                   \
    write(FD, &BINFILE_MAGIC_VAR, sizeof(unsigned long));            \
  } while (0)
#define YIELD_IF_ERR(...)			\
  if ((yield = __VA_ARGS__) < 0)		\
  	return yeild

#if defined(__amd64__) && !defined(NO_ASM_FUNC)
#define ASM_ELAPSE "rdtsc; movq %%rax, %0; shlq 32, %0; orq %%edx, %0;"
#define ASM_IDQWRD "movq (%1), %%r11; movq %%r11, %0"
#define CLB_ELAPSE "rax", "rdx"
#define CLB_IDQWRD "r11"
#elif defined(__aarch64__) && !defined(NO_ASM_FUNC)
#define ASM_ELAPSE "mrs %0, cntvct_el0"
#define ASM_IDQWRD "ldr %0, [%1]"
#define CLB_ELAPSE "xzr"
#define CLB_IDQWRD "xzr"
#elif !defined(NO_ASM_FUNC)
#error "Assembly function capabilities are enabled. However, either your machine is not an Intel or ARM one, or the macros are not defined."
#error "If you are on an ARM machine, such as Apple M1, please pass __aarch64__."
#error "If you are on an Intel or AMD machine, please pass __amd64__."
#error "In neither case, please pass NO_ASM_FUNC"
#endif

typedef QWORD markerid_t;
typedef QWORD elapsed_t;
typedef DWORD fld_t;
typedef DWORD yield_t;
typedef NILTY nonyield_t;

typedef struct ProfileIO {
    mqd_t relegatemq;
    fld_t serialoutfd;
    pid_t pidserializer;
    pid_t pidprofiler;
} profiler_t;

typedef struct ProfileInfo {
    markerid_t id;
    elapsed_t elapsed;
} profinfo_t;

elapsed_t get_elapsed_since(NILTY);
markerid_t byteptr_to_qword(char *markerid);

nonyield_t poll_for_profile_and_serialize(profiler_t *prof);
nonyield_t poll_for_info_and_profile(profiler_t *prof);

yield_t init_profiler(profiler_t *prof, char *profname, char *outpath);
yield_t queue_message_to_profiler(profiler_t *prof, char *markerid);
yield_t terminate_profiler(profiler_t *prof);

