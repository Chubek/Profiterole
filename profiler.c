#include "profiler.h"

#ifndef NO_ASM_FUNC

elapsed_t get_elapsed_since(NILTY) {
    elapsed_t cputime;
    asm volatile( ASM_ELAPSE 
        : "=r" (cputime)
        :
        : CLB_ELAPSE);
    return cputime;
}

markerid_t byteptr_to_qword(char *markerid) {
    markerid_t qword;
    asm volatile( ASM_IDQWRD
            : "=r" (qword)
            : "r" (markerid)
            : CLB_IDQWRD);
    return qword;
}

#else

elapsed_t get_elapsed_since(NILTY) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_nsec;
}

markerid_t byteptr_to_qword(char *markerid) {
    markerid_t qword = 0;
    for (int i = 0; i < sizeof(markerid_t); i++)
        qword |= markerid[i] << (i << 3);
    return qword;
}

#endif


nonyield_t poll_for_profile_and_serialize(profiler_t *prof) {
    ssize_t rcvlen = 0;
    profinfo_t pinfo = (profinfo_t) { 0 };
    WRITE_OUT_MAGIC(prof->serialoutfd);

    do {
        if (MESSAGE_RECEIVED)
            write(prof->serialoutfd, &pinfo, rcvlen);
    } while (!TERM_REQUESTED);

    close(prof->serialoutfd);
    mq_close(prof->relegatemq);
    exit(EXIT_SUCCESS);
}

nonyield_t poll_for_info_and_profile(profiler_t *prof) {
    volatile bitmask_t bitmask = 0;
    ssize_t rcvlen = 0;
    profinfo_t pinfo = (profinfo_t) { 0 };
    elapsed_t curlapse = 0, prvlapse = 0;

    do {
        if (MESSAGE_RECEIVED) {
            curlapse = get_elapsed_since();
            pinfo.elapsed = (curlapse - prvlapse) & bitmask;
            prvlapse = curlapse;
            bitmask = QWORD_MAX;
            SEND_MESSAGE(prof->relegatemq, &pinfo);
        }
    } while (!TERM_REQUESTED);

    close(prof->serialoutfd);
    mq_close(prof->relegatemq);
    exit(EXIT_SUCCESS);
}

yield_t init_profiler(profiler_t *prof, char *profname, char *outpath) {
    yield_t yield;
    pid_t cpidser, cpidprof;
    YIELD_IF_ERR(prof->relegatemq = mq_open(profname, O_CREAT | O_RDWR));
    YIELD_IF_ERR(prof->serialoutfd = open(outpath, O_CREAT | O_RDWR));
    YIELD_IF_ERR(prof->pidserializer = cpidser = fork());
    YIELD_IF_ERR(prof->pidprofiler = cpidprof = fork());

    if (!cpidser || !cpidprof) {
        if (!cpidser) {
            poll_for_profile_and_serialize(prof);
        } else if (!cpidprof) {
            poll_for_info_and_profile(prof);
        }    
    } else {
        return RETURN_SUCCESS;
    }

    return RETURN_FAILURE;
}

yield_t queue_message_to_profiler(profiler_t *prof, char *markerid) {
    yield_t yield;
    markerid_t qwdid = byteptr_to_qword(markerid);
    profinfo_t pinfo = (profinfo_t){ .id = qwdid };
    YIELD_IF_ERR(SEND_MESSAGE(prof->relegatemq, &pinfo));
    return RETURN_SUCCESS;
}

yield_t terminate_profiler(profiler_t *prof) {
    yield_t yield;
    profinfo_t pinfo = (profinfo_t){ .id = (markerid_t)-1 };
    YIELD_IF_ERR(SEND_MESSAGE(prof->relegatemq, &pinfo));
    return RETURN_SUCCESS;
}

