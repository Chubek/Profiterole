#include <fcntl.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <sys/types.h>
#include <unistd.h>
#include "custardmacs.h"
#include "profiterole.h"

#ifdef ENABLE_ASM_FUNCS

elapsed_t get_elapsed_since(void) {
  elapsed_t cputime;
  asm volatile(ASM_ELAPSE : "=r"(cputime) : : CLB_ELAPSE);
  return cputime;
}

QWORD buffer_to_qword(const char *src) {
  QWORD qword;
  asm volatile(ASM_BUF2QW : "=r"(qword) : "r"(src) : CLB_IDQWRD);
  return qword;
}

nonyield_t qword_to_buffer(char *dst, const QWORD src) {
  asm volatile(ASM_QW2BUF : "=r"(dst) : "r"(src));
}

#else

elapsed_t get_elapsed_since(void) {
  struct tms proctime;
  times(&proctime);
  return proctime.tms_utime;
}

QWORD buffer_to_qword(const char *src) {
  QWORD qword = 0;
  MEMCOPYFN(&qword, src, sizeof(QWORD));
  return qword;
}

nonyield_t qword_to_buffer(char *dst, const QWORD src) {
  MEMCOPYFN(dst, &src, sizeof(QWORD));
}

#endif

nonyield_t hash_sentinel(const char *profname, QWORD *sentinelqword,
                         char *sentinelbytes) {
  char c;
  const char *profnamecpy = profname;
  sentinel_t sentinel = 0;

  while ((c = *profname++) &&
         (((QWORD)(profname - profnamecpy)) <= sizeof(QWORD)))
    sentinel = ((sentinel << 5) * sentinel) + c;

  *sentinelqword = sentinel;
  qword_to_buffer(sentinelbytes, sentinel);
}

nonyield_t poll_for_profile_and_serialize(profiler_t *prof) {
  UDWORD priority = 0;
  ssize_t rcvlen = 0;
  profinfo_t pinfo = (profinfo_t){0};
  WRITE_BINMAGIC(prof->serialoutfd);

  do {
    if (MESSAGE_RECEIVED)
      write(prof->serialoutfd, &pinfo, rcvlen);
  } while (!TERM_REQUESTED);

  WRITE_BINMAGIC(prof->serialoutfd);
  close(prof->serialoutfd);
  mq_close(prof->relegatemq);
  exit(EXIT_SUCCESS);
}

nonyield_t poll_for_info_and_profile(profiler_t *prof) {
  UDWORD priority = 0;
  QWORD bitmask = 0;
  ssize_t rcvlen = 0;
  profinfo_t pinfo = (profinfo_t){0};
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

yield_t init_profiler(profiler_t *prof, const char *profname,
                      const char *outpath, char *sentinelbytes) {
  yield_t yield;
  pid_t cpidser, cpidprof;
  hash_sentinel(profname, &prof->sentinel, sentinelbytes);
  YIELD_IF_ERR(prof->relegatemq =
                   mq_open(profname, O_CREAT | O_RDWR, S_IRWXU, NULL));
  YIELD_IF_ERR(prof->serialoutfd = open(outpath, O_CREAT | O_WRONLY, S_IRWXU));
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

yield_t queue_message_to_profiler(profiler_t *prof, const char *markerid) {
  yield_t yield;
  UDWORD priority = 0;
  markerid_t qwdid = buffer_to_qword(markerid);
  profinfo_t pinfo = (profinfo_t){.id = qwdid};
  YIELD_IF_ERR(SEND_MESSAGE(prof->relegatemq, &pinfo));
  return RETURN_SUCCESS;
}
