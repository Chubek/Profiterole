#define _POSIX_C_SOURCE 199309L
#include <stdint.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <mqueue.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>


#ifndef ELAPSE_ID_SENTINEL
#define ELAPSE_ID_SENTINEL 0xffffffff
#endif

#define BINFILE_MAGIC 0x74c787965cdbf970

#define SIGNUM_REACHED_PROFPOINT SIGRTMIN
#define SIGNUM_REACHED_SCRIPTEND RIGRTMIN + 1

#define RETURN_SUCCESS 0
#define RETURN_FAILURE -127

#define TERM_REQUESTED(ELAPSEID) (ELAPSEID == ELAPSE_ID_SENTINEL)
#define SEND_MESSAGE(CHANNEL, BUFFER) mq_send(CHANNEL, (char *)BUFFER, sizeof(elapsed_t), NULL)
#define RECEIVE_MESSAGE(CHANNEL, BUFFER) mq_receive(CHANNEL, (char *)BUFFER, sizeof(elapsed_t), NULL)

#define WAIT_FOR_SIGNALS(INFO, ...)                                     \
  do {                                                                         \
    int signal, signals[] = {__VA_ARGS__, 0}, *siptr = &signals[0];            \
    sigset_t signalset;                                                        \
    sigemptyset(&signalset);                                                   \
    while ((signal = *siptr++))                                                \
      sigaddset(&signalset, signal);                                           \
    sigwaitinfo(&signalset, INFO);                                     \
  } while (0)

#define WRITE_OUT_MAGIC(FD)                                                       \
  do {                                                                              \
    const unsigned long BINFILE_MAGIC_VAR = BINFILE_MAGIC;                   \
    write(FD, &BINFILE_MAGIC_VAR, sizeof(unsigned long));            \
  } while (0)

typedef unsigned long elpsid_t;
typedef unsigned long elpsamnt_t;
typedef int fld_t;
typedef int yield_t;

typedef struct ProfileIO {
    mqd_t serialmsgfd;
    fld_t serialoutfd;
    pid_t pidserializer;
    pid_t pidprofiler;
} profiler_t;

typedef struct Elapse {
    elpsid_t id;
    elpsamnt_t amount;
} elapsed_t;

#if defined(__amd64__) && !defined(NO_ASM_FUNC)
#define ELAPSE_AMOUNT_INSTRUCTION "rdtsc; movq %%rax, %0; shlq 32, %0; orq %%edx, %0;"
#define ELAPSE_ID_INSTRUCTION "movq (%1), %%r11; movq %%r11, %0"
#define CLOBBERED_ELAPSE_AMOUNT "rax", "rdx"
#define CLOBBERED_ELAPSE_ID "r11"
#elif defined(__aarch64__) && !defined(NO_ASM_FUNC)
#define ELAPSE_AMOUNT_INSTRUCTION "mrs %0, cntvct_el0"
#define ELAPSE_ID_INSTRUCTION "ldr %0, [%1]"
#define CLOBBERED_ELAPSE_AMOUNT "xzr"
#define CLOBBERED_ELAPSE_ID "xzr"
#elif !defined(NO_ASM_FUNC)
#error "Assembly function capabilities are enabled. However, either your machine is not an Intel or ARM one, or the macros are not defined."
#error "If you are on an ARM machine, such as Apple M1, please pass __aarch64__."
#error "If you are on an Intel or AMD machine, please pass __amd64__."
#error "In case of neither, please pass NO_ASM_FUNC"
#endif

#ifndef NO_ASM_FUNC

elpsamnt_t get_elapsed_amount() {
    elpsamnt_t cputime;
    asm volatile( ELAPSE_AMOUNT_INSTRUCTION 
        : "=r" (cputime)
        :
        : CLOBBERED_ELAPSE_AMOUNT);
    return cputime;
}

elpsid_t get_elapsed_id(char *profname) {
    elpsid_t qword;
    asm volatile( ELAPSE_ID_INSTRUCTION
            : "=r" (qword)
            : "r" (profname)
            : CLOBBERED_ELAPSE_ID);
    return qword;
}

#else

elpsamnt_t get_elapsed_amount() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_nsec;
}

elpsid_t get_elapsed_id(char *profname) {
    elpsid_t qword = 0;
    for (int i = 0; i < sizeof(elpsid_t); i++)
        qword |= profname[i] << (i << 3);
    return qword;
}

#endif


void wait_for_messages_and_write(profiler_t *prof) {
    ssize_t rcvlen;
    elapsed_t elapsed;
    WRITE_OUT_MAGIC(prof->serialoutfd);
    do {
        if ((rcvlen = RECEIVE_MESSAGE(prof->serialmsgfd, &elapsed)) > 0)
            write(prof->serialoutfd, &elapsed, rcvlen);
    } while (!TERM_REQUESTED(elapsed.id));
    close(prof->serialoutfd);
    mq_close(prof->serialmsgfd);
    exit(EXIT_SUCCESS);
}

void wait_for_signal_and_profile(profiler_t *prof) {
    elapsed_t elapsed = (elapsed_t) { 0 };
    siginfo_t signalinfo = (siginfo_t){ 0 };
    elpsamnt_t currelapse = 0, lastelapse = 0;
    do {
        WAIT_FOR_SIGNALS(&signalinfo, SIGNUM_REACHED_PROFPOINT);
        currelapse = get_elapsed_amount();
        elapsed.id = signalinfo.si_value.sival_int;
        elapsed.amount = currelapse - lastelapse;
        lastelapse = currelapse;
        SEND_MESSAGE(prof->serialmsgfd, &elapsed);
    } while (!TERM_REQUESTED(elapsed.id));
    close(prof->serialoutfd);
    mq_close(prof->serialmsgfd);
    exit(EXIT_SUCCESS);
}

yield_t send_signal_to_profiler(profiler_t *prof, char *pointid) {
    elpsid_t elapseid = get_elapsed_id(pointid);
    return sigqueue(prof->pidprofiler, SIGNUM_REACHED_PROFPOINT, (union sigval){ .sival_int = elapseid });
}

yield_t init_profiler(profiler_t *prof, char *profname, char *outpath) {
    yield_t yield;
    pid_t cpid1, cpid2;
    if ((yield = prof->serialmsgfd = mq_open(profname, O_CREAT)) < 0)
        return yield;
    else if ((yield = prof->serialoutfd = open(outpath, O_CREAT)) < 0)
        return yield;
    else if ((yield  = prof->pidserializer = cpid1 = fork()) < 0)
        return yield;
    else if ((yield = prof->pidprofiler = cpid2 = fork()) < 0)
        return yield;
    
    if (!cpid1 || !cpid2) {
        if (!cpid1) {
            wait_for_messages_and_write(prof);
        }

        if (!cpid2) {
            wait_for_signal_and_profile(prof);
        }    
    } else {
        return RETURN_SUCCESS;
    }

    return RETURN_FAILURE;
}

