#ifndef PROFILER_H
#define PROFILER_H

#define QWORD_prf unsigned long
#define DWORD_prf int
#define UDWORD_prf unsigned int

typedef QWORD_prf sentinel_t;
typedef QWORD_prf markerid_t;
typedef QWORD_prf elapsed_t;
typedef QWORD_prf snl_t;
typedef DWORD_prf fld_t;
typedef DWORD_prf yield_t;
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
nonyield_t qword_to_buffer(char *dst, const QWORD_prf src);

nonyield_t hash_sentinel(const char *profname, QWORD_prf *sentinelqword,
                         char *sentinelbytes);

nonyield_t poll_for_profile_and_serialize(profiler_t *prof);
nonyield_t poll_for_info_and_profile(profiler_t *prof);

yield_t init_profiler(profiler_t *prof, const char *profname,
                      const char *outpath, char *sentinelbytes);
yield_t queue_message_to_profiler(profiler_t *prof, const char *markerid);

#ifndef SIZEOF_LONG
#define SIZEOF_LONG 8
#endif

#define PROFITEROLE_DECLARE(NAME) profiler_t NAME##_profiler; char NAME##_sentinel[SIZEOF_LONG]
#define PROFITEROLE_INIT(NAME, OUTPATH) init_profiler(&NAME##_profiler, #NAME, OUTPATH, &NAME##_sentinel[0])
#define PROFITEROLE_MARKER(NAME, MARKERID) queue_message_to_profiler(&NAME##_profiler, MARKERID)
#define PROFITEROLE_TERMINATE(NAME) queue_message_to_profiler(&NAME##_profiler, &NAME##_sentinel[0])

#endif