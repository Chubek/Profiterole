

#define BINFILE_MAGIC 0x74c787965cdbf970

#define RETURN_SUCCESS 0
#define RETURN_FAILURE -127

#define QWORD unsigned long
#define DWORD int

#define TERM_REQUESTED(FLAG) (FLAG < 0)
#define SEND_MESSAGE(CHANNEL, BUFFER) mq_send(CHANNEL, (char *)BUFFER, sizeof(profinfo_t), NULL)
#define RECEIVE_MESSAGE(CHANNEL, BUFFER) mq_receive(CHANNEL, (char *)BUFFER, sizeof(profinfo_t), NULL)

#define WRITE_OUT_MAGIC(FD)                                                       \
  do {                                                                              \
    const unsigned long BINFILE_MAGIC_VAR = BINFILE_MAGIC;                   \
    write(FD, &BINFILE_MAGIC_VAR, sizeof(unsigned long));            \
  } while (0)

typedef QWORD markerid_t;
typedef QWORD elapsed_t;
typedef QWORD iflags_t;
typedef DWORD fld_t;
typedef DWORD yield_t;

typedef struct ProfileIO {
    mqd_t relegatemq;
    fld_t serialoutfd;
    pid_t pidserializer;
    pid_t pidprofiler;
} profiler_t;

typedef struct ProfileInfo {
    markerid_t id;
    elapsed_t elapsed;
    iflags_t flags;
} profinfo_t;

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

elapsed_t get_elapsed_since() {
    elapsed_t cputime;
    asm volatile( ELAPSE_AMOUNT_INSTRUCTION 
        : "=r" (cputime)
        :
        : CLOBBERED_ELAPSE_AMOUNT);
    return cputime;
}

markerid_t byteptr_to_qword(char *profname) {
    markerid_t qword;
    asm volatile( ELAPSE_ID_INSTRUCTION
            : "=r" (qword)
            : "r" (profname)
            : CLOBBERED_ELAPSE_ID);
    return qword;
}

#else

elapsed_t get_elapsed_since() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_nsec;
}

markerid_t byteptr_to_qword(char *profname) {
    markerid_t qword = 0;
    for (int i = 0; i < sizeof(markerid_t); i++)
        qword |= profname[i] << (i << 3);
    return qword;
}

#endif


void poll_for_profile_and_serialize(profiler_t *prof) {
    ssize_t rcvlen = 0;
    profinfo_t pinfo = (profinfo_t) { 0 };
    WRITE_OUT_MAGIC(prof->serialoutfd);
    do {
        if ((rcvlen = RECEIVE_MESSAGE(prof->relegatemq, &pinfo)) > 0)
            write(prof->serialoutfd, &pinfo, rcvlen);
    } while (!TERM_REQUESTED(pinfo.flag));
    close(prof->serialoutfd);
    mq_close(prof->relegatemq);
    exit(EXIT_SUCCESS);
}

void poll_for_info_and_profile(profiler_t *prof) {
    ssize_t rcvlen = 0;
    profinfo_t pinfo = (profinfo_t) { 0 };
    elapsed_t currelapse = 0, lastelapse = 0;
    do {
        if ((rcvlen = RECEIVE_MESSAGE(prof->relegatemq, &pinfo)) > 0) {
            currelapse = get_elapsed_since();
            pinfo.id = signalinfo.si_value.sival_int;
            pinfo.elapsed = currelapse - lastelapse;
            lastelapse = currelapse;
            SEND_MESSAGE(prof->relegatemq, &pinfo);
        }
    } while (!TERM_REQUESTED(pinfo.flag));
    close(prof->serialoutfd);
    mq_close(prof->relegatemq);
    exit(EXIT_SUCCESS);
}

yield_t queue_message_to_profiler(profiler_t *prof, char *markerid, iflags_t flags) {
    markerid_t qwdid = byteptr_to_qword(markerid);
    profinfo_t pinfo = (profinfo_t){ .id = qwdid, .flags = flags };
    return SEND_MESSAGE(prof->relegatemq, &pinfo);
}

yield_t init_profiler(profiler_t *prof, char *profname, char *outpath) {
    yield_t yield;
    pid_t cpid1, cpid2;
    if ((yield = prof->relegatemq = mq_open(profname, O_CREAT | O_RDWR)) < 0)
        return yield;
    else if ((yield = prof->serialoutfd = open(outpath, O_CREAT | O_RDWR)) < 0)
        return yield;
    else if ((yield  = prof->pidserializer = cpid1 = fork()) < 0)
        return yield;
    else if ((yield = prof->pidprofiler = cpid2 = fork()) < 0)
        return yield;
    
    if (!cpid1 || !cpid2) {
        if (!cpid1) {
            poll_for_profile_and_serialize(prof);
        }

        if (!cpid2) {
            poll_for_info_and_profile(prof);
        }    
    } else {
        return RETURN_SUCCESS;
    }

    return RETURN_FAILURE;
}

